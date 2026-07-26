---
name: community-requests
description: Add, edit, or debug Community Requests — the sidequests the city posts on the gym's mailboard (quest names, blurbs, zones, rank-gated or script-posted unlocks, substeps, money and item rewards, the board UI). Use this skill whenever the work touches a quest/sidequest/request/errand/mission/bounty in this hack, the request board or mailboard, anything under src/community_requests.c, src/data/community_requests.h, include/constants/community_requests.h, or data/scripts/community_requests.pory — even if the user just says "add a quest where someone wants X" without naming the system. Also use it when placing the board or quest NPCs on a map, since object-event placement here has traps that silently break the overworld.
---

# Community Requests

The city asks things of its gym. Requests are pinned to a mailboard: "come meet
the leader", "bring an old man a POTION", "settle down that rowdy trainer". The
player reads the board, does the thing, gets paid.

The system is sized for **50 one-shot requests**. Repeatable requests are out of
scope — they need a completion count and a reset rule, and should get their own
pass rather than being bolted on.

Design notes and rationale live in `game_docs/community-requests.md`. This skill
is the how-to.

## The one critical rule

**A request id is a save slot.** `gSaveBlock3Ptr->requests[id]` is indexed by
the id directly, so renumbering or reordering ids silently rewrites every
existing save's quest progress. Only ever append new ids. Raising
`NUM_COMMUNITY_REQUESTS` changes the size of `SaveBlock3` and invalidates old
saves outright — do it deliberately, not casually.

## Where things live

| Piece | File |
| --- | --- |
| Counts, status values, request ids | `include/constants/community_requests.h` |
| The request table | `src/data/community_requests.h` |
| State, unlock logic, specials, board UI | `src/community_requests.c` |
| C declarations | `include/community_requests.h` |
| Shared payout script | `data/scripts/community_requests.pory` |
| Example requests + the board | `data/maps/TestingGrounds_Exterior1/` |

## How state works

Each request owns **one byte**: the low two bits are the status
(`REQUEST_STATUS_LOCKED` / `ACTIVE` / `DONE`), the upper six are the step
counter. Fifty requests cost 50 bytes, and `SaveBlock3` has well over a
kilobyte spare.

The byte is plain, not encrypted — there's nothing here worth protecting the way
the Gym Points wallet is, and `ClearSav3`'s zero-fill gives exactly the right
new-game default (locked, step 0).

Everything else about a request is const ROM data, so adding content costs no
save space.

`sCommunityRequests` is **sparse**: a slot with no `name` is a reserved id with
no content yet, and both the board and the unlock specials skip it. That is how
the table can be sized for 50 while only a handful are written.

## Adding a request

### 1. Reserve an id

Append a `#define REQUEST_*` to `include/constants/community_requests.h`. That
header is included from `data/event_scripts.s` as well as C, so it holds
preprocessor definitions only — no C declarations.

### 2. Fill in the table

Add a designated-initializer entry to `sCommunityRequests` in
`src/data/community_requests.h`:

```c
[REQUEST_ROWDY_TRAINER] =
{
    .name = COMPOUND_STRING("The Rowdy Trainer"),
    .description = COMPOUND_STRING("A trainer is picking fights by the\ngate. Hear them out, then settle it."),
    .zone = COMPOUND_STRING("Testing Grounds"),
    .unlockRank = 1,                 // or REQUEST_UNLOCK_SCRIPT
    .numSteps = 3,                   // omit or 0/1 for no step readout
    .money = 1200,
    .items = { { ITEM_GREAT_BALL, 3 } },
},
```

**Text budgets.** Nothing auto-wraps, so both fields are hand-fitted:

- `name` — one list row, roughly **18 characters**. Longer names get squeezed by
  the narrow-width fallback and start looking cramped.
- `description` — **two lines of about 36 characters**, split with an explicit
  `\n`. A third line is drawn outside the window and clipped away.
- `zone` — one line in a 104px panel, so keep it short. Zones are plain strings
  rather than `MAPSEC` names on purpose: the city's maps don't exist yet, and
  quest zones will likely be finer-grained than map sections anyway.

**Unlocking.** `unlockRank` 1-8 posts the request when the gym reaches that
rank. `REQUEST_UNLOCK_SCRIPT` (0) leaves it to a map script.
`CommunityRequests_UnlockForRank` runs on rank-up, on new game, and every time
the board is opened, posting everything at or below the current rank that is
still locked — so a missed call repairs itself and old saves catch up.

A request that exists but isn't posted shows as `???`. That's intentional: the
board is how the player sees there is more coming.

### 3. Write the map script

The vocabulary, with the request id in `VAR_0x8004`:

```
setvar(VAR_0x8004, REQUEST_YOUR_REQUEST)
special(CommunityRequests_GetRequestStatus)   // -> VAR_RESULT
special(CommunityRequests_GetRequestStep)     // -> VAR_RESULT
special(CommunityRequests_UnlockRequest)      // post it from a script
special(CommunityRequests_AdvanceStep)        // -> VAR_RESULT = new step
special(CommunityRequests_HasRoomForRewards)  // -> VAR_RESULT
call(CommunityRequests_EventScript_PayReward) // mark done, pay out
```

