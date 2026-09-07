---
name: npc-integration
description: Register an overworld NPC graphic, its palette and animation frames, and place or update an NPC on a map in this Emerald project. Use for wiring custom NPC sprites into the game, object-event graphics IDs, map placement, or debugging invisible sprites, wrong colors and frame selection. For drawing or previewing the artwork itself, use npc-sprite-art.
---

# NPC Integration

Integrate a normal human overworld NPC using the existing object-event system.
A trainer-looking NPC does not imply a trainer battle: only add battle behavior,
trainer data or battle portraits when requested.

## Scope and starting point

- Inspect the working tree first. Do not restore discarded experiments or depend
  on example assets from a previous conversation still existing.
- Distinguish registering a reusable graphic from placing an NPC. Do the portions
  requested. An image-only request does not authorize game integration.
- For new art or visual defects, use [npc-sprite-art](../npc-sprite-art/SKILL.md).
- For dialogue, movement scripts and map scripts, use
  [poryscript](../poryscript/SKILL.md). For quest NPCs, also use
  [community-requests](../community-requests/SKILL.md).
- Follow the project's graph-first code-discovery instructions. The exact files
  below are known integration points; inspect their current contents directly.

Success means the intended graphic is registered, the NPC appears at its intended
location, its colors and directional animation are correct, interaction works,
and the ROM builds. State which runtime checks were actually performed.

## Normal human sprite contract

For the common `sAnimTable_Standard` / 16×32 NPC setup:

- PNG: **144×32**, nine horizontal **16×32** frames, indexed, **4 bpp**.
- Each frame packs to **256 bytes**; the complete graphic is **2304 bytes**.
- Frame order: **down idle, up idle, left idle, down step A, down step B,
  up step A, up step B, left step A, left step B**.
- Right-facing animations mirror the left frames automatically. Do not insert
  right-facing frames into this layout. Asymmetric designs needing independent
  right-facing art require a different animation configuration.
- Index **0** is transparent in-game. Palette indices must match the registered
  palette exactly. A visually correct PNG with reordered indices renders wrong
  colors in-game; PNG alpha alone does not establish the GBA transparent index.

Some native sprites use different sizes or additional frames. Copy the setup of
an appropriate standard NPC such as `Boy2` or `Camper`, not an arbitrary character.

## Register the asset

Use one consistent name across these entries (`NewTrainer` is an example):

| File | Addition |
| --- | --- |
| `graphics/object_events/pics/people/new_trainer.png` | Final native indexed sprite sheet |
| `include/constants/event_objects.h` | Append `OBJ_EVENT_GFX_NEW_TRAINER` immediately before `NUM_OBJ_EVENT_GFX` |
| `src/data/object_events/object_event_graphics.h` | Graphic data declaration |
| `src/data/object_events/object_event_pic_tables.h` | Frame-image table |
| `src/data/object_events/object_event_graphics_info.h` | `ObjectEventGraphicsInfo` definition |
| `src/data/object_events/object_event_graphics_info_pointers.h` | Extern declaration and designated pointer-table entry |

Append the ID: do not renumber existing static graphics IDs or hardcode the value
of `NUM_OBJ_EVENT_GFX`. Keep Emerald additions outside FRLG-only conditionals.

```c
// object_event_graphics.h
const u32 gObjectEventPic_NewTrainer[] = INCGFX_U32(
    "graphics/object_events/pics/people/new_trainer.png",
    ".4bpp", "-mwidth 2 -mheight 4");

// object_event_pic_tables.h
static const struct SpriteFrameImage sPicTable_NewTrainer[] = {
    overworld_ascending_frames(gObjectEventPic_NewTrainer, 2, 4),
};

// object_event_graphics_info.h — example using the existing NPC_1 palette
const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_NewTrainer = {
    .tileTag = TAG_NONE,
    .paletteTag = OBJ_EVENT_PAL_TAG_NPC_1,
    .reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE,
    .size = 256,
    .width = 16,
    .height = 32,
    .paletteSlot = PALSLOT_NPC_1,
    .shadowSize = SHADOW_SIZE_M,
    .inanimate = FALSE,
    .compressed = FALSE,
    .tracks = TRACKS_FOOT,
    .oam = &gObjectEventBaseOam_16x32,
    .subspriteTables = sOamTables_16x32,
    .anims = sAnimTable_Standard,
    .images = sPicTable_NewTrainer,
};

// object_event_graphics_info_pointers.h, with the other extern declarations
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_NewTrainer;
// Inside gObjectEventGraphicsInfoPointers:
[OBJ_EVENT_GFX_NEW_TRAINER] = &gObjectEventGraphicsInfo_NewTrainer,
```

