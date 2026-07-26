#ifndef GUARD_CONSTANTS_COMMUNITY_REQUESTS_H
#define GUARD_CONSTANTS_COMMUNITY_REQUESTS_H

// This header is included from data/event_scripts.s as well as C code:
// preprocessor definitions only, no C declarations.

// Community Requests: the sidequests the city posts on the gym's mailboard.
// Every request has a slot in SaveBlock3 (see gSaveBlock3Ptr->requests), so
// the count is a save-layout decision — raising it invalidates old saves.
#define NUM_COMMUNITY_REQUESTS 50

// Money plus this many item stacks per request.
#define REQUEST_NUM_REWARD_ITEMS 2

// Per-request save byte: status in the low bits, current step above it.
#define REQUEST_STATUS_LOCKED 0 // Not posted yet, or a slot with no request.
#define REQUEST_STATUS_ACTIVE 1 // Posted; shows under IN PROGRESS.
#define REQUEST_STATUS_DONE   2 // Completed and paid out.

#define REQUEST_STATUS_MASK  0x03
#define REQUEST_STEP_SHIFT   2
#define REQUEST_MAX_STEPS    63 // What fits in the remaining six bits.

// unlockRank value for requests that only a map script may post.
#define REQUEST_UNLOCK_SCRIPT 0

// Request ids. Ids are save slots: only ever append, never renumber.
#define REQUEST_GREET_THE_LEADER   0
#define REQUEST_POTION_FOR_THE_OLD_MAN 1
#define REQUEST_ROWDY_TRAINER      2

#endif // GUARD_CONSTANTS_COMMUNITY_REQUESTS_H
