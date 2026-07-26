# Community Requests

The city asks things of its gym. Requests are pinned to the gym's mailboard —
sidequests like "come meet the leader", "bring an old man a POTION", or
"someone settle down that rowdy trainer". Most appear as the gym climbs the
rank ladder; the rest are posted by an NPC in conversation.

The system is built for **50 one-shot requests**. Repeatable requests are
deliberately out of scope: they need a different state model (a completion
count and a reset rule) and should get their own pass.

## Where things live

| Piece | File |
| --- | --- |
| Counts, status values, request ids | `include/constants/community_requests.h` |
| The request table (names, blurbs, zones, rewards) | `src/data/community_requests.h` |
| State, unlock logic, script specials, board UI | `src/community_requests.c` |
| C declarations | `include/community_requests.h` |
| Shared "pay the reward" script | `data/scripts/community_requests.pory` |
| Example requests and the board object | `data/maps/TestingGrounds_Exterior1/` |

## State

Each request owns **one byte** in `SaveBlock3` (`gSaveBlock3Ptr->requests[]`):
the low two bits are the status (`LOCKED` / `ACTIVE` / `DONE`), the remaining
six are the step counter for multi-step requests. Fifty requests cost 50 bytes;
`SaveBlock3` sits at 328 of its 1624 available bytes with them included.

The byte is plain, not encrypted — nothing here is worth protecting the way the
Gym Points wallet is, and `ClearSav3`'s zero-fill gives exactly the right
new-game default (locked, step 0).

Everything else about a request is const ROM data, so adding requests costs
save space only when `NUM_COMMUNITY_REQUESTS` itself goes up. **Raising that
count changes the save layout and invalidates existing saves.**

`sCommunityRequests` is sparse: a slot with no `name` is a reserved id with no
content yet, and both the board and the unlock specials skip it. That is how
the table can be sized for 50 while only a few are written.

## Unlocking

- **By rank.** `unlockRank` 1-8 posts the request when the gym reaches that
  rank. `CommunityRequests_UnlockForRank` runs from `GymChallenge_RankUp`, from
  `NewGameInitData`, and again every time the board is opened. It posts
  everything at or below the current rank that is still locked, so a missed
  call is repaired by the next one and a save that predates a request catches
  up the first time the leader reads the board.
- **By script.** `unlockRank = REQUEST_UNLOCK_SCRIPT` leaves it to a map
  script calling `CommunityRequests_UnlockRequest`.

A request that exists but is not posted yet shows on the board as `???`. That
is intentional: the board is how the player sees there is more coming.

## The board

The board is a **sign metatile plus a BG event**, not an object event. Every
sign-shaped object graphic in the expansion (`OBJ_EVENT_GFX_SIGN`, `GYM_SIGN`,
`TRAINER_TIPS`) declares `paletteSlot = PALSLOT_NPC_4`, which the overworld
overwrites at map load with the standard NPC palette set — the sprite spawns
but renders invisible. Signs belong in the tilemap; metatile `0x003` in
`gTileset_General` is the wooden sign post the rest of the game uses.

Watch the object-event budget on a map generally: each distinct NPC graphic
wants its own sprite palette, `LoadSpritePalette` (`src/sprite.c:1638`) returns
`0xFF` once all 16 are taken, and objects past that point quietly fail to
appear. Reuse graphics that share `OBJ_EVENT_PAL_TAG_NPC_1`-`NPC_4` where the
exact sprite doesn't matter.

Interacting runs `CommunityRequests_OpenBoard`, a full-screen browser built
on the same bones as the TM Machine (`src/tm_machine.c`) — standard message
frames on a black backdrop, no new graphics. List on the left, details on the
right (zone, `STEP x/y`, money and item rewards), the request's blurb along the
bottom. **L/R** (or left/right) swaps between `IN PROGRESS` and `COMPLETED`;
**B** or Cancel closes it. Rows are read, not chosen — A does nothing.

Zone labels are plain strings in the table rather than `MAPSEC` names. The
city's maps don't exist yet, and quest zones will likely be finer-grained than
map sections anyway; switch to `GetMapName` later if that stops being true.

## Writing a request

1. Add an id to `include/constants/community_requests.h` (append only — ids are
   save slots).
2. Add its entry to `sCommunityRequests`. Keep the name inside ~18 characters
   and hand-break the description into two ~34 character lines; neither field
   auto-wraps.
3. Write the map script. The vocabulary:

   ```
   setvar(VAR_0x8004, REQUEST_YOUR_REQUEST)
   special(CommunityRequests_GetRequestStatus)   // -> VAR_RESULT
   special(CommunityRequests_GetRequestStep)     // -> VAR_RESULT
   special(CommunityRequests_UnlockRequest)      // post it from a script
   special(CommunityRequests_AdvanceStep)        // -> VAR_RESULT = new step
   special(CommunityRequests_HasRoomForRewards)  // -> VAR_RESULT
   call(CommunityRequests_EventScript_PayReward) // mark done, pay out
   ```

   Always check `HasRoomForRewards` before `PayReward`, and print a "your bag is
   full" line when it fails — `PayReward` marks the request done either way.

The payout is deliberately a single beat: `CompleteRequest` adds the money and
every item stack in one go, then the script plays one fanfare over one
single-page message. Splitting it per item (via `STD_OBTAIN_ITEM`) gave two
fanfares and three message boxes with inconsistent skip behaviour. `PayReward`
writes `STR_VAR_1` (money), `STR_VAR_2` (the item stacks as readable text) and
`VAR_0x8006` (how many stacks were given, which picks the message); the request
id in `VAR_0x8004` is untouched.

A step that needs a battle is just `trainerbattle_*` before `AdvanceStep`.

## The three examples

On the testing map, each example exercises one mechanism:

- **Meet the Leader** (`REQUEST_GREET_THE_LEADER`) — rank 1 batch, no steps,
  money only. The visitor at (3,10).
- **A Potion, Please** (`REQUEST_POTION_FOR_THE_OLD_MAN`) — script-posted. The
  old man at (9,10) asks in person, then takes a POTION for money plus items.
- **The Rowdy Trainer** (`REQUEST_ROWDY_TRAINER`) — rank 1, three steps, so the
  board's `STEP x/3` readout moves as the conversation goes on. At (11,10).

The board sign is at (5,10) and a supply crate sign at (13,10) hands out
POTIONs, since the test map has no MART. All of it sits on the row just above
the map's entrance warp.

The other 47 slots are reserved and inert.
