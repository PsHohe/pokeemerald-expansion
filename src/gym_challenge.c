#include "global.h"
#include "battle.h"
#include "battle_frontier.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "battle_transition.h"
#include "battle_util.h"
#include "community_requests.h"
#include "event_data.h"
#include "fpmath.h"
#include "frontier_util.h"
#include "data.h"
#include "gym_challenge.h"
#include "item.h"
#include "load_save.h"
#include "main.h"
#include "move.h"
#include "money.h"
#include "overworld.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "task.h"
#include "constants/abilities.h"
#include "constants/characters.h"
#include "constants/battle_frontier.h"
#include "constants/frontier_util.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/trainers.h"

// The gym challenge repurposes the Battle Frontier's facility trainer
// machinery: while a challenge is active, GymChallenge_OverrideFacilityPtrs
// points gFacilityTrainers/gFacilityTrainerMons at the tables below, so
// party generation, trainer names, classes, sprites and battle UI all read
// gym challengers instead of frontier trainers.

// Per-rank gym rules. Rank is 1-based (VAR_GYM_RANK); index with rank - 1.
// - offTypeSlots: how many brought mons may be off-type, provided they know
//   the gym's signature move (gSaveBlock3Ptr->gym.signatureMove).
// - doubles: from rank 4 on, gym battles are double battles.
// - barThreshold: Gym Points earned this rank to unlock the rank-up exam.
//   Tuned to ~20 average wins at the rank's level cap; adjust with playtests.
struct GymRankInfo
{
    u8 levelCap;
    u8 bringCount;
    u8 offTypeSlots;
    bool8 doubles;
    u16 barThreshold;
};

static const struct GymRankInfo sGymRanks[GYM_RANK_COUNT] =
{
    [0] = { .levelCap = 15, .bringCount = 2, .offTypeSlots = 0, .doubles = FALSE, .barThreshold = 4800 },
    [1] = { .levelCap = 22, .bringCount = 3, .offTypeSlots = 0, .doubles = FALSE, .barThreshold = 7000 },
    [2] = { .levelCap = 29, .bringCount = 3, .offTypeSlots = 1, .doubles = FALSE, .barThreshold = 9500 },
    [3] = { .levelCap = 36, .bringCount = 4, .offTypeSlots = 1, .doubles = TRUE,  .barThreshold = 11500 },
    [4] = { .levelCap = 43, .bringCount = 4, .offTypeSlots = 1, .doubles = TRUE,  .barThreshold = 14000 },
    [5] = { .levelCap = 50, .bringCount = 4, .offTypeSlots = 1, .doubles = TRUE,  .barThreshold = 16000 },
    [6] = { .levelCap = 58, .bringCount = 4, .offTypeSlots = 1, .doubles = TRUE,  .barThreshold = 18500 },
    [7] = { .levelCap = 65, .bringCount = 4, .offTypeSlots = 1, .doubles = TRUE,  .barThreshold = 21000 },
};

static const struct GymRankInfo *GetRankInfo(void)
{
    u32 rank = VarGet(VAR_GYM_RANK);

    if (rank < GYM_RANK_MIN)
        rank = GYM_RANK_MIN;
    else if (rank > GYM_RANK_MAX)
        rank = GYM_RANK_MAX;
    return &sGymRanks[rank - 1];
}

// Challenger dialogue. The frontier renders facility trainers' battle-end
// speech from 6-word easy chat arrays; gym challengers use plain text
// instead (see the gate in CopyFrontierTrainerText). introText is the
// overworld line on arrival ({STR_VAR_1} expands to the trainer's name),
// shown via GymChallenge_BufferChallengerIntro + msgbox gStringVar4 so the
// map script needs no per-challenger dialogue.
struct GymChallengerSpeech
{
    const u8 *introText;
    const u8 *beforeText;     // FRONTIER_BEFORE_TEXT
    const u8 *playerLostText; // FRONTIER_PLAYER_LOST_TEXT (challenger won)
    const u8 *playerWonText;  // FRONTIER_PLAYER_WON_TEXT (challenger lost)
};

#include "data/gym_challengers/rank1_common.h"
#include "data/gym_challengers/rank1_strong.h"
#include "data/gym_challengers/rank2_common.h"
#include "data/gym_challengers/rank2_strong.h"
#include "data/gym_challengers/rank3_common.h"
#include "data/gym_challengers/rank3_strong.h"
#include "data/gym_challengers/rank4_common.h"
#include "data/gym_challengers/rank4_strong.h"

// A challenger roster: the trainer table (frontier-compatible), the mon pool
// its monSets index into, and the matching in-battle speech.
struct GymChallengerSet
{
    const struct BattleFrontierTrainer *trainers;
    const struct GymChallengerSpeech *speech;
    const struct TrainerMon *monPool;
    u8 trainerCount;
};

#define GYM_TIER_COUNT 2

