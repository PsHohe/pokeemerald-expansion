// Community Requests: what the city posts on the gym's mailboard. The table
// is sparse — a slot with no name is an unwritten request, and both the board
// UI and the unlock specials skip it, so ids can be reserved before their
// content exists.
//
// Ids are save slots (see gSaveBlock3Ptr->requests): only ever append.
//
// unlockRank posts the request when the gym reaches that rank;
// REQUEST_UNLOCK_SCRIPT leaves it to a map script
// (CommunityRequests_UnlockRequest).
//
// numSteps drives the "STEP x/y" readout only. 0 or 1 means the board shows
// no step counter; scripts still advance the step with
// CommunityRequests_AdvanceStep if they want the progress remembered.
//
// Names are squeezed into a 96px list row, descriptions into two ~34
// character lines — keep both short and break descriptions by hand.

struct CommunityRequestReward
{
    u16 itemId;
    u16 quantity;
};

struct CommunityRequest
{
    const u8 *name;
    const u8 *description; // Two lines, split with an explicit \n.
    const u8 *zone;        // Where in the city the request starts.
    u32 money;
    struct CommunityRequestReward items[REQUEST_NUM_REWARD_ITEMS];
    u8 unlockRank;
    u8 numSteps;
};

static const struct CommunityRequest sCommunityRequests[NUM_COMMUNITY_REQUESTS] =
{
    [REQUEST_GREET_THE_LEADER] =
    {
        .name = COMPOUND_STRING("Meet the Leader"),
        .description = COMPOUND_STRING("A visitor would like to meet the\ncity's new Gym Leader in person."),
        .zone = COMPOUND_STRING("Testing Grounds"),
        .unlockRank = 1,
        .money = 500,
    },
    [REQUEST_POTION_FOR_THE_OLD_MAN] =
    {
        .name = COMPOUND_STRING("A Potion, Please"),
        .description = COMPOUND_STRING("An old man's POKéMON is hurt and\nhe can't make it to the MART."),
        .zone = COMPOUND_STRING("Testing Grounds"),
        .unlockRank = REQUEST_UNLOCK_SCRIPT,
        .money = 800,
        .items =
        {
            { ITEM_SUPER_POTION, 2 },
        },
    },
    [REQUEST_ROWDY_TRAINER] =
    {
        .name = COMPOUND_STRING("The Rowdy Trainer"),
        .description = COMPOUND_STRING("A trainer is picking fights by the\ngate. Hear them out, then settle it."),
        .zone = COMPOUND_STRING("Testing Grounds"),
        .unlockRank = 1,
        .numSteps = 3,
        .money = 1200,
        .items =
        {
            { ITEM_GREAT_BALL, 3 },
        },
    },
};
