---
name: npc-sprite-art
description: Draw or revise native-resolution human overworld NPC sprites in Pokemon Emerald's style, including hair, clothing, indexed palettes and stable walking frames. Use for trainer or NPC sprite concepts, sprite sheets and visual fixes. This skill creates artwork and previews; game registration and map placement belong to npc-integration. It does not cover battle portraits or Pokemon sprites.
---

# NPC Sprite Art

## The project's style rule

**Use an existing Emerald NPC's body and face as the base. Make the character
individual through hair and clothes.** Most ordinary human NPCs share a small
set of body proportions, poses, facial pixels and expressions. The limited grid
does not support independently inventing detailed faces for every character.

Keep the base's face, eyes, skin clusters, stance, arm and leg placement, and
foot anchor unless the requested character expressly needs a distinction. Choose
a different appropriate native base for a different age, build or posture rather
than distorting one. A different NPC should have a recognizably different haircut
and outfit; a palette swap plus a tiny badge is usually not enough.

The accepted experiment used `boy_2.png` as the structural base, changed its
hairstyle to a swept side part, and changed the clothes to an open jacket over a
contrasting shirt. It retained the original face and body. That is an example of
the method, not a character design to repeat for every request.

## Scope and method

- Art-only requests produce sprites and previews. Do not edit graphics constants,
  game assets, maps or scripts when the user asks only to see a concept. Save
  concepts outside game-consumed paths, preferably outside the working tree for
  an explicitly experimental preview. Do not restore discarded experiments.
- When integration is requested, also use
  [npc-integration](../npc-integration/SKILL.md).
- The project's requested workflow is direct editing of native bitmap pixels,
  using a pixel editor or deterministic pixel operations. For scripted edits,
  Pillow indexed images and explicit pixel coordinates or palette-index grids
  are suitable. Respect applicable tool instructions and the user's chosen method.
- Do not generate a large illustration and shrink it into a sprite. That approach
  produced the rejected experiment: mismatched proportions, unstable details,
  inconsistent anchors and visible shimmering during animation. A prompt saying
  "16×32 pixel art" does not make a large generated image native-resolution art.
- Do not replace the common body and face with a wholly hand-invented character
  silhouette simply to make the NPC different. The second rejected experiment
  made that mistake. Invent the hair and garment shapes on the established base.

## Inspect the base at its real resolution

Start with `graphics/object_events/pics/people/`. Useful normal-human references
include `boy_2.png`, `camper.png` and `youngster.png`. Inspect their corresponding
entries in `src/data/object_events/object_event_graphics_info.h` for dimensions,
palette tag and animation table. Do not assume every people sprite has nine
16×32 frames; some have different dimensions, facing conventions or extra poses.

View the native sprite and an integer nearest-neighbor enlargement. A palette-
index grid is useful for precise edits, but keep a visual reference in view.
Compare the new sprite with native NPCs at the same scale, on the same background.

A **16×32 frame is a canvas, not a 16×32-tall character**. Normal NPCs often occupy
only roughly 19–21 rows near its bottom. For example, Boy2 idle pixels occupy
rows 12–30; Camper idle pixels occupy rows 11–30. Their standard walking poses
move down one row. These are reference observations, not universal bounding boxes.
Preserve the selected base's actual anchors. Do not trim, independently center,
stretch or normalize each pose to its own content bounds.

## Design a distinct character on that base

1. Pick a small, coherent design: a distinct hair part/fringe/tuft arrangement,
   plus a garment construction such as an open jacket, overalls or a collared
   shirt. Use a small accessory only when it reads at native scale.
2. Work on the standing front view first. Redraw the hair mass, part and highlight
   clusters above and around the unchanged face. Preserve familiar eye spacing,
   mouth/chin pixels and the base's head-to-body proportion.
3. Redraw clothing inside the existing torso and limb structure. Use garment
   edges, collar, shirt opening, sleeves and hem to establish identity. Preserve
   skin pixels and the pose; do not turn cloth shadows into changed anatomy.
4. Keep pixel clusters deliberate and shading compact, commonly three tones per
   material. Match Emerald's outline contrast and light direction. Avoid stray
   pixels, antialiasing, gradients and high-detail illustration shading.
5. Extend the same design to up and left views using the base's corresponding
   poses. Clothing and hair details should occupy consistent places on the body.
   Do not invent a new face or proportions for each view.