// Rosters by rank and tier. Ranks without content yet fall back to the
// rank 1 roster of the same tier (see GetChallengerSet).
static const struct GymChallengerSet sChallengerSets[GYM_RANK_COUNT][GYM_TIER_COUNT] =
{
    [0] =
    {
        [GYM_TIER_COMMON] =
        {
            .trainers = sRank1CommonChallengers,
            .speech = sRank1CommonSpeech,
            .monPool = sRank1CommonMons,
            .trainerCount = ARRAY_COUNT(sRank1CommonChallengers),
        },
        [GYM_TIER_STRONG] =
        {
            .trainers = sRank1StrongChallengers,
            .speech = sRank1StrongSpeech,
            .monPool = sRank1StrongMons,
            .trainerCount = ARRAY_COUNT(sRank1StrongChallengers),
        },
    },
    [1] =
    {
        [GYM_TIER_COMMON] =
        {
            .trainers = sRank2CommonChallengers,
            .speech = sRank2CommonSpeech,
            .monPool = sRank2CommonMons,
            .trainerCount = ARRAY_COUNT(sRank2CommonChallengers),
        },
        [GYM_TIER_STRONG] =
        {
            .trainers = sRank2StrongChallengers,
            .speech = sRank2StrongSpeech,
            .monPool = sRank2StrongMons,
            .trainerCount = ARRAY_COUNT(sRank2StrongChallengers),
        },
    },
    [2] =
    {
        [GYM_TIER_COMMON] =
        {
            .trainers = sRank3CommonChallengers,
            .speech = sRank3CommonSpeech,
            .monPool = sRank3CommonMons,
            .trainerCount = ARRAY_COUNT(sRank3CommonChallengers),
        },
        [GYM_TIER_STRONG] =
        {
            .trainers = sRank3StrongChallengers,
            .speech = sRank3StrongSpeech,
            .monPool = sRank3StrongMons,
            .trainerCount = ARRAY_COUNT(sRank3StrongChallengers),
        },
    },
    // Rank 4+ battles are doubles; these rosters (and their fallbacks for
    // ranks 5-8) avoid ally-hitting spread moves.
    [3] =
    {
        [GYM_TIER_COMMON] =
        {
            .trainers = sRank4CommonChallengers,
            .speech = sRank4CommonSpeech,
            .monPool = sRank4CommonMons,
            .trainerCount = ARRAY_COUNT(sRank4CommonChallengers),
        },
        [GYM_TIER_STRONG] =
        {
            .trainers = sRank4StrongChallengers,
            .speech = sRank4StrongSpeech,
            .monPool = sRank4StrongMons,
            .trainerCount = ARRAY_COUNT(sRank4StrongChallengers),
        },
    },
};

static EWRAM_DATA bool8 sChallengeActive = FALSE;
static EWRAM_DATA u8 sUsedChallengers[GYM_TIER_COUNT] = {0};
static EWRAM_DATA u8 sCurrentChallenger = 0;
static EWRAM_DATA u8 sCurrentTier = GYM_TIER_COMMON;

static void Task_StartGymBattleAfterTransition(u8 taskId);
static void HandleGymBattleEnd(void);

// The roster the current challenger belongs to: the current rank and tier.
// Ranks without authored content fall back to the nearest lower authored
// rank (levels still scale with the real rank's cap), so higher gyms always
// face the strongest available roster.
static const struct GymChallengerSet *GetChallengerSet(void)
{
    u32 rank = VarGet(VAR_GYM_RANK);

    if (rank < GYM_RANK_MIN)
        rank = GYM_RANK_MIN;
    else if (rank > GYM_RANK_MAX)
        rank = GYM_RANK_MAX;

    while (rank > GYM_RANK_MIN && sChallengerSets[rank - 1][sCurrentTier].trainers == NULL)
        rank--;
    return &sChallengerSets[rank - 1][sCurrentTier];
}

// Called from CopyFrontierTrainerText. While a gym challenge is active the
// challenger's in-battle speech comes from plain text instead of the
// frontier's easy chat words.
bool32 GymChallenge_CopyChallengerText(u8 whichText, u16 trainerId)
{
    const struct GymChallengerSet *set;

    if (!sChallengeActive)
        return FALSE;
    set = GetChallengerSet();
    if (trainerId >= set->trainerCount)
        return FALSE;

    switch (whichText)
    {
    case FRONTIER_BEFORE_TEXT:
        StringCopy(gStringVar4, set->speech[trainerId].beforeText);
        return TRUE;
    case FRONTIER_PLAYER_LOST_TEXT:
        StringCopy(gStringVar4, set->speech[trainerId].playerLostText);
        return TRUE;
    case FRONTIER_PLAYER_WON_TEXT:
        StringCopy(gStringVar4, set->speech[trainerId].playerWonText);
        return TRUE;
    }

    return FALSE;
}

// Called from SetFacilityPtrsGetLevel. While a gym challenge is active all
// facility trainer lookups use the current challenger's roster and the gym
// rank's level cap.
bool32 GymChallenge_OverrideFacilityPtrs(u8 *level)
{
    const struct GymChallengerSet *set;

    if (!sChallengeActive)
        return FALSE;

    set = GetChallengerSet();
    gFacilityTrainers = set->trainers;
    gFacilityTrainerMons = set->monPool;
    *level = GetRankInfo()->levelCap;
    return TRUE;
}

static u16 ValidateTeam(void)
{
    u32 i;
    u32 count = CalculatePlayerPartyCount();
    const struct GymRankInfo *rank = GetRankInfo();
    u32 gymType = GymChallenge_GetGymType();
    u16 sigMove = gSaveBlock3Ptr->gym.signatureMove;
    u32 offTypeUsed = 0;

    if (count < 1 || count > rank->bringCount)
        return GYM_TEAM_WRONG_SIZE;

    for (i = 0; i < count; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        u16 species = GetMonData(mon, MON_DATA_SPECIES);

        if (GetMonData(mon, MON_DATA_IS_EGG))
            return GYM_TEAM_WRONG_TYPE;
        if (GetMonData(mon, MON_DATA_LEVEL) > rank->levelCap)
            return GYM_TEAM_OVERLEVELED;
        if (gSpeciesInfo[species].types[0] != gymType
         && gSpeciesInfo[species].types[1] != gymType)
        {
            // Off-type mons take one of the rank's off-type slots and must
            // know the gym's signature move.
            if (++offTypeUsed > rank->offTypeSlots
             || sigMove == MOVE_NONE
             || !MonKnowsMove(mon, sigMove))
                return GYM_TEAM_WRONG_TYPE;
        }
    }

    return GYM_TEAM_OK;
}

