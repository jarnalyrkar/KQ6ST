# Changelog

All notable changes to the project. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Changes before the first release are
grouped by date. Details and the reasoning behind each fix are in
[`PROJECT.md`](PROJECT.md) (the "findings" numbers below refer to its findings list).

## [Unreleased]

### Added
- Real-hardware check before a release: `tools/hatari/hc.sh -acsi` runs the game from an ACSI hard-disk
  image with the AHDI driver (`tools/acsiput.py` puts the folder on it). v0.1.0 passes it: boots, plays,
  saves and restores; none of the KQ5 ST real-hardware bugs apply (PROJECT.md "Testing like real hardware").

## [0.1.0] - 2026-10-20

First release: a ready-to-play hard-disk folder (`KQ6ST-v0.1.0.zip`), built with
`tools/mkrelease.sh` from the unmodified ST disk image. It contains everything below.

### 2026-10-20

#### Added
- `tools/mkrelease.sh` and the player's `READ_ME.TXT` for release zips.
- Click to interact: a right click or double click on something in a room types its command
  ("get ring", "open box" …) and the room's own script does the rest. Alexander first walks to
  where the script allows the action. 347 hotspots in all 51 rooms that have commands
  (findings 23).
- Pathfinding: Alexander finds a way around walk barriers to a hotspot; the walk waits while the
  game takes control and then goes on (findings 26).
- A plain single click also walks around barriers, and a click above the horizon or near the
  left, right or bottom edge leaves the room that way (findings 27).
- Mouse-driven game cursors in the vizier's ebony box, the trunk of letters, the beach box and the
  sword fight: the arrow or sword follows the mouse, a click counts as the key. Portrait dialogues
  close on a click (findings 21).
- Title choice with the mouse: hovering Opening / Play / Restore moves the highlight, a click
  starts it (findings 18).
- Clickable number menus: the ferryman's "Ask about" list and a new pawn-shop menu (Talk, Trade,
  Take mint, Leave; opened by clicking the owner). Typing the number still works (findings 24).
- The inventory picker takes the pointer: hover highlights, a click picks (findings 25).

#### Fixed
- M9: animations (oyster and gnomes, the flames at the Lord of the Dead, the Minotaur room) no
  longer draw over open message boxes (findings 22).

### 2026-10-19

#### Added
- M2: pushing the pointer against the status line opens the menu bar (findings 17).
- M8: typed `talk`, `trade`, `give` and `take mint` in the pawn shop (findings 16).
- `load` as a synonym for `restore`.
- Esc skips the disclaimers and the title texts; Play is preselected on the title choice
  (findings 18).

#### Fixed
- M1: the mouse pointer no longer erases text (title buttons, spell book, `display` text)
  (findings 19).
- M5: black text on blue boxes (map, ferryman, vizier's letter) now uses the colour it sits on
  (findings 20).
- Long path: after the Cliffs of Logic skip, Alexander was not drawn at the top once the Minotaur
  was dead, which left the game without input (findings 13).

#### Tested
- The whole game played through in Hatari (long path) with no crashes or error messages.

### 2026-10-18

#### Added
- Runs from a plain hard-disk folder without Petari's loader (`tools/mkhd.sh`); saves go to the
  folder and Quit returns to the desktop (M3, M4).
- 99 save slots in a scrolling list instead of 12.
- M13: the Cliffs of Logic climb is skipped after the first etching.
- Tools: FAT12 image reader, AGI resource reader and logic disassembler, TOS program
  disassembler, logic and interpreter patchers, headless Hatari test harness.

#### Fixed
- G1–G3, G6: "memory error" at the Minotaur, the druids, Charon and Jollo's lamp. The
  interpreter's game heap is enlarged from 64 KB to 128 KB.
- G8: 2 bombs when giving the milk to the baby's tears. Releasing heap memory now cuts the sound
  list, which could point into freed memory (findings 15).
- G7: trading in the pawn shop after using the arrow keys gave the map talk and lost the item.

[Unreleased]: https://github.com/jarnalyrkar/KQ6ST/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/jarnalyrkar/KQ6ST/releases/tag/v0.1.0
