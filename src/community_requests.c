#include "global.h"
#include "main.h"
#include "bg.h"
#include "community_requests.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "gym_challenge.h"
#include "international_string_util.h"
#include "item.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "data/community_requests.h"

// Community Requests: the city posts sidequests on the gym's mailboard. Each
// request owns one byte of SaveBlock3 (status plus an optional step counter);
// everything else about it — name, blurb, zone, rewards — is const data in
// src/data/community_requests.h.
//
// Most requests are posted when the gym reaches a rank
// (CommunityRequests_UnlockForRank, called from GymChallenge_RankUp); the
// rest wait for a map script to post them. Requests that exist but are not
// posted yet show on the board as "???".
//
// The board itself is a read-only browser modeled on the TM Machine
// (src/tm_machine.c): list on the left, details on the right, blurb along
// the bottom, L/R to swap between posted and completed requests.

static const u8 sText_Space[] = _(" ");
static const u8 sText_RewardJoin[] = _(" and ");

static const struct CommunityRequest *GetRequest(u32 id)
{
    if (id >= NUM_COMMUNITY_REQUESTS || sCommunityRequests[id].name == NULL)
        return NULL;
    return &sCommunityRequests[id];
}

u32 CommunityRequests_GetStatus(u32 id)
{
    if (id >= NUM_COMMUNITY_REQUESTS)
        return REQUEST_STATUS_LOCKED;
    return gSaveBlock3Ptr->requests[id] & REQUEST_STATUS_MASK;
}

u32 CommunityRequests_GetStep(u32 id)
{
    if (id >= NUM_COMMUNITY_REQUESTS)
        return 0;
    return gSaveBlock3Ptr->requests[id] >> REQUEST_STEP_SHIFT;
}

static void SetStatus(u32 id, u32 status)
{
    u8 *slot = &gSaveBlock3Ptr->requests[id];

    *slot = (*slot & ~REQUEST_STATUS_MASK) | status;
}

static void SetStep(u32 id, u32 step)
{
    u8 *slot = &gSaveBlock3Ptr->requests[id];

    if (step > REQUEST_MAX_STEPS)
        step = REQUEST_MAX_STEPS;
    *slot = (*slot & REQUEST_STATUS_MASK) | (step << REQUEST_STEP_SHIFT);
}

// Posts every rank-gated request the gym has reached and not seen yet. Called
// on rank-up and on a new game; requests already active or done are left
// alone, so a missed call is caught by the next one.
void CommunityRequests_UnlockForRank(u32 rank)
{
    u32 i;

    for (i = 0; i < NUM_COMMUNITY_REQUESTS; i++)
    {
        const struct CommunityRequest *request = GetRequest(i);

        if (request == NULL || request->unlockRank == REQUEST_UNLOCK_SCRIPT || request->unlockRank > rank)
            continue;
        if (CommunityRequests_GetStatus(i) == REQUEST_STATUS_LOCKED)
            SetStatus(i, REQUEST_STATUS_ACTIVE);
    }
}

// Script specials. All take the request id in gSpecialVar_0x8004; unwritten
// or out-of-range ids are inert rather than fatal.

void CommunityRequests_GetRequestStatus(void)
{
    gSpecialVar_Result = CommunityRequests_GetStatus(gSpecialVar_0x8004);
}

void CommunityRequests_GetRequestStep(void)
{
    gSpecialVar_Result = CommunityRequests_GetStep(gSpecialVar_0x8004);
}

// Posts a request that waits on a map script. VAR_RESULT receives TRUE if
// this call is what posted it.
void CommunityRequests_UnlockRequest(void)
{
    u32 id = gSpecialVar_0x8004;

    gSpecialVar_Result = FALSE;
    if (GetRequest(id) == NULL || CommunityRequests_GetStatus(id) != REQUEST_STATUS_LOCKED)
        return;
    SetStatus(id, REQUEST_STATUS_ACTIVE);
    gSpecialVar_Result = TRUE;
}

// Advances a multi-step request by one, stopping at its last step.
// VAR_RESULT receives the step the request is on afterwards.
void CommunityRequests_AdvanceStep(void)
{
    u32 id = gSpecialVar_0x8004;
    const struct CommunityRequest *request = GetRequest(id);
    u32 step;

    gSpecialVar_Result = 0;
    if (request == NULL || CommunityRequests_GetStatus(id) != REQUEST_STATUS_ACTIVE)
        return;
    step = CommunityRequests_GetStep(id) + 1;
    if (step > request->numSteps)
        step = request->numSteps;
    SetStep(id, step);
    gSpecialVar_Result = step;
}