// The chance (in percent) that the next challenger is a strong one.
static u32 GetStrongChallengerChance(void)
{
    u32 chance = GYM_STRONG_CHANCE_BASE
               + GYM_STRONG_CHANCE_PER_WIN * gSaveBlock3Ptr->gym.winStreak;

    if (chance > GYM_STRONG_CHANCE_MAX)
        chance = GYM_STRONG_CHANCE_MAX;
    return chance;
}

// Validates the player's team against the gym rules. On success the gym
// opens for challenges. VAR_RESULT receives a GymTeamValidation value.
void GymChallenge_Begin(void)
{
    gSpecialVar_Result = ValidateTeam();
    if (gSpecialVar_Result == GYM_TEAM_OK)
    {
        sChallengeActive = TRUE;
        sUsedChallengers[GYM_TIER_COMMON] = 0;
        sUsedChallengers[GYM_TIER_STRONG] = 0;
        sCurrentTier = GYM_TIER_COMMON;
    }
}

// Rolls the challenger's tier against the streak-based strong chance
// (gSpecialVar_0x8006 receives the GYM_TIER_* result), then picks a random
// not-yet-fought challenger from that tier's roster, prepares its overworld
// sprite (VAR_OBJ_GFX_ID_0) and copies its name to gStringVar1.
// VAR_RESULT receives the challenger index for dialogue selection.
void GymChallenge_PickChallenger(void)
{
    u32 id;
    const struct GymChallengerSet *set;

    sCurrentTier = (Random() % 100 < GetStrongChallengerChance())
                 ? GYM_TIER_STRONG : GYM_TIER_COMMON;
    gSpecialVar_0x8006 = sCurrentTier;

    // Point the facility tables at this tier's roster right away so the
    // overworld sprite lookup below reads the right trainer.
    set = GetChallengerSet();
    gFacilityTrainers = set->trainers;
    gFacilityTrainerMons = set->monPool;

    // If the whole roster was already fought this wave, allow repeats.
    if (sUsedChallengers[sCurrentTier] == (1u << set->trainerCount) - 1)
        sUsedChallengers[sCurrentTier] = 0;

    do
    {
        id = Random() % set->trainerCount;
    } while (sUsedChallengers[sCurrentTier] & (1u << id));

    sUsedChallengers[sCurrentTier] |= 1u << id;
    sCurrentChallenger = id;
    SetBattleFacilityTrainerGfxId(id, 0);
    StringCopy(gStringVar1, set->trainers[id].trainerName);
    gSpecialVar_Result = id;
}

// Expands the current challenger's overworld intro line into gStringVar4
// (the name from PickChallenger is still in gStringVar1 for {STR_VAR_1}).
// The script displays it with "msgbox gStringVar4".
void GymChallenge_BufferChallengerIntro(void)
{
    StringExpandPlaceholders(gStringVar4, GetChallengerSet()->speech[sCurrentChallenger].introText);
}

// Fixed IVs for counter-picked strong parties (the frontier's lowest band
// is 3; rank-1 commons get that via the low trainer id).
#define GYM_STRONG_FIXED_IV 12

// Converts a type-chart multiplier into counter-pick score points, loosely
// mirroring the Battle Dome's pre-battle scoring (battle_dome.c,
// GetTypeEffectivenessPoints).
static s32 MatchupToPoints(uq4_12_t eff)
{
    if (eff == UQ_4_12(0.0))
        return -8;
    if (eff <= UQ_4_12(0.25))
        return -6;
    if (eff <= UQ_4_12(0.5))
        return -3;
    if (eff <= UQ_4_12(1.0))
        return 0;
    if (eff <= UQ_4_12(2.0))
        return 4;
    return 8;
}

// Scores a strong challenger's pool entry against the gym's type: the best
// damaging move it has into the gym type, minus how hard the gym's STAB
// hits it back.
static s32 ScoreStrongCandidate(const struct TrainerMon *mon, u32 gymType)
{
    u32 i;
    s32 offense = -16;
    uq4_12_t incoming;
    u32 type1 = gSpeciesInfo[mon->species].types[0];
    u32 type2 = gSpeciesInfo[mon->species].types[1];

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        s32 points;

        if (mon->moves[i] == MOVE_NONE || GetMovePower(mon->moves[i]) == 0)
            continue;
        points = MatchupToPoints(GetTypeModifier(GetMoveType(mon->moves[i]), gymType));
        if (points > offense)
            offense = points;
    }

    incoming = GetTypeModifier(gymType, type1);
    if (type2 != type1)
        incoming = uq4_12_multiply(incoming, GetTypeModifier(gymType, type2));

    return offense - MatchupToPoints(incoming);
}