Switch on the status and handle all three states — an NPC that only handles
`ACTIVE` says nothing at all before the request is posted, which reads as a bug.

Check `HasRoomForRewards` before `PayReward` and print a "your bag is full"
line when it fails; `PayReward` marks the request done either way. A step that
needs a battle is just `trainerbattle_*` before `AdvanceStep`.

The specials fail closed: an id that isn't a posted, written request is inert
rather than fatal. `AdvanceStep` and `CompleteRequest` additionally require the
request to be `ACTIVE`, so a script bug can't pay out a locked quest.

Use the **poryscript** skill for the script itself — these are `.pory` files and
the `.inc` siblings are generated.

## The payout is one beat

`CompleteRequest` adds the money *and* every item stack in one call, then the
shared script plays a single fanfare over a single-page message. It writes
`STR_VAR_1` (money), `STR_VAR_2` (item stacks as readable text) and
`VAR_0x8006` (how many stacks, which picks the message). `VAR_0x8004` survives.

This shape is deliberate. Handing items over one at a time through
`STD_OBTAIN_ITEM` produced two fanfares and three message boxes with
inconsistent skip behaviour — the first skippable, the last not. If you add
reward kinds, keep them inside `CompleteRequest` and inside that one message
rather than appending another announcement.

## Putting things on a map

This is where the traps are. Both of these cost real debugging time already.

**Signs must be metatiles, not object events.** Every sign-shaped object graphic
in the expansion (`OBJ_EVENT_GFX_SIGN`, `GYM_SIGN`, `TRAINER_TIPS`) declares
`paletteSlot = PALSLOT_NPC_4`, and the overworld overwrites that slot at map
load with the standard NPC palette set. The sprite spawns and is simply
invisible. Put a sign in the tilemap instead — metatile `0x003` in
`gTileset_General` is the wooden post the rest of the game uses — and attach a
`bg_events` entry of type `sign`. Set collision so the player can't stand on it:

```python
# raw = (elevation << 12) | (collision << 10) | metatile
SIGN = (3 << 12) | (1 << 10) | 0x003
```

**Watch the sprite-palette budget.** Each distinct NPC graphic wants its own
sprite palette, there are 16 total, and `LoadSpritePalette` (`src/sprite.c`)
returns `0xFF` once they're gone. Objects past that point quietly fail to spawn,
and the half-initialised state can take the overworld down on a map transition —
it presents as a freeze or a reboot a couple of steps after entering the map,
which looks nothing like a palette problem. When the exact sprite doesn't
matter, pick graphics that reuse `OBJ_EVENT_PAL_TAG_NPC_1`-`NPC_4`; check what
the map already loads and match it.

To find candidates for a given palette tag:

```bash
python3 - <<'EOF'
import re
src = open('src/data/object_events/object_event_graphics_info.h').read()
for name, body in re.findall(r'gObjectEventGraphicsInfo_(\w+) = \{(.*?)\n\};', src, re.S):
    m = re.search(r'\.paletteTag = (\w+)', body)
    if m and m.group(1) == 'OBJ_EVENT_PAL_TAG_NPC_3':
        print(name)
EOF
```

Object events and bg events are edited in `map.json`; `events.inc` and
`include/constants/map_event_ids.h` are generated from it, so never hand-edit
those. **Map changes only appear after the player leaves and re-enters the map**
— a loaded save keeps the old event list, which makes fresh edits look like they
did nothing.

## Touching the board UI

The info panel is 96px tall and every row is spoken for. `FONT_SMALL` is 12px
and `FONT_NARROW` is 16px, and the rows stack flush:
`12 + 12 + 16 + 12 + 12 + 16 + 16 == 96`. Adding a gap anywhere pushes the last
reward row out of the window; adding a row means taking one away. Text drawn
past the window edge is clipped, not corrupted, so this fails as a cosmetic
overlap rather than a crash.

The board is a read-only browser modeled on `src/tm_machine.c` — standard
message frames on a black backdrop, no custom graphics. L/R and left/right swap
tabs, which is free because the list template uses `LIST_NO_MULTIPLE_SCROLL` and
therefore ignores those buttons.

## Verifying

1. `make` — the `SaveBlock3` static assert in `src/save.c` proves the save still
   fits, and poryscript compiles the `.pory` files.
2. **Start a new game** when the save layout changed, and remember the
   walk-out-and-back-in rule for map edits.
3. On the test map the row above the entrance warp has everything: visitor at
   x=3, board sign at x=5, old man at x=9, rowdy trainer at x=11, supply crate
   sign at x=13 (POTIONs, since there's no MART). The Rank Examiner ranks you up
   on demand, which is how to exercise a batch unlock.
4. Walk the request end to end and check the board between steps: the name and
   blurb read correctly, the zone and `STEP x/y` are right, the reward matches
   the table, and the request moves from IN PROGRESS to COMPLETED.