// VAR_RESULT receives whether the bag can take the request's item rewards, so
// a script can hold the reward back instead of dropping items on the floor.
void CommunityRequests_HasRoomForRewards(void)
{
    const struct CommunityRequest *request = GetRequest(gSpecialVar_0x8004);
    u32 i;

    gSpecialVar_Result = FALSE;
    if (request == NULL)
        return;
    gSpecialVar_Result = TRUE;
    for (i = 0; i < REQUEST_NUM_REWARD_ITEMS; i++)
    {
        if (request->items[i].itemId == ITEM_NONE)
            continue;
        if (!CheckBagHasSpace(request->items[i].itemId, request->items[i].quantity))
        {
            gSpecialVar_Result = FALSE;
            return;
        }
    }
}

// "3 GREAT BALLs", or just "POTION" for a single one.
static void AppendRewardName(u8 *dest, const struct CommunityRequestReward *reward)
{
    u8 buffer[ITEM_NAME_LENGTH + 10];

    if (reward->quantity > 1)
    {
        ConvertIntToDecimalStringN(buffer, reward->quantity, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringAppend(dest, buffer);
        StringAppend(dest, sText_Space);
    }
    CopyItemNameHandlePlural(reward->itemId, buffer, reward->quantity);
    StringAppend(dest, buffer);
}

// Marks a request completed and hands over the whole reward at once: money
// plus every item stack. The payout is one event rather than one per item, so
// the script can announce it with a single fanfare and a single message.
//
// STR_VAR_1 receives the money, STR_VAR_2 the item stacks as readable text,
// and VAR_0x8006 how many stacks made it into the bag (0 means money only, so
// the script knows which message to print).
void CommunityRequests_CompleteRequest(void)
{
    u32 id = gSpecialVar_0x8004;
    const struct CommunityRequest *request = GetRequest(id);
    u32 i;
    u32 given = 0;

    gSpecialVar_Result = FALSE;
    gSpecialVar_0x8006 = 0;
    if (request == NULL || CommunityRequests_GetStatus(id) != REQUEST_STATUS_ACTIVE)
        return;
    SetStatus(id, REQUEST_STATUS_DONE);
    SetStep(id, request->numSteps);
    AddMoney(&gSaveBlock1Ptr->money, request->money);
    ConvertIntToDecimalStringN(gStringVar1, request->money, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);

    gStringVar2[0] = EOS;
    for (i = 0; i < REQUEST_NUM_REWARD_ITEMS; i++)
    {
        if (request->items[i].itemId == ITEM_NONE)
            continue;
        // The caller checks CommunityRequests_HasRoomForRewards first; a stack
        // that still doesn't fit is left out of the message rather than
        // announced and never given.
        if (!AddBagItem(request->items[i].itemId, request->items[i].quantity))
            continue;
        if (given > 0)
            StringAppend(gStringVar2, sText_RewardJoin);
        AppendRewardName(gStringVar2, &request->items[i]);
        given++;
    }

    gSpecialVar_0x8006 = given;
    gSpecialVar_Result = TRUE;
}

// ---------------------------------------------------------------------------
// The mailboard menu
// ---------------------------------------------------------------------------

enum
{
    WIN_LIST,
    WIN_INFO,
    WIN_MSG,
};

enum
{
    TAB_IN_PROGRESS,
    TAB_FINISHED,
    TAB_COUNT,
};

#define TAG_LIST_ARROWS 5426

#define INFO_WIDTH 104 // WIN_INFO's usable width in pixels.

static EWRAM_DATA struct CommunityRequestBoard
{
    struct ListMenuItem menuItems[NUM_COMMUNITY_REQUESTS + 1];
    u8 mainTask;
    u8 listMenuTask;
    u8 scrollArrowTask;
    u8 tab;
    u16 numMenuItems;
    u16 numToShowAtOnce;
    u16 listOffset;
    u16 listRow;
} *sBoard = NULL;

static void CB2_InitBoard(void);
static void CB2_BoardMain(void);
static void Task_Board_HandleInput(u8 taskId);
static void Task_Board_Quit(u8 taskId);
static void BuildRequestList(void);
static void PrintRequestInfo(s32 id);
static void AddListScrollArrows(void);
static void RemoveListScrollArrows(void);

static const u8 sText_Unknown[] = _("???");
static const u8 sText_TabInProgress[] = _("IN PROGRESS");
static const u8 sText_TabFinished[] = _("COMPLETED");
static const u8 sText_ZoneLabel[] = _("ZONE");
static const u8 sText_RewardLabel[] = _("REWARD");
static const u8 sText_StepFmt[] = _("STEP {STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_MoneyFmt[] = _("¥{STR_VAR_1}");
static const u8 sText_RewardItemFmt[] = _("{STR_VAR_1} x{STR_VAR_2}");
static const u8 sText_BoardHint[] = _("L/R: switch list    B: close the board");
static const u8 sText_NothingPosted[] = _("Nothing posted here right now.");
static const u8 sText_NotPostedYet[] = _("The city hasn't asked for this one\nyet.");

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_LIST] = {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 13,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0xA
    },
    [WIN_INFO] = {
        .bg = 1,
        .tilemapLeft = 16,
        .tilemapTop = 1,
        .width = 13,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0xA6
    },
    [WIN_MSG] = {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x142
    },
    DUMMY_WIN_TEMPLATE
};