// Builds a strong challenger's party by counter-picking against the gym's
// type: every pool entry is scored, and the party is drawn from the top of
// the ranking with a small skip chance so the pick isn't fully
// deterministic. Species in an authored pool are unique, so no duplicate
// rejection is needed.
static void FillStrongChallengerParty(u32 monCount)
{
    u32 i, j, count, chosen;
    u32 gymType = GymChallenge_GetGymType();
    const struct GymChallengerSet *set = GetChallengerSet();
    const u16 *monSet = set->trainers[sCurrentChallenger].monSet;
    u32 otId = Random32();
    u16 ids[16];
    s32 scores[16];

    for (count = 0; count < ARRAY_COUNT(ids) && monSet[count] != 0xFFFF; count++)
    {
        ids[count] = monSet[count];
        scores[count] = ScoreStrongCandidate(&set->monPool[monSet[count]], gymType);
    }

    // Insertion sort, best score first.
    for (i = 1; i < count; i++)
    {
        u16 id = ids[i];
        s32 score = scores[i];

        for (j = i; j > 0 && scores[j - 1] < score; j--)
        {
            ids[j] = ids[j - 1];
            scores[j] = scores[j - 1];
        }
        ids[j] = id;
        scores[j] = score;
    }

    if (monCount > count)
        monCount = count;

    ZeroEnemyPartyMons();
    for (chosen = 0, i = 0; chosen < monCount; i++)
    {
        // Skip a candidate now and then, unless every remaining one is needed.
        if (count - i > monCount - chosen && (Random() % 100) < 25)
            continue;
        CreateFacilityMon(&set->monPool[ids[i]], GetRankInfo()->levelCap,
                          GYM_STRONG_FIXED_IV, otId, 0,
                          &gParties[B_TRAINER_OPPONENT_A][chosen]);
        chosen++;
    }
}

// Starts the battle against the current challenger. Must be followed by
// waitstate. Uses the Battle Tower battle type so the pre-generated party
// is kept, no EXP/money is handed out and losing returns to the script
// instead of whiting out. From rank 4 on, gym battles are doubles (the same
// flag the frontier's doubles mode uses).
void GymChallenge_StartBattle(void)
{
    InitTrainerBattleParameter();
    TRAINER_BATTLE_PARAM.opponentA = sCurrentChallenger;
    gBattleScripting.specialTrainerBattleType = FACILITY_BATTLE_TOWER;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_BATTLE_TOWER;
    if (GetRankInfo()->doubles)
        gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
    if (sCurrentTier == GYM_TIER_STRONG)
        FillStrongChallengerParty(GetRankInfo()->bringCount);
    else
        FillFrontierTrainerParty(GetRankInfo()->bringCount);
    CreateTask(Task_StartGymBattleAfterTransition, 1);
    PlayMapChosenOrBattleBGM(0);
    BattleTransition_StartOnField(GetSpecialBattleTransition(B_TRANSITION_GROUP_B_TOWER));
}

void GymChallenge_End(void)
{
    sChallengeActive = FALSE;
}

// Gym Points wallet and rank-up bar. The wallet reuses the money helpers, so
// it shares money's cap and save encryption; the bar total is plain and only
// ever compared against the rank's threshold.

u32 GymChallenge_GetPoints(void)
{
    return GetMoney(&gSaveBlock3Ptr->gym.points);
}

u32 GymChallenge_GetBarProgress(void)
{
    return gSaveBlock3Ptr->gym.pointsEarnedThisRank;
}

u32 GymChallenge_GetBarThreshold(void)
{
    return GetRankInfo()->barThreshold;
}

u32 GymChallenge_GetRank(void)
{
    return VarGet(VAR_GYM_RANK);
}

u32 GymChallenge_GetGymType(void)
{
    return VarGet(VAR_CHOSEN_GYM_TYPE);
}

u32 GymChallenge_GetLevelCap(void)
{
    return GetRankInfo()->levelCap;
}

u32 GymChallenge_GetBringCount(void)
{
    return GetRankInfo()->bringCount;
}

u32 GymChallenge_GetOffTypeSlots(void)
{
    return GetRankInfo()->offTypeSlots;
}

// Script special: VAR_RESULT receives whether gym battles are doubles at
// the current rank.
void GymChallenge_IsDoubles(void)
{
    gSpecialVar_Result = GetRankInfo()->doubles;
}

// Advances to the next rank (called by the rank-up exam script). Resets the
// rank-up bar and the win streak; the points wallet is untouched.
void GymChallenge_RankUp(void)
{
    u32 rank = VarGet(VAR_GYM_RANK);

    if (rank < GYM_RANK_MAX)
        VarSet(VAR_GYM_RANK, rank + 1);
    gSaveBlock3Ptr->gym.pointsEarnedThisRank = 0;
    gSaveBlock3Ptr->gym.winStreak = 0;
    // Word gets around: the city posts the requests this rank unlocks. Read
    // the rank back, since the VarSet above is skipped at max rank.
    CommunityRequests_UnlockForRank(VarGet(VAR_GYM_RANK));
}

// Mirrors GetTrainerMoneyToGive's formula (4 * lastMonLevel * classRate).
// Generated parties are all at the rank's level cap, so the cap stands in
// for the last mon's level.
static u32 CalcChallengerPoints(u32 challengerId)
{
    u32 facilityClass = GetChallengerSet()->trainers[challengerId].facilityClass;
    u32 classRate = gTrainerClasses[gFacilityClassToTrainerClass[facilityClass]].money;

    if (classRate == 0)
        classRate = 5;
    return 4 * GetRankInfo()->levelCap * classRate;
}