For a concept, front/left/back standing views are often enough. For an integrated
walking NPC, extend the chosen design to every required frame. A sketch or
three-view preview is not a complete game-ready animation sheet. Show artwork
when it helps the user assess the design; no extra approval gate is required when
the user already authorized the complete creation and integration task.

## Palette and bitmap format

Game-ready standard NPCs use a **144×32 indexed PNG**, nine horizontal **16×32**
frames, **4 bpp**, with **16 palette entries including transparent index 0**.
Keep material ramps consistent across all views and poses.

Prefer a suitable existing `graphics/object_events/palettes/npc_1.pal` through
`npc_4.pal` when making an asset for integration. Preserve its RGB values **and
index order**. Merely reducing the number of colors is insufficient. Do not let an
automatic quantizer or optimizer reorder the palette. Do not change a shared
palette for one sprite.

For standalone concepts, a custom Emerald-like 16-entry palette is acceptable,
but label it as needing palette integration if that is still unresolved. If a
base uses the same indices for hair and shoes, recoloring that ramp recolors both:
remap pixels by material/region deliberately when those should differ.

Example indexed-image handling (Pillow):

```python
from pathlib import Path
from PIL import Image

colors = [int(c) for line in Path(palette_path).read_text().splitlines()[3:]
          for c in line.split()]
assert len(colors) == 48
sheet = Image.new("P", (144, 32), 0)
sheet.putpalette(colors)
# Paste or draw already-native frames using their intended palette indices.
sheet.save(output_path, bits=4, transparency=0)
```

Check the saved file again: indexed mode, dimensions, indices 0–15 and the exact
intended palette. Alpha in a preview is not a substitute for background index 0
in the GBA graphic.

## Build stable animation, not nine independent drawings

For `sAnimTable_Standard` the frame indices are:

| Index | Pose |
| --- | --- |
| 0 | Down idle |
| 1 | Up idle |
| 2 | Left idle |
| 3, 4 | Down steps A, B |
| 5, 6 | Up steps A, B |
| 7, 8 | Left steps A, B |

Right-facing poses are mirrored from left. An asymmetric badge or satchel mirrors
with the character; independently drawn right-facing frames require different
engine wiring. Do not silently change the frame layout for an accessory.

Copy the directional head artwork consistently onto the base's walking poses.
Retain the base's intentional **one-pixel vertical walking bob**; it is not a
flicker bug. After compensating for that bob, hair and face pixels should normally
match exactly between the same direction's idle and step poses. Move garment
details with the torso and preserve the original limb motion and ground contact.
Do not freeze the entire sprite just to make all frames identical.

The engine walking loops are **3,0,4,0** (down), **5,1,6,1** (up), and **7,2,8,2**
(left); right mirrors left. Standard walking displays each frame for 8 game
frames, approximately 133 ms at 60 Hz. Preview those actual loops. Cycling frames
0 through 8 in sequence is not an animation test.

## Visual and structural QA

- Inspect at 1× and at an integer enlargement. The sprite should look like a
  native NPC placed beside its base, while the haircut and outfit identify a
  different character. A valid PNG does not establish style quality.
- Compare the face and skin pixels with the chosen base. For an ordinary clothing
  and hair edit, they should remain unchanged. Compare body masks and foot anchors;
  changed hair outlines are expected, changed anatomy requires a design reason.
- In walking work, compare registered directional heads pixel for pixel, inspect
  full movement loops for shimmer, and check left/right mirroring. Check intended
  arm/leg changes as well as stability; identical copies are not walking poses.
- Inspect palette correctness, transparent borders and frame boundaries. For the
  standard nine-frame layout, the packed 4bpp data should total 2304 bytes.
- Preserve the native PNG separately from presentation assets. Enlarge only the
  **preview**, using nearest-neighbor scaling, never the game asset.

For a neutral-background or transparent preview, derive a mask from **indices**,
not palette luminance. In Pillow, `indexed.point(...).convert("L")` can retain an
unintended palette and produce an inverted or blank silhouette. Use:

```python
mask = Image.new("L", frame.size)
mask.putdata([255 if index != 0 else 0 for index in frame.getdata()])
preview.paste(frame.convert("RGB"), position, mask)
```

Show the finished sprite inline. For walking work, show an animated preview when
supported. Say whether the result is a concept or a complete sheet, and avoid
claiming in-game validation when only artwork or static checks were inspected.