static const struct ScrollArrowsTemplate sListScrollArrowsTemplate =
{
    .firstArrowType = SCROLL_ARROW_UP,
    .firstX = 60,
    .firstY = 8,
    .secondArrowType = SCROLL_ARROW_DOWN,
    .secondX = 60,
    .secondY = 104,
    .fullyUpThreshold = 0,
    .fullyDownThreshold = 0,
    .tileTag = TAG_LIST_ARROWS,
    .palTag = TAG_LIST_ARROWS,
    .palNum = 0,
};

static void RequestListCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu *list);

static const struct ListMenuTemplate sRequestListMenuTemplate =
{
    .items = NULL,
    .moveCursorFunc = RequestListCursorCallback,
    .itemPrintFunc = NULL,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = WIN_LIST,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 0,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_NORMAL,
    .cursorKind = CURSOR_BLACK_ARROW,
    .textNarrowWidth = 96,
};

static void VBlankCB_Board(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

// Script special: opens the board. The calling script fades out first and
// waits; quitting resumes it.
//
// Posts anything the gym's rank has earned but that was never posted — a save
// that predates a request, or one made before this system existed, catches up
// the first time the leader reads the board.
void CommunityRequests_OpenBoard(void)
{
    CommunityRequests_UnlockForRank(GymChallenge_GetRank());
    CB2_InitBoard();
}

static const u8 *GetTabName(void)
{
    return sBoard->tab == TAB_FINISHED ? sText_TabFinished : sText_TabInProgress;
}

// The IN PROGRESS list also carries the requests the city hasn't posted yet,
// as "???" — the board is how the player sees there is more coming.
static void BuildRequestList(void)
{
    u32 i;

    sBoard->numMenuItems = 0;
    for (i = 0; i < NUM_COMMUNITY_REQUESTS; i++)
    {
        u32 status = CommunityRequests_GetStatus(i);

        if (GetRequest(i) == NULL)
            continue;
        if (sBoard->tab == TAB_FINISHED)
        {
            if (status != REQUEST_STATUS_DONE)
                continue;
            sBoard->menuItems[sBoard->numMenuItems].name = sCommunityRequests[i].name;
        }
        else
        {
            if (status == REQUEST_STATUS_DONE)
                continue;
            sBoard->menuItems[sBoard->numMenuItems].name =
                (status == REQUEST_STATUS_ACTIVE) ? sCommunityRequests[i].name : sText_Unknown;
        }
        sBoard->menuItems[sBoard->numMenuItems].id = i;
        sBoard->numMenuItems++;
    }
    sBoard->menuItems[sBoard->numMenuItems].name = gText_Cancel;
    sBoard->menuItems[sBoard->numMenuItems].id = LIST_CANCEL;
    sBoard->numMenuItems++;

    gMultiuseListMenuTemplate = sRequestListMenuTemplate;
    gMultiuseListMenuTemplate.totalItems = sBoard->numMenuItems;
    gMultiuseListMenuTemplate.items = sBoard->menuItems;
    gMultiuseListMenuTemplate.maxShowed = min(sBoard->numMenuItems, 6);
    sBoard->numToShowAtOnce = gMultiuseListMenuTemplate.maxShowed;

    if (sBoard->listOffset + sBoard->listRow >= sBoard->numMenuItems)
    {
        sBoard->listOffset = 0;
        sBoard->listRow = 0;
    }
}

static void InitBoardWindows(void)
{
    u32 i;

    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(0, 1, BG_PLTT_ID(14));
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);

    for (i = 0; i < ARRAY_COUNT(sWindowTemplates) - 1; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(1));
        PutWindowTilemap(i);
        DrawStdFrameWithCustomTileAndPalette(i, FALSE, 1, 0xE);
    }
    ScheduleBgCopyTilemapToVram(1);
}