// Awards Gym Points for defeating the current challenger (strong tier pays
// a multiplier) and extends the win streak. Copies the amount to gStringVar1
// for the reward message. VAR_RESULT receives TRUE if this win just filled
// the rank-up bar (for a one-time announcement).
void GymChallenge_AwardVictoryPoints(void)
{
    u32 points = CalcChallengerPoints(sCurrentChallenger);
    u32 threshold = GymChallenge_GetBarThreshold();
    bool32 wasFull = gSaveBlock3Ptr->gym.pointsEarnedThisRank >= threshold;

    if (sCurrentTier == GYM_TIER_STRONG)
        points *= GYM_STRONG_POINTS_MULTIPLIER;

    AddMoney(&gSaveBlock3Ptr->gym.points, points);
    gSaveBlock3Ptr->gym.pointsEarnedThisRank += points;
    if (gSaveBlock3Ptr->gym.winStreak < 0xFFFF)
        gSaveBlock3Ptr->gym.winStreak++;
    ConvertIntToDecimalStringN(gStringVar1, points, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
    gSpecialVar_Result = !wasFull && gSaveBlock3Ptr->gym.pointsEarnedThisRank >= threshold;
}

// Called when the leader loses to a challenger: the win streak resets, and
// with it the strong-challenger chance.
void GymChallenge_RecordLoss(void)
{
    gSaveBlock3Ptr->gym.winStreak = 0;
}

// Buckets the strong-challenger chance into a flavor tier for the
// attendant's opening phrase. VAR_RESULT receives 0-3 (calm day -> the
// strong ones are lining up).
void GymChallenge_GetStreakTier(void)
{
    u32 chance = GetStrongChallengerChance();

    if (chance <= GYM_STRONG_CHANCE_BASE)
        gSpecialVar_Result = 0;
    else if (chance <= 40)
        gSpecialVar_Result = 1;
    else if (chance <= 70)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 3;
}

// Buffers the current win streak to gStringVar1 (for the rank examiner).
void GymChallenge_BufferStreak(void)
{
    ConvertIntToDecimalStringN(gStringVar1, gSaveBlock3Ptr->gym.winStreak, STR_CONV_MODE_LEFT_ALIGN, 5);
}

// VAR_RESULT receives whether the rank-up exam is unlocked. A full bar only
// unlocks the exam; ranking up is story-driven and resets the bar, not the
// wallet.
void GymChallenge_IsRankUpUnlocked(void)
{
    gSpecialVar_Result = gSaveBlock3Ptr->gym.pointsEarnedThisRank >= GymChallenge_GetBarThreshold();
}

// Gym starter gifts: three low-level Pokemon per gym type, handed out by the
// intro (currently the TestingGrounds gift NPC) in place of a single starter.
#define GYM_STARTER_COUNT 3

static const u16 sGymStarterSpecies[NUMBER_OF_MON_TYPES][GYM_STARTER_COUNT] =
{
    [TYPE_NORMAL]   = {SPECIES_ZIGZAGOON, SPECIES_WHISMUR,  SPECIES_SKITTY},
    [TYPE_FIGHTING] = {SPECIES_MACHOP,    SPECIES_MAKUHITA, SPECIES_MEDITITE},
    [TYPE_FLYING]   = {SPECIES_TAILLOW,   SPECIES_DODUO,    SPECIES_SWABLU},
    [TYPE_POISON]   = {SPECIES_GRIMER,    SPECIES_GULPIN,   SPECIES_ZUBAT},
    [TYPE_GROUND]   = {SPECIES_SANDSHREW, SPECIES_PHANPY,   SPECIES_TRAPINCH},
    [TYPE_ROCK]     = {SPECIES_GEODUDE,   SPECIES_NOSEPASS, SPECIES_LILEEP},
    [TYPE_BUG]      = {SPECIES_NINCADA,   SPECIES_SURSKIT,  SPECIES_VOLBEAT},
    [TYPE_GHOST]    = {SPECIES_SHUPPET,   SPECIES_DUSKULL,  SPECIES_SABLEYE},
    [TYPE_STEEL]    = {SPECIES_ARON,      SPECIES_MAWILE,   SPECIES_MAGNEMITE},
    [TYPE_FIRE]     = {SPECIES_TORCHIC,   SPECIES_NUMEL,    SPECIES_SLUGMA},
    [TYPE_WATER]    = {SPECIES_MUDKIP,    SPECIES_CORPHISH, SPECIES_WAILMER},
    [TYPE_GRASS]    = {SPECIES_TREECKO,   SPECIES_SEEDOT,   SPECIES_SHROOMISH},
    [TYPE_ELECTRIC] = {SPECIES_PIKACHU,   SPECIES_ELECTRIKE, SPECIES_PLUSLE},
    [TYPE_PSYCHIC]  = {SPECIES_RALTS,     SPECIES_ABRA,     SPECIES_SPOINK},
    [TYPE_ICE]      = {SPECIES_SPHEAL,    SPECIES_SNORUNT,  SPECIES_SWINUB},
    [TYPE_DRAGON]   = {SPECIES_BAGON,     SPECIES_DRATINI,  SPECIES_AXEW},
    [TYPE_DARK]     = {SPECIES_POOCHYENA, SPECIES_ABSOL,    SPECIES_CARVANHA},
    [TYPE_FAIRY]    = {SPECIES_MARILL,    SPECIES_SNUBBULL, SPECIES_JIGGLYPUFF},
};

static u32 GetSafeGymType(void)
{
    u32 type = GymChallenge_GetGymType();

    if (type >= NUMBER_OF_MON_TYPES || sGymStarterSpecies[type][0] == SPECIES_NONE)
        type = TYPE_NORMAL;
    return type;
}

// Gives the gym-type starter trio at the current rank's level cap and copies
// the gym type's name to gStringVar1 for the reward message. VAR_RESULT
// receives FALSE if the party lacks room for all three.
void GymChallenge_GiveGymStarters(void)
{
    u32 i;
    u32 type = GetSafeGymType();
    u32 level = GetRankInfo()->levelCap;

    if (CalculatePlayerPartyCount() + GYM_STARTER_COUNT > PARTY_SIZE)
    {
        gSpecialVar_Result = FALSE;
        return;
    }

    for (i = 0; i < GYM_STARTER_COUNT; i++)
        ScriptGiveMon(sGymStarterSpecies[type][i], level, ITEM_NONE);
    StringCopy(gStringVar1, gTypesInfo[type].name);
    gSpecialVar_Result = TRUE;
}

// Tier-1 signature TM move per gym type: a good but not overpowering move
// the gym is known for. Assigned once at the start; replaced by a stronger
// choice at rank 5 (not yet implemented). Every entry must exist as a TM
// (FOREACH_TM in include/constants/tms_hms.h) so the move can be taught to
// off-type recruits.
static const u16 sGymSignatureMoves[NUMBER_OF_MON_TYPES] =
{
    [TYPE_NORMAL]   = MOVE_FACADE,
    [TYPE_FIGHTING] = MOVE_BRICK_BREAK,
    [TYPE_FLYING]   = MOVE_AERIAL_ACE,
    [TYPE_POISON]   = MOVE_SLUDGE_BOMB,
    [TYPE_GROUND]   = MOVE_DIG,
    [TYPE_ROCK]     = MOVE_ROCK_TOMB,
    [TYPE_BUG]      = MOVE_X_SCISSOR,
    [TYPE_GHOST]    = MOVE_SHADOW_CLAW,
    [TYPE_STEEL]    = MOVE_STEEL_WING,
    [TYPE_FIRE]     = MOVE_FIRE_PUNCH,
    [TYPE_WATER]    = MOVE_WATER_PULSE,
    [TYPE_GRASS]    = MOVE_GIGA_DRAIN,
    [TYPE_ELECTRIC] = MOVE_SHOCK_WAVE,
    [TYPE_PSYCHIC]  = MOVE_PSYSHOCK,
    [TYPE_ICE]      = MOVE_AVALANCHE,
    [TYPE_DRAGON]   = MOVE_DRAGON_CLAW,
    [TYPE_DARK]     = MOVE_THIEF,
    [TYPE_FAIRY]    = MOVE_DAZZLING_GLEAM,
};

// Assigns the gym's tier-1 signature move (from the gym type). Storing it is
// what unlocks the TM in the TM Machine — no bag item involved. Idempotent:
// if a signature move is already assigned it only re-buffers the names.
// Buffers the TM item name to gStringVar2 and the move name to gStringVar3
// (gStringVar1 is left alone — the starter gift keeps the type name there).
// VAR_RESULT always receives TRUE.
void GymChallenge_AssignSignatureTM(void)
{
    u16 move = gSaveBlock3Ptr->gym.signatureMove;

    if (move == MOVE_NONE)
    {
        move = sGymSignatureMoves[GetSafeGymType()];
        gSaveBlock3Ptr->gym.signatureMove = move;
    }
    CopyItemName(GetTMHMItemIdFromMoveId(move), gStringVar2);
    StringCopy(gStringVar3, GetMoveName(move));
    gSpecialVar_Result = TRUE;
}

// Tier-2 signature TM choices per gym type, offered once at rank 5. Only
// the chosen move counts afterwards — off-type mons taught the tier-1 move
// stop qualifying (the upgrade dialogue warns about this).
static const u16 sGymSignatureUpgrades[NUMBER_OF_MON_TYPES][2] =
{
    [TYPE_NORMAL]   = {MOVE_RETURN, MOVE_HYPER_BEAM},
    [TYPE_FIGHTING] = {MOVE_CLOSE_COMBAT, MOVE_FOCUS_PUNCH},
    [TYPE_FLYING]   = {MOVE_AIR_SLASH, MOVE_BRAVE_BIRD},
    [TYPE_POISON]   = {MOVE_GUNK_SHOT, MOVE_TOXIC},
    [TYPE_GROUND]   = {MOVE_EARTHQUAKE, MOVE_EARTH_POWER},
    [TYPE_ROCK]     = {MOVE_STONE_EDGE, MOVE_ROCK_SLIDE},
    [TYPE_BUG]      = {MOVE_BUG_BUZZ, MOVE_U_TURN},
    [TYPE_GHOST]    = {MOVE_SHADOW_BALL, MOVE_PHANTOM_FORCE},
    [TYPE_STEEL]    = {MOVE_IRON_TAIL, MOVE_FLASH_CANNON},
    [TYPE_FIRE]     = {MOVE_FLAMETHROWER, MOVE_FIRE_BLAST},
    [TYPE_WATER]    = {MOVE_SCALD, MOVE_HYDRO_PUMP},
    [TYPE_GRASS]    = {MOVE_ENERGY_BALL, MOVE_SOLAR_BEAM},
    [TYPE_ELECTRIC] = {MOVE_THUNDERBOLT, MOVE_THUNDER},
    [TYPE_PSYCHIC]  = {MOVE_PSYCHIC, MOVE_ZEN_HEADBUTT},
    [TYPE_ICE]      = {MOVE_ICE_BEAM, MOVE_BLIZZARD},
    [TYPE_DRAGON]   = {MOVE_OUTRAGE, MOVE_DRAGON_PULSE},
    [TYPE_DARK]     = {MOVE_DARK_PULSE, MOVE_CRUNCH},
    [TYPE_FAIRY]    = {MOVE_PLAY_ROUGH, MOVE_MOONBLAST},
};

#define GYM_TM_UPGRADE_RANK 5

// VAR_RESULT receives whether the rank-5 signature TM upgrade is available:
// rank high enough, a tier-1 move assigned, and not yet upgraded.
void GymChallenge_CanUpgradeSignatureTM(void)
{
    u16 move = gSaveBlock3Ptr->gym.signatureMove;
    const u16 *choices = sGymSignatureUpgrades[GetSafeGymType()];

    gSpecialVar_Result = GymChallenge_GetRank() >= GYM_TM_UPGRADE_RANK
                      && move != MOVE_NONE
                      && move != choices[0]
                      && move != choices[1];
}

// Buffers the two upgrade choices' move names (gStringVar1, gStringVar2)
// for the selection menu.
void GymChallenge_BufferSignatureUpgradeChoices(void)
{
    const u16 *choices = sGymSignatureUpgrades[GetSafeGymType()];

    StringCopy(gStringVar1, GetMoveName(choices[0]));
    StringCopy(gStringVar2, GetMoveName(choices[1]));
}

// Replaces the signature move with upgrade choice gSpecialVar_0x8004 (0/1).
// Storing it unlocks BOTH upgrade options in the TM Machine (see
// GymChallenge_IsMoveUnlockedBySignature); only the chosen one is free.
// Buffers the TM item name to gStringVar2 and the move name to gStringVar3.
// VAR_RESULT always receives TRUE.
void GymChallenge_UpgradeSignatureTM(void)
{
    u32 choice = (gSpecialVar_0x8004 != 0);
    u16 move = sGymSignatureUpgrades[GetSafeGymType()][choice];

    gSaveBlock3Ptr->gym.signatureMove = move;
    CopyItemName(GetTMHMItemIdFromMoveId(move), gStringVar2);
    StringCopy(gStringVar3, GetMoveName(move));
    gSpecialVar_Result = TRUE;
}

// Buffers the signature move's name to gStringVar1. VAR_RESULT receives
// whether a signature move has been assigned yet.
void GymChallenge_BufferSignatureMove(void)
{
    u16 move = gSaveBlock3Ptr->gym.signatureMove;

    if (move == MOVE_NONE)
    {
        StringCopy(gStringVar1, COMPOUND_STRING("???"));
        gSpecialVar_Result = FALSE;
    }
    else
    {
        StringCopy(gStringVar1, GetMoveName(move));
        gSpecialVar_Result = TRUE;
    }
}

u32 GymChallenge_GetSignatureMove(void)
{
    return gSaveBlock3Ptr->gym.signatureMove;
}

// Whether the signature system unlocks this move in the TM Machine: the
// tier-1 move once a signature is assigned, and both tier-2 choices once the
// player has upgraded (the chosen one is the free signature, the other is
// purchasable at its normal price).
bool32 GymChallenge_IsMoveUnlockedBySignature(u16 move)
{
    u16 sig = gSaveBlock3Ptr->gym.signatureMove;
    u32 type = GetSafeGymType();
    const u16 *upgrades = sGymSignatureUpgrades[type];

    if (sig == MOVE_NONE)
        return FALSE;
    if (move == sGymSignatureMoves[type])
        return TRUE;
    if (sig == upgrades[0] || sig == upgrades[1])
        return move == upgrades[0] || move == upgrades[1];
    return FALSE;
}

// Deducts Gym Points from the wallet (e.g. for a TM Machine purchase).
// Returns FALSE and deducts nothing if the wallet can't cover it.
bool32 GymChallenge_TrySpendPoints(u32 amount)
{
    if (!IsEnoughMoney(&gSaveBlock3Ptr->gym.points, amount))
        return FALSE;
    RemoveMoney(&gSaveBlock3Ptr->gym.points, amount);
    return TRUE;
}

// Buffers rank (gStringVar1), level cap (gStringVar2) and bring count
// (gStringVar3) for attendant/examiner dialogue.
void GymChallenge_BufferRankInfo(void)
{
    const struct GymRankInfo *rank = GetRankInfo();

    ConvertIntToDecimalStringN(gStringVar1, GymChallenge_GetRank(), STR_CONV_MODE_LEFT_ALIGN, 1);
    ConvertIntToDecimalStringN(gStringVar2, rank->levelCap, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, rank->bringCount, STR_CONV_MODE_LEFT_ALIGN, 1);
}

// Buffers the gym type's name (gStringVar1), rank-up bar progress
// (gStringVar2) and bar threshold (gStringVar3).
void GymChallenge_BufferGymProgress(void)
{
    StringCopy(gStringVar1, gTypesInfo[GetSafeGymType()].name);
    ConvertIntToDecimalStringN(gStringVar2, GymChallenge_GetBarProgress(), STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
    ConvertIntToDecimalStringN(gStringVar3, GymChallenge_GetBarThreshold(), STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS);
}

// Saved gym teams. A team stores identity references (personality + OT id),
// not copies: the mon is looked up in the party and PC boxes when needed, so
// references survive box reorganization and only go stale on release/trade.

// Finds the referenced mon in srcParty (the live party, or the save-block
// backup while the live party is swapped out) or in the PC boxes, and copies
// it into dest as a party mon. Returns FALSE if the identity no longer
// exists anywhere (released or traded away).
static bool32 FetchMonByIdentity(const struct GymTeamSlot *slot, struct Pokemon *srcParty, struct Pokemon *dest)
{
    u32 i, box;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &srcParty[i];

        if (GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE
         && GetMonData(mon, MON_DATA_PERSONALITY) == slot->personality
         && GetMonData(mon, MON_DATA_OT_ID) == slot->otId)
        {
            *dest = *mon;
            return TRUE;
        }
    }

    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (i = 0; i < IN_BOX_COUNT; i++)
        {
            if (GetBoxMonDataAt(box, i, MON_DATA_SPECIES) != SPECIES_NONE
             && GetBoxMonDataAt(box, i, MON_DATA_PERSONALITY) == slot->personality
             && GetBoxMonDataAt(box, i, MON_DATA_OT_ID) == slot->otId)
            {
                BoxMonToMon(GetBoxedMonPtr(box, i), dest);
                return TRUE;
            }
        }
    }

    return FALSE;
}