The `.size` field is the size of **one frame**, not the whole sheet. The normal
build handles PNG conversion; do not introduce a separate graphics build system.

## Palette choice is part of the integration

Prefer an existing `graphics/object_events/palettes/npc_1.pal` through `npc_4.pal`
and its matching `OBJ_EVENT_PAL_TAG_NPC_*` / `PALSLOT_NPC_*` pair. Do not modify a
shared palette to suit one NPC: that recolors other users of the palette.

A custom preview palette is not automatically game-ready. Either explicitly map
its pixel indices to a suitable shared palette and inspect the result, or follow
an existing custom-palette NPC through the palette definitions, tag lookup,
loading and slot behavior. Copying only the graphic-info structure is insufficient.
Check reflections if the chosen map requires them.

There are 16 hardware sprite palette banks, shared with the player and effects;
not all are available to new NPCs. Reused palette tags avoid extra allocations.
Palette exhaustion can silently prevent spawning and destabilize transitions.
A tag/slot mismatch can also make objects invisible or wrong-colored when the
map's standard palettes are loaded. Verify actual map-load behavior.

## Place the NPC and attach dialogue

Edit `data/maps/<Map>/map.json`, adding a unique local ID and an object event.
Follow neighboring object-event fields for the exact JSON schema:

- `graphics_id`: the new symbolic graphics ID.
- `x`, `y`: map tile coordinates, not screen pixels.
- `elevation`: compatible with the target floor.
- `movement_type` and movement ranges: choose existing constants; check every
  allowed pacing tile, not just the spawn position.
- `trainer_type`: `TRAINER_TYPE_NONE` for a conversational NPC.
- `trainer_sight_or_berry_tree_id`: `"0"` unless the requested behavior needs it.
- `script`: the map's actual event-script symbol.
- `flag`: `"0"` for always present, or a deliberate visibility flag.

Inspect `data/layouts/layouts.json` to locate the layout dimensions and block data.
A little-endian 16-bit block encodes metatile in bits 0–9, collision in bits 10–11,
and elevation in bits 12–15. Check terrain behavior as well as collision. Avoid
occupied tiles, warps, signs, narrow routes and scripted event positions. Check
active-object capacity (`OBJECT_EVENTS_COUNT`) as well as palette capacity.
`TestingGrounds_Exterior1` is useful when the user asks for a test location, but
its free coordinates must be checked anew.

A simple Poryscript interaction locks, faces the player, shows a message, then
releases and ends. Use `format()` for textbox wrapping. Append independent scripts
where practical to avoid shifting generated line markers throughout the file.
When `scripts.pory` exists, edit it; its `.inc` sibling is generated. Preserve all
existing content if converting a map that has only `.inc` scripts.

`events.inc` and `include/constants/map_event_ids.h` are generated from map JSON:
do not hand-edit them. Include regenerated tracked script output with the source.

## Verify

1. Validate sprite dimensions, palette indices, frame order and packed size.
   Run the existing converter with `-mwidth 2 -mheight 4` into a temporary `.4bpp`
   file when useful; the standard nine-frame asset should be 2304 bytes.
2. Compile the `.pory` using the commands in the poryscript skill, then run the
   normal project `make`. Use the configured toolchain; if it is installed there,
   `make -j8 TOOLCHAIN=/opt/devkitpro/devkitARM` is a working local invocation.
3. Load the updated ROM. **Leave and re-enter a map after loading an existing
   save**: a loaded save may retain old object events. Use a copy of the ROM and
   save for isolated testing when appropriate; do not overwrite the user's save.
4. Check spawn, colors, transparency, movement, all four facing directions,
   interaction, dialogue dismissal and resumed movement. Walk away and return
   to test map loading. For walking art, observe a complete cycle.
5. Report build, static and runtime results separately. A successful build or
   screenshot does not prove animation or interaction works.

If a sprite flickers, first distinguish artwork shimmer (hair, eyes or limbs
changing pixels between poses) from an engine problem. Compare the native frames
and their anchors before changing animation code or palette allocation.