static void InitBoardBgs(void)
{
    ResetVramOamAndBgCntRegs();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    ResetAllBgsCoordinates();
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 |
                                  DISPCNT_OBJ_1D_MAP |
                                  DISPCNT_OBJ_ON);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
}

static void CB2_InitBoard_Internal(void)
{
    switch (gMain.state)
    {
    case 0:
        ResetSpriteData();
        FreeAllSpritePalettes();
        ClearScheduledBgCopiesToVram();
        SetVBlankCallback(VBlankCB_Board);
        gMain.state++;
        break;
    case 1:
        InitBoardBgs();
        InitBoardWindows();
        gMain.state++;
        break;
    case 2:
        BuildRequestList();
        sBoard->listMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sBoard->listOffset, sBoard->listRow);
        gMain.state++;
        break;
    case 3:
        SetBackdropFromColor(RGB_BLACK);
        BeginNormalPaletteFade(PALETTES_ALL, -2, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 4:
        UpdatePaletteFade();
        if (!gPaletteFade.active)
            gMain.state++;
        break;
    default:
        AddListScrollArrows();
        gTasks[sBoard->mainTask].func = Task_Board_HandleInput;
        SetVBlankCallback(VBlankCB_Board);
        SetMainCallback2(CB2_BoardMain);
        break;
    }
}

static void CB2_InitBoard(void)
{
    ResetTasks();
    sBoard = AllocZeroed(sizeof(*sBoard));
    sBoard->mainTask = CreateTask(TaskDummy, 1);
    sBoard->scrollArrowTask = TASK_NONE;
    SetMainCallback2(CB2_InitBoard_Internal);
}

static void CB2_BoardMain(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void FreeBoardResources(void)
{
    RemoveListScrollArrows();
    DestroyListMenuTask(sBoard->listMenuTask, &sBoard->listOffset, &sBoard->listRow);
    FreeAllWindowBuffers();
    FREE_AND_SET_NULL(sBoard);
    ResetSpriteData();
    FreeAllSpritePalettes();
}

static void AddListScrollArrows(void)
{
    if (sBoard->scrollArrowTask == TASK_NONE)
    {
        gTempScrollArrowTemplate = sListScrollArrowsTemplate;
        gTempScrollArrowTemplate.fullyDownThreshold = sBoard->numMenuItems - sBoard->numToShowAtOnce;
        sBoard->scrollArrowTask = AddScrollIndicatorArrowPair(&gTempScrollArrowTemplate, &sBoard->listOffset);
    }
}

static void RemoveListScrollArrows(void)
{
    if (sBoard->scrollArrowTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sBoard->scrollArrowTask);
        sBoard->scrollArrowTask = TASK_NONE;
    }
}

// The bottom bar carries whatever the highlighted row has to say: the
// request's blurb, or a note when there is nothing to describe.
static void PrintDescription(const u8 *text)
{
    FillWindowPixelBuffer(WIN_MSG, PIXEL_FILL(1));
    AddTextPrinterParameterized(WIN_MSG, FONT_NARROW, text, 0, 1, 0, NULL);
    CopyWindowToVram(WIN_MSG, COPYWIN_GFX);
}