// Copies the referenced mon's nickname into dest (POKEMON_NAME_LENGTH + 1
// bytes). Returns FALSE if no mon with this identity exists anymore.
static bool32 BufferMonNicknameByIdentity(const struct GymTeamSlot *slot, u8 *dest)
{
    struct Pokemon mon;

    if (!FetchMonByIdentity(slot, gParties[B_TRAINER_PLAYER], &mon))
        return FALSE;
    GetMonData(&mon, MON_DATA_NICKNAME, dest);
    StringGet_Nickname(dest);
    return TRUE;
}

// Registers the current party as saved team gSpecialVar_0x8004.
// VAR_RESULT receives the number of members stored.
void GymTeams_RegisterParty(void)
{
    u32 i;
    u32 teamId = gSpecialVar_0x8004;
    struct GymSavedTeam *team;
    u32 count = 0;

    if (teamId >= GYM_NUM_SAVED_TEAMS)
    {
        gSpecialVar_Result = 0;
        return;
    }

    team = &gSaveBlock3Ptr->gym.teams[teamId];
    memset(team, 0, sizeof(*team));
    for (i = 0; i < PARTY_SIZE && count < GYM_TEAM_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];

        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        team->slots[count].personality = GetMonData(mon, MON_DATA_PERSONALITY);
        team->slots[count].otId = GetMonData(mon, MON_DATA_OT_ID);
        count++;
    }
    team->numMons = count;
    gSpecialVar_Result = count;
}

// Builds the member list of saved team gSpecialVar_0x8004 into gStringVar1,
// two names per message line ("???" for stale references). VAR_RESULT
// receives the member count (0 = empty team).
void GymTeams_BufferTeam(void)
{
    u32 i;
    u32 teamId = gSpecialVar_0x8004;
    const struct GymSavedTeam *team;
    u8 *end = gStringVar1;

    *end = EOS;
    if (teamId >= GYM_NUM_SAVED_TEAMS)
    {
        gSpecialVar_Result = 0;
        return;
    }

    team = &gSaveBlock3Ptr->gym.teams[teamId];
    for (i = 0; i < team->numMons && i < GYM_TEAM_SIZE; i++)
    {
        u8 nickname[POKEMON_NAME_LENGTH + 1];

        if (!BufferMonNicknameByIdentity(&team->slots[i], nickname))
            StringCopy(nickname, COMPOUND_STRING("???"));
        if (i != 0)
        {
            if (i % 2 == 1)
            {
                end = StringAppend(end, COMPOUND_STRING(", "));
            }
            else
            {
                *end++ = (i == 2) ? CHAR_NEWLINE : CHAR_PROMPT_SCROLL;
                *end = EOS;
            }
        }
        end = StringAppend(end, nickname);
    }
    gSpecialVar_Result = team->numMons;
}