// Right panel: which list is on screen, then where the request starts, how
// far along it is, and what it pays.
static void PrintRequestInfo(s32 id)
{
    const struct CommunityRequest *request = GetRequest(id);
    s32 x;
    u32 i;
    u32 y;

    // The panel is 96px tall and every row is needed, so the rows stack flush:
    // FONT_SMALL is 12px, FONT_NARROW 16px, and 12+12+16+12+12+16+16 == 96.
    // Adding a gap anywhere pushes the last reward row out of the window.
    FillWindowPixelBuffer(WIN_INFO, PIXEL_FILL(1));
    x = GetStringCenterAlignXOffset(FONT_SMALL, GetTabName(), INFO_WIDTH);
    AddTextPrinterParameterized(WIN_INFO, FONT_SMALL, GetTabName(), x, 0, TEXT_SKIP_DRAW, NULL);

    if (id == LIST_CANCEL || request == NULL)
    {
        CopyWindowToVram(WIN_INFO, COPYWIN_GFX);
        // Only the Cancel row is left when a list is empty.
        PrintDescription(sBoard->numMenuItems == 1 ? sText_NothingPosted : sText_BoardHint);
        return;
    }

    if (CommunityRequests_GetStatus(id) == REQUEST_STATUS_LOCKED)
    {
        x = GetStringCenterAlignXOffset(FONT_NORMAL, sText_Unknown, INFO_WIDTH);
        AddTextPrinterParameterized(WIN_INFO, FONT_NORMAL, sText_Unknown, x, 40, TEXT_SKIP_DRAW, NULL);
        CopyWindowToVram(WIN_INFO, COPYWIN_GFX);
        PrintDescription(sText_NotPostedYet);
        return;
    }

    AddTextPrinterParameterized(WIN_INFO, FONT_SMALL, sText_ZoneLabel, 0, 12, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, FONT_NARROW, request->zone, 4, 24, TEXT_SKIP_DRAW, NULL);

    if (request->numSteps > 1)
    {
        // The stored step counts what is finished; the board reads out the
        // step being worked on, so a finished request stops at its last one.
        u32 step = min(CommunityRequests_GetStep(id) + 1, request->numSteps);

        ConvertIntToDecimalStringN(gStringVar1, step, STR_CONV_MODE_LEFT_ALIGN, 2);
        ConvertIntToDecimalStringN(gStringVar2, request->numSteps, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringExpandPlaceholders(gStringVar4, sText_StepFmt);
        AddTextPrinterParameterized(WIN_INFO, FONT_SMALL, gStringVar4, 0, 40, TEXT_SKIP_DRAW, NULL);
    }

    AddTextPrinterParameterized(WIN_INFO, FONT_SMALL, sText_RewardLabel, 0, 52, TEXT_SKIP_DRAW, NULL);
    ConvertIntToDecimalStringN(gStringVar1, request->money, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
    StringExpandPlaceholders(gStringVar4, sText_MoneyFmt);
    x = GetStringRightAlignXOffset(FONT_SMALL, gStringVar4, INFO_WIDTH);
    AddTextPrinterParameterized(WIN_INFO, FONT_SMALL, gStringVar4, x, 52, TEXT_SKIP_DRAW, NULL);

    y = 64;
    for (i = 0; i < REQUEST_NUM_REWARD_ITEMS; i++)
    {
        if (request->items[i].itemId == ITEM_NONE)
            continue;
        StringCopy(gStringVar1, GetItemName(request->items[i].itemId));
        if (request->items[i].quantity > 1)
        {
            ConvertIntToDecimalStringN(gStringVar2, request->items[i].quantity, STR_CONV_MODE_LEFT_ALIGN, 3);
            StringExpandPlaceholders(gStringVar4, sText_RewardItemFmt);
        }
        else
        {
            StringCopy(gStringVar4, gStringVar1);
        }
        AddTextPrinterParameterized(WIN_INFO, FONT_NARROW, gStringVar4, 4, y, TEXT_SKIP_DRAW, NULL);
        y += 16;
    }

    CopyWindowToVram(WIN_INFO, COPYWIN_GFX);
    PrintDescription(request->description);
}

static void RequestListCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
        PlaySE(SE_SELECT);
    PrintRequestInfo(itemIndex);
}

static void SwitchTab(void)
{
    PlaySE(SE_SELECT);
    RemoveListScrollArrows();
    DestroyListMenuTask(sBoard->listMenuTask, &sBoard->listOffset, &sBoard->listRow);
    sBoard->tab = (sBoard->tab + 1) % TAB_COUNT;
    sBoard->listOffset = 0;
    sBoard->listRow = 0;
    FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(1));
    BuildRequestList();
    sBoard->listMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
    AddListScrollArrows();
}

static void Task_Board_HandleInput(u8 taskId)
{
    s32 itemId;

    // Checked before the list menu runs: with LIST_NO_MULTIPLE_SCROLL it
    // ignores these buttons, so they are free for the tab swap.
    if (JOY_NEW(L_BUTTON | R_BUTTON | DPAD_LEFT | DPAD_RIGHT))
    {
        SwitchTab();
        return;
    }

    itemId = ListMenu_ProcessInput(sBoard->listMenuTask);
    ListMenuGetScrollAndRow(sBoard->listMenuTask, &sBoard->listOffset, &sBoard->listRow);

    // The board is a noticeboard: rows are read, not chosen. Only Cancel and
    // B (which the list menu reports as Cancel) leave.
    if (itemId == LIST_CANCEL)
    {
        PlaySE(SE_SELECT);
        RemoveListScrollArrows();
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_Board_Quit;
    }
}

static void Task_Board_Quit(u8 taskId)
{
    if (gPaletteFade.active)
        return;
    FreeBoardResources();
    DestroyTask(taskId);
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