// Clears saved team gSpecialVar_0x8004.
void GymTeams_ClearTeam(void)
{
    u32 teamId = gSpecialVar_0x8004;

    if (teamId < GYM_NUM_SAVED_TEAMS)
        memset(&gSaveBlock3Ptr->gym.teams[teamId], 0, sizeof(struct GymSavedTeam));
}

// Swaps saved team gSpecialVar_0x8004 into the party for gym duty. The
// roaming party is backed up with SavePlayerParty first (the script restores
// it with LoadPlayerParty when the gym closes or the opening is cancelled).
// VAR_RESULT receives the number of members found; 0 means nothing was
// swapped and the party is untouched.
void GymTeams_LoadTeamIntoParty(void)
{
    u32 i;
    u32 teamId = gSpecialVar_0x8004;
    const struct GymSavedTeam *team;
    u32 count = 0;

    gSpecialVar_Result = 0;
    if (teamId >= GYM_NUM_SAVED_TEAMS)
        return;
    team = &gSaveBlock3Ptr->gym.teams[teamId];
    if (team->numMons == 0)
        return;

    SavePlayerParty();
    ZeroPlayerPartyMons();
    for (i = 0; i < team->numMons && i < GYM_TEAM_SIZE; i++)
    {
        // The live party was just zeroed; resolve against the backup.
        if (FetchMonByIdentity(&team->slots[i], gSaveBlock1Ptr->playerParty, &gParties[B_TRAINER_PLAYER][count]))
            count++;
    }

    if (count == 0)
    {
        LoadPlayerParty();
        return;
    }
    CalculatePlayerPartyCount();
    gSpecialVar_Result = count;
}

// Opens the choose-your-defenders party menu with the gym's rules: exactly
// the rank's bring count, each mon gym-legal (see the FACILITY_GYM cases in
// party_menu.c). Must be preceded by SavePlayerParty (or
// GymTeams_LoadTeamIntoParty) and fadescreen. VAR_RESULT receives TRUE if a
// selection was confirmed, FALSE if the player backed out.
static void CB2_ReturnFromChooseGymParty(void)
{
    gSpecialVar_Result = (gSelectedOrderFromParty[0] != 0);
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

void GymChallenge_ChooseGymParty(void)
{
    gMain.savedCallback = CB2_ReturnFromChooseGymParty;
    VarSet(VAR_FRONTIER_FACILITY, FACILITY_GYM);
    gSpecialVar_0x8005 = GetRankInfo()->bringCount;
    InitChooseHalfPartyForBattle(0);
}

static void Task_StartGymBattleAfterTransition(u8 taskId)
{
    if (IsBattleTransitionDone() == TRUE)
    {
        gMain.savedCallback = HandleGymBattleEnd;
        SetMainCallback2(CB2_InitBattle);
        DestroyTask(taskId);
    }
}

static void HandleGymBattleEnd(void)
{
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
