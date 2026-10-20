# King's Quest VI AGI demake – Atari ST: bug-fix project

Notes for fixing the bugs found while playing the Atari ST version on real hardware.
Read this first in a new session; keep it updated as things are learned.

## Current state (2026-10-19, full game completed)

- **What the user plays**: `build/KQ6ST-HD/` – a hard-disk folder run *without* PP's loader, in Hatari
  (`hatari --machine ste -d build/KQ6ST-HD --auto 'C:\SIERRA.PRG'`, user's config: STE, 4 MB, TOS 1.62
  SE, 8 MHz). Real machine: stock 1040ST, 4 MB. The folder has the user's saves (`sg.1`–`sg.47`+);
  never delete or overwrite `sg.*` there.
- **In the folder**: `SIERRA.PRG` = Sierra ST AGI 1.422 + heap 128 KB + 99 scrolling save slots +
  sound-list fix on heap release (findings 15);
  `MG.PRG` = copy of it (disk check); `LOGDIR`/`VOL.1` = original + script patches (logics 8 and 15
  appended). Rebuild everything with `tools/mkhd.sh <E.ST> <outdir>`; only the script patches:
  `python3 -I tools/logicpatches.py work/est build/KQ6ST-HD`.
- **FULL GAME COMPLETED (2026-10-19)**: the user finished the whole game in Hatari from
  `build/KQ6ST-HD` (they were on the long path) with **no bombs and no error messages**. So the heap
  fix covered druids/Charon/Jollo, G8 (milk) and the cliffs-skip fix work, and the magic door +
  Nightmare 4-bomb crashes (G4/G5) did not happen with the current build (likely the same heap/sound-list
  family, fixed by findings 15). Still untested: the real 1040ST.
- **Done and confirmed by the user's play**: heap fix (Minotaur room worked, short path finished),
  saving to the folder, 99 slots, cliffs skip, pawn-shop trade fix (user's save `sg.45` "got flute").
- **User is on the long path** (from `sg.40` "long, chess pieces"). Still to come there: druids,
  Charon (heap fix should cover them), magic door + Nightmare (4 bombs, not fixed), Jollo's lamp.
- **G8 fixed (2026-10-18 late)**: 2 bombs on "give milk" to the baby's tears (user's `sg.48`) = sound list
  left pointing into freed heap (findings 15). Interpreter fix in `build/KQ6ST-HD`, tested in Hatari from
  `sg.48`; the user needs to restart the game to get it. Awaiting the user's confirmation.
- **Cliffs skip, long path (2026-10-19)**: after the skip, room 25 never drew Alexander once the
  minotaur is dead (flag 117), so Night Mare grazed forever with no ego and no input. Fixed in logic 25
  (`p025` in `logicpatches.py`, findings 13), tested in Hatari from the user's `sg.76`; the user must
  restart the game to get it.
- **Pawn shop typed commands (M8, 2026-10-19)**: in `build/KQ6ST-HD`, **confirmed by the user**: talk,
  trading the coin, then flute, brush and bird all worked (findings 16). Map-related answers depend on
  having talked to the ferryman (original game logic, flag 67).
- **Mouse menu (M2, 2026-10-19)**: pushing the pointer up against the status line opens the menu bar
  (hover over a title opens it, click or Return picks, Esc or a click elsewhere closes). In
  `build/KQ6ST-HD/SIERRA.PRG` + `MG.PRG` (findings 17), tested in Hatari; the user must restart the game.
- **"load" = "restore" (2026-10-19)**: `load` added to word group 4 in `WORDS.TOK`
  (`tools/wordpatches.py`, run by `mkhd.sh`; the encoder round-trips the original byte for byte).
  Installed in `build/KQ6ST-HD`; the user must restart the game to get it.
- **Esc skips the intro (2026-10-19)**: Esc on the first disclaimer (room 97) goes on to the title room,
  Esc during the second disclaimer / title texts (room 255) goes straight to Opening / Play / Restore
  (findings 18); Esc on that choice is ignored and Play is preselected. Installed in `build/KQ6ST-HD`, tested in Hatari; the user must restart the game.
- **Pointer no longer erases text (M1, 2026-10-19)**: hovering over white-on-black text (title Opening /
  Play / Restore, the spell book, any `display` text) wiped the letters. Interpreter fix
  (`tools/slots/pointer.c`, findings 19), installed in `build/KQ6ST-HD` (`SIERRA.PRG` + `MG.PRG`), tested in
  Hatari (title, spell book, Alexander walking under the pointer, menu); the user must restart the game.
- **Blue text boxes (M5, 2026-10-19)**: black text on blue on the map, the ferryman's "Ask about" list and
  the vizier's letter now gets the colour it sits on (map: yellow names, brown numbers; ferryman and letter:
  white). Logics 12, 14, 72 (findings 20), installed in `build/KQ6ST-HD`, tested in Hatari; the user must
  restart the game.
- **Mouse-driven game cursors (2026-10-20)**: in the beach box (212), the vizier's ebony box (210) and trunk
  (211), and the three steps of the sword fight (111), the system pointer is hidden and the game's own cursor
  (arrow / sword) follows the mouse; a left click counts as the key. Portrait dialogues (logic 88) close on a
  click (and Esc) everywhere. Interpreter (`menu.c`) + logics 0, 88, 111, 210–212 (findings 21), installed in
  `build/KQ6ST-HD` (`SIERRA.PRG`, `MG.PRG`, `LOGDIR`, `VOL.1`), tested in Hatari; the user must restart the game.
- **Animations over text boxes (M9, 2026-10-20)**: sprites (oyster/gnomes on the Isle of Wonder, flames at the
  Lord of the Dead, minotaur room) no longer paint over an open message box. Interpreter (`tools/slots/blit.c`,
  findings 22), installed in `build/KQ6ST-HD` (`SIERRA.PRG`, `MG.PRG`), tested in Hatari from `sg.86`; the user must
  restart the game.
- **Click to interact (2026-10-20)**: beach confirmed by the user (double/right click + walking "worked great");
  then extended to 51 rooms on all islands (347 hotspots, `tools/slots/hotspots/*.h`), installed, partly tested in
  Hatari (see findings 23). Straight-line walking gets stuck where the stand spot is fenced by walk barriers (e.g.
  the pawn-shop door in room 7) → "not close enough"; pathfinding (level 2) is the fix. Beach details: a left double
  click or a right click on something in
  room 2 types its command ("get ring", "move plank", "open box", "look ocean"/rocks/ship/path/sand/plants) and
  the room logic does the rest. If he is outside the spot where the logic allows it, he first walks there in a
  straight line (ring, plank, box); stuck → "not close enough". Interpreter
  (`tools/slots/act.c`, hotspots in `tools/slots/hotspots.h`, findings 23), installed in `build/KQ6ST-HD`
  (`SIERRA.PRG`, `MG.PRG`), tested in Hatari; the user must restart the game.
- **Title choice with the mouse (2026-10-20)**: hovering Opening / Play / Restore moves the highlight there,
  a click starts it; arrow keys and Return still work (findings 18). Logic 255 + `menu.c`, installed in
  `build/KQ6ST-HD` (`SIERRA.PRG`, `MG.PRG`, `LOGDIR`, `VOL.1`), tested in Hatari; the user must restart the game.
- **Clickable number menus (2026-10-20)**: the ferryman's "Ask about" list and a new pawn-shop menu (1. Talk,
  2. Trade, 3. Take mint, 4. Leave; clicking the owner opens it) show the pointer and take a click on a line;
  typing the number still works, typed shop verbs unchanged (findings 24). Installed in `build/KQ6ST-HD`
  (`SIERRA.PRG`, `MG.PRG`, `LOGDIR`, `VOL.1`), tested in Hatari; the user must restart the game.
- **Pointer in the inventory picker (2026-10-20)**: when the game asks for an item (pawn-shop trade, giving
  things), hovering a name highlights it and a click picks it; arrow keys / Return / Esc unchanged (findings 25).
  Interpreter only (`SIERRA.PRG`, `MG.PRG`), installed in `build/KQ6ST-HD`, tested in Hatari; restart the game.
- **Pathfinding for click to interact (2026-10-20)**: when the straight walk to a hotspot's stand spot is blocked,
  Alexander now finds a way around barriers (`tools/slots/pf.c`, findings 26). Pawn-shop door in room 7 from both
  sides: walks around the stalls, the room's own scripted entry takes over, the door opens, he is in the shop. The
  walk now also waits through the game taking control (beggar's talk, scripted walks) and goes on afterwards.
  Installed in `build/KQ6ST-HD` (`SIERRA.PRG`, `MG.PRG`), tested in Hatari (room 7, beach); restart the game.
- **Pathfinding for plain clicks (2026-10-20)**: a single click now walks around barriers too, and a click above
  the horizon or near the left/right/bottom edge walks out of the room that way (findings 27). The user's routes
  tested in Hatari from a new game: beach plank + ring by right click, then one click on the path top → room 3; one
  click on the village → room 7; one click at the bottom → room 3; one click on the castle → castle path script → room 4.
  Installed in `build/KQ6ST-HD` (`SIERRA.PRG`, `MG.PRG`); **BSHIFT is now 0x7000**; restart the game.
- **Open**: G4/G5 not seen in the full Hatari playthrough (reopen only if they show up on real
  hardware); M6, M7, M10–M12, M14, ferryman part of M8 (see the bug table).

## Who made what

- **Game**: *King's Quest VI AGI Demake* by **Brandon Kouri** (2006–2024, https://kq6agi.com).
  It is built in Sierra's **AGI** engine (AGI Studio + WinAGI GDS), *not* AGS. The data in the ST image is the
  **8/30/2024** DOS release (`LOGDIR`/`VOL.*` dated 2024-08-30). The DOS release ships its own
  interpreter `KQ6AGI.EXE` (DOS AGI **2.917**, "tailor-made", per Brandon's readme).
- **Atari ST version**: **Marakatti** made the first working Atari ST disk image (`E.ST`) from Brandon's
  DOS version: the DOS game data run by **Sierra's own Atari ST AGI interpreter**. **Petari (signs as PP)**
  made the hard-disk conversion of that image (Sept + Nov 2024): his loader (`HAG2`, `RUNME*.TOS`) in
  `KQVIST/`. "PP" in these notes means Petari.
- The user played it through on real hardware, with debug help from Brandon. Bug list:
  `encountered-bugs.txt`.

## What is in `KQ6AGI/`

Brandon's DOS release (8/30/2024), added by the user 2026-10-18. It is the **compiled game, not
source**: every file is byte-identical to the same file inside `E.ST`, plus `README.txt` (=
`KQVIST/READMEO.TXT`). So the ST version uses Brandon's data unchanged. No logic source (`.txt`/`.lgc`),
no WinAGI project (`.wag`).

## What is in `KQVIST/`

| File | What it is |
|---|---|
| `RUNME.TOS` / `RUNME2.TOS` | PP's packed starters (1 MB / 2 MB). They load `KQ6HE1.TOS` / `KQ6HE2.TOS` (packed inside) and `HAG2`. |
| `HAG2` | PP's "HAG2" hard-disk game adapter: machine detect (ST/STE/Mega STE/TT/Falcon, CPU clock menu on M), runs the floppy image from hard disk, adds **state saves** (`/` key, per the bug list) and **exit to desktop (numpad `*`)**. |
| `FILES/E.ST` | The game floppy image (FAT12, 512 B sectors, 3410 sectors, 20 sectors/track, 2 sides, 1 745 920 bytes). Bootable: `AUTO/SIERRA.PRG`. md5 `814e61fc463c594488d4140e88ca61ef`. |
| `FILES/T14GG.FIC` | Packed helper (`T14G16.TOS` inside), part of PP's loader. |
| `KQ6M`, `KQ240R.BMP`, `FKQ273.HST`, `LKQ273.HST` | Title / cover pictures (32000-byte ST screen, BMP, STE hi-colour pictures). |
| `README.TXT`, `READMEO.TXT`, `LOG.TXT` | Brandon's original readme (`READMEO`), PP's notes. PP's tools: Steem Debugger (+ a modded one for stack-frame problems), Devpac 3, HxD. |

### Contents of `E.ST` (extracted to `work/est/` with `tools/fat12.py`)

```
SIERRA.PRG  78845 1987-12-03  Sierra Atari ST AGI interpreter "Version 1.422" (also in AUTO/)
KQ6AGI.EXE  39424            Brandon's DOS interpreter (2.917) + *.OVL, HGC_FONT – unused on ST
LOGDIR PICDIR VIEWDIR SNDDIR  (768 B each, AGI v2 separate dirs)
VOL.0 1046417, VOL.1 447428, WORDS.TOK, OBJECT
```

Resource counts: 164 logics (largest 14 287 B, logic 220), 90 pictures, 255 views (largest
19 082 B, view 209), 84 sounds. `OBJECT`: 16 animated objects max.

## Findings so far (2026-10-18)

1. **The ST interpreter is Sierra's 1987 ST AGI 1.422**, running data made for DOS AGI 2.917.
   Its command table (data segment, file offset `0x1c+0xe77a`, 6-byte entries
   `{long func; byte nargs; byte varmask}`) has **178 action commands** (0–177) and **19 tests**
   (at `0xed00`). Commands 0–173 have the same argument counts as DOS AGI 2.9xx (checked against
   ScummVM `engines/agi/opcodes.cpp`). 174–177 are ST-specific (0,0,4,2 args).
2. **KQ6 uses no command above 169 (`close.window`)**, and all 164 logics decode cleanly. So the
   crashes are **not** an opcode/argument mismatch between DOS and ST interpreters.
3. (Superseded by 6–8.) First guess was memory limits of the ST interpreter. Confirmed for the
   "memory errors" (fixed heap, see 7); the two 4-bomb crashes are a separate, unexplained problem.
   Six of the seven crashes happen in scenes with animated flames, which use a lot of heap.
4. **Debug mode** (in the game data, logic 99, enabled by flag 32): type **`maplemeatloaf`**.
   Then (vocabulary from `WORDS.TOK`):
   - `tp` – new room by number; `pos` – move ego to x,y
   - `show mem` – AGI `show.mem` (heapsize / now / max / max script) ← **key tool for the crashes**
   - `show var` / `set var`, `show flag` / `set flag` / `reset flag`
   - `object` – object status; `show pri` – priority screen; `set pri` / `release pri`
   - `get object` – get item by number; `object room` – where an item is
   - `gimme gimme` – get many items (drops 22, 23, 40, 50–54; game warns not to look at inventory)
   - `coords` – toggle the status-line coordinates; `version`; `trace`; `log`
5. **Hatari runs the game straight from `E.ST`** (no PP loader needed):
   `hatari --machine ste --memsize 1024 --disk-a E.ST --protect-floppy on`. Boots to the disclaimer
   (`work/hatari-boot.png`). Running `SIERRA.PRG` from a GEMDOS hard-disk folder needs `MG.PRG`
   (see 11).
   Headless screenshot trick (Hatari 2.6.1): a `--parse` file with
   `b VBL = 2500 :trace :once :file shot.ini`, where `shot.ini` holds `screenshot <png>` and `quit`;
   use `SDL_VIDEODRIVER=dummy` and `< /dev/null`.
6. **Crash messages (from the user, 2026-10-18)**: the "memory errors" are AGI's own
   **`No memory. Want 6720, have 6016.`** (Minotaur room; "have" varies per playthrough, e.g. 5568,
   6016). Only two crashes are **4 bombs (illegal instruction)**: opening the magic door (room 6) and
   after the Nightmare lands (Lord of Death cutscene). So there are two separate problems.
7. **Cause of "No memory": the ST interpreter's heap is a fixed 64 KB.** Startup (text `0x1e24`)
   does one GEMDOS `Malloc(0x7d00 + 0x6900 + 0x10000)` = screen buffer + priority buffer + **heap
   0x10000**, whatever RAM the machine has. Heap variables (text-relative): end `$13ff4`, base
   `$14084`, top `$14096`, max-top `$140a6`, saved top `$14010`/`$1409e`. Allocator at `0x4e24`
   is a bump allocator with 32-bit pointers; out of memory → `No memory.\nWant %d, have %l`. Free KB
   is written to **v8** (`$f932`, byte, `free >> 10`); no game logic reads v8 (only the debug var
   viewer), so a heap over 255 KB just makes v8 wrap, harmlessly. AGI vars start at data `$f92a`.
   Measured in Hatari at the opening beach: heap 65 042 B, 51 948 B already used → ~13 KB left.
   Brandon's notes (`debugging-steps.txt`) also say these scenes crash "from memory".
8. **Fix 1: bigger heap – confirmed by the user's play (Minotaur room OK, 2026-10-18).** `tools/patchheap.py` changes the constant
   to 128 KB (heap 130 578 B, verified in Hatari). It patches both copies of `SIERRA.PRG` inside the
   image in place: `build/E.ST` (bytes 18000 and 97872 change from 01 to 02). Costs 64 KB more RAM
   at startup; if `Malloc` fails the game shows "not enough memory" and exits (no crash). Larger
   values: `patchheap.py in out 192` etc. (multiples of 64).
9. **Magic door (4 bombs)**: logic 6, `open door` (said 33,43) with flag 253/254 → ego walks to
   74,103 (flag 43) → `print(m34)`, door view 249 `end.of.loop`, **sound 246** (has noise-channel
   data), then `new.room(51)`. Not reproduced yet: with debug `set flag 254`, `tp 6`, `pos 76 104`,
   the typed `open door` got no answer at all (score stayed 0) – maybe the door state needs more
   flags, or input parsed elsewhere. Script: `tools/hatari/door.txt`. Brandon's workaround: debug
   `set var 3` (+2 points), `tp 51`.
10. **Saving under PP's loader (HAG2)** – found 2026-10-18 while the user played in Hatari
   (`hatari -d KQVIST`, `RUNME2.TOS`). HAG2 reads the **whole `E.ST` into RAM** (two `Fopen`s
   read-only, then no GEMDOS calls from the game) and serves every drive from that copy, **C:
   included** (`C:\` shows an empty slot list, then "directory c: is full"). Saves go into the image
   (`SG.1`, `SG.2` in the root, ~4 KB each) and HAG2 writes the image back to `E.ST`; Hatari
   still logs `GEMDOS Fwrite() to a read-only file 'E.ST'` warnings. The image has only **4 KB
   free**, so a third save says the disk is full. Freeing room would mean deleting the DOS-only files
   from the image (`KQ6AGI.EXE` + `*.OVL` + `HGC_FONT`, about 57 KB, room for ~14 saves); this needs a
   FAT12 delete in `tools/fat12.py`.
11. **Running from a hard-disk folder without HAG2 works** (`build/KQ6ST-HD/`, made by
   `tools/mkhd.sh <E.ST> <outdir> [heap_kb]`). The earlier "Please insert disk 1" came from the
   interpreter's disk check (function at text `0xb290`). On drive A/B it opens `A:\SIERRA.PRG`; on
   C: and later it opens `<current dir>\MG.prg` (game ID "MG": this `SIERRA.PRG` is the *Mixed-Up
   Mother Goose* interpreter) and reads 3 bytes at offset 62546. **`MG.PRG` = a copy of
   `SIERRA.PRG`** passes the check. Verified in Hatari (STE, TOS 1.62 SE, 4 MB):
   - Save with the default path `\` → `sg.1` in the game folder (the save path is `Dgetpath` of the
     current folder, buffer at data `$ef7a`; the code at text `0x2870` fills it on the first save).
     File name `%s%s%ssg.%d` with an empty game ID → `sg.N`.
   - Restore lists and loads saves copied out of the image (the user's "beach").
   - `quit` → OK → back to the TOS desktop cleanly. So the quit crash (M4) is HAG2's.
   - What is lost without HAG2: state saves (`/`), numpad `*` exit, its machine settings menu.
   Hatari: `hatari --machine ste -d build/KQ6ST-HD --auto 'C:\SIERRA.PRG'`. Typed path characters
   in the save dialog depend on the TOS keyboard layout (Swedish TOS here: scancode 0x2B ≠ `\`).
12. **99 save slots, scrolling list** (2026-10-18, `tools/patchslots.py`, `tools/slots/`). The slot
   list was one function at text `0x3118`: it reads 12 slots (`cmpi.w #$c`) into a 12 × 34-byte array
   on its stack (`{word num; char desc[32]}`) and opens a window `count + 3` rows high. It is replaced by
   `tools/slots/slots.c`: up to 99 slots (`sg.1`–`sg.99`), at most 12 rows shown, the list scrolls with
   up/down and wraps at both ends; newest save preselected and scrolled into view. Mouse loop: the row
   under the pointer is selected only when the mouse moves (so arrow keys work there too), and holding
   the pointer just above/below the list scrolls every 3 ticks.
   - How it is built: gcc `-mshort` (16-bit int like the interpreter's Alcyon C), `-fcall-used-d2
     -fcall-used-a2`; linked twice (base 0 and 0x01000000) and diffed for relocations (KQ5 `elf2prg`
     trick). The code is appended to the **data segment** at `0x125c0`; every relocated long pointing into
     the BSS moves up by `BSHIFT` (0x2000, 0x3000 since findings 22); `0x3118` becomes `jmp 0x125c0`; the reloc table is rebuilt.
     Interpreter functions/variables are linker symbols in `slots.ld.in` (BSS ones get `+BSHIFT`).
   - **Gotcha: the interpreter returns pointers in d0, gcc's m68k ABI expects a0** – declare such
     functions as returning `long` and cast (`ev_poll`/`ev_wait`).
   - Interpreter helpers used (Alcyon: args pushed right to left, caller pops, result in d0):
     `0x3996 read_slot(n, &ent, &time)`, `0x3916 print_entry(desc, fg, bg)` (normal 0,15; highlight
     15,0), `0xa92a gotorc(row, col)`, `0x8b04 open_win(msg, h, w, 1)` (sets window top `$1408a`, left
     `$1408c`), `0x8d1a close_win`, `0x2514` pop event / `0x2560` wait event (`{type, code}`: type 1 key
     13/27, type 2 direction 1 up 5 down), `0x26ce` mouse buttons → 13/27, `0x6422`/`0x63ac` hide/show
     pointer, `0x644a` mouse poll, `0x247e` tick work, `$fa60` ticks (~20/s), `$140f4` mouse mode.
   - Tested in Hatari (keyboard loop, which is what runs here): restore with 20 saves scrolls and wraps;
     save to slot 41 → `sg.41`; restore preselects and loads it. The mouse loop is untested.
13. **Script patches** (2026-10-18, `tools/agipatch.py`, `tools/logicpatches.py`). `agipatch.replace()`
   swaps a byte range in a logic and fixes every if/goto offset that crosses it; `write_logics()` appends
   the new logic to the last VOL file and repoints LOGDIR (old copy stays, unused). `logicpatches.py
   work/est build/KQ6ST-HD` rebuilds all patches from the original files; `mkhd.sh` runs it.
   AGI save files (`sg.N`): 31-byte description, then blocks of `<u16 LE length><data>`; block 1 =
   game ID (8) + 256 vars + 32 flag bytes (flag n = bit `7 - n%8` of byte `n/8`) + more.
   - **Logic 15 – skip the Cliffs of Logic** (user request). Room 15 is the cliff base, 239/238 the climb,
     235/236/237/100 the etchings, 240 the first etching (sets flag 115, stones appear), 25 the top.
     At `0x0258` `print(m3) new.room(239)` became `print(m3) addn(v3,4) set(143) set(145) set(146)
     set(147) new.room(25)`; the values come from diffing the user's saves "cliffs-1" and "top of
     cliffs" (score 61 → 65, those four flags; flags 143–147 are only read by the cliff rooms). Room 25
     treats arrival from 15 like arrival from the climb. Tested in Hatari from the user's save.
   - **Logic 25 – ego invisible after the skip on the long path** (user, 2026-10-19). Room 25 draws ego
     at `0x0259` only if `v1 != 29` **and** flag 117 (minotaur dead) is clear (the listing's
     `|| a || || b ||` is two one-item or-brackets = AND), otherwise at `0x0272` only for arrivals from
     238/239/233/2. The short-path test had 117 clear, so it worked there; with 117 set and `v1 = 15`
     ego was never drawn, the walk-in `move.obj(0,79,147,1,37)` never set flag 37, input stayed off
     (`distance` to Night Mare read 255). Fix: add `equaln(v1,15)` to the `0x0272` list. Tested from
     `sg.76` (debug `tp 15`, `pos 53 104`): ego walks in, "arrives at the top", `look horse` answers.
   - **Logic 8 – pawn shop trade breaks the game** (found by the user 2026-10-18, game-breaking, also in
     the DOS data). In room 8, `v100` is ego's direction (`get.dir(0,v100)`), which drives the counter
     menu (TALK/TRADE/TAKE MINT/EXIT, `v102`; down = 5 moves to the next icon). Handing over an item
     (TRADE → inventory → `v25` = item) does `new.room(234)` (the conversation screen) without clearing
     `v100`; logic 234 reads `v100` as the talk topic (5 = m13 "What would you take in exhange for the
     magic map?"). So after pressing down to reach TRADE, giving the nightingale did `drop(9)` and then
     the map conversation replaced the trade: the bird was gone and the trade menu never came. Fix: at
     `0x05fb` (first instruction of the item path) insert `assignn(v100,0)`. Tested in Hatari from the
     user's save `sg.44`: TRADE → nightingale → "Would you mind if I traded this in?" → pick 2 → flute.
     Items (OBJECT): 1 coin, 2 ring, 7 mint, 8 map, 9 nightingale, 10 flute, 11 brush, 12 tinder box, 17
     pearl (full list: decode OBJECT, XOR "Avis Durgan").
14. AGI logic `if` in the listings: `if C else goto X` = run the following block when C is true,
   otherwise jump to X.
15. **2 bombs on "give milk" (G8) = dangling sound-list node after a discard** (found and fixed
   2026-10-18 from the user's `sg.48`, room 20). Bus error reading `$4a4a4a4e` in the sound lookup
   (text `0xb84e`, list head `$13fc2`, node `{next; word num; data; 4 channel ptrs}`, 0x1a bytes).
   Chain: playing the flute in the garden does `load.view(40)` then `load.sound(18)` (logic 20 `0x0cb5`),
   and logic 90 later `discard.view(40)`. The heap is a stack: `discard.view` (text `0xd740`) cuts the
   *view* list and sets the heap top back to view 40 (release `0x4ed4`: `heap_top = p`), which frees sound
   18's node too, but the *sound* list still links it. The blit lists (sprite backgrounds, text `0xe158`)
   are rebuilt over that memory each frame, so the next `load.sound` of a new number (give milk →
   `load.sound(62)` 20 cycles later) walks into picture bytes. The save's replay script (block 4 of `sg.N`:
   `load.view(40)`, `load.sound(18)`, `discard.view(40)`) recreates the state on restore.
   **Fix** (`tools/slots/release.c`, linked in by `patchslots.py`): text `0x4ed4` → `jmp heap_release`,
   which sets the top and cuts the sound list at the first node at or above it (nodes are appended in
   address order); if a sound is playing from freed memory (`$1008a` set, a channel pointer in `$f5fc`
   (14-byte stride) at or above `p`), it stops it first via `0xb9ea` (sets the done-flag). All five callers
   of `0x4ed4` are frees (temporary logic after `call`, `discard.pic`, blit free, failed load,
   `discard.view`), so the fix covers the same mistake anywhere in the game. Only the sound list is cut;
   logic and pic lists could in principle dangle the same way (not seen). gcc 15 `-mshort` ICEs (ivopts)
   on a loop over the channel array, so the four checks are written out.
   Debug method that found it: Hatari breakpoint on the corrupted long (`b ($41c58).l = $4a4a4a4a`), on
   heap-top changes (`b ($28722).l ! ($28722).l`), and on the node allocation; abs = text + `0x1268c`
   in the `-hd` setup (basepage `0x1258c`), BSS addresses + `0x2000` (slot patch shift; `0x3000` since findings 22).
   Test script `tools/hatari/milk.txt` (restore slot 1, give milk).

16. **M8 pawn shop: typed verbs instead of the counter menu** (user request, built 2026-10-19,
   `p008` in `logicpatches.py`, replaces the old G7-only `p008`). The menu (TALK/TRADE/TAKE MINT/EXIT
   icons, view 6, objects 5–8, `v102`) is never opened; typed commands at the counter
   (`posn(0,92,113,100,129)`, the menu's own test) **jump into the bodies of the menu branches**, so the
   conversations, trades and points are Brandon's unchanged. Commands: `talk`, `talk owner|merchant|pawn
   owner`, `talk man` (only without the old man, flag 114) → TALK body (`0x0653`, always `new.room(234)`);
   `trade …` / `give|offer|show …` (`9999` = rest of line) → TRADE (`0x0538`, inventory picker, refusals,
   room 234); `take|get mint|candy dish` → TAKE MINT body (`0x06c6`, after its `v102 = 3`). Away from the
   counter: talk/trade/give → m44 "step up to the counter"; talk old man/beggar/man, take mint → m42 "not
   close enough". Changes: the `0x033f` talk handler replaced by these; the menu-opening block
   `0x0396–0x0437` removed (`agipatch.replace(..., drop_inner=True)` drops whole if-blocks); in TRADE the ten
   refusal/cancel `assignn(v102,2)` → 0 and `erase(5..8) discard.view(6)` → `assignn(v100,0)` (G7 fix kept).
   The old menu block `0x0437–0x062f` stays as dead code (`v102` never > 0; logic 101 clears it at room
   init). Why jump instead of setting `v102`: with `v102 > 0` the menu block runs `set.cel` on objects 5–8
   when ego walks down. Vocabulary (no WORDS.TOK change): talk 25, take/get 33, mint 103, candy dish 68,
   trade/exchange 196, give/offer/show 1012, merchant/owner 78, pawn owner 79, man 76, old man 77, beggar
   46. Logic 90's global `take mint` (= eat it) still works outside the shop; room 8 runs first.
   Tested in Hatari (`-hd` scratch copy; ego moved to x 96 in a copy of the save, block 1 offset 7 =
   ego x low byte, the user's saves stand at 89,125): from the user's `sg.99` "pawn-shop": `talk` away →
   m44, `take mint` → mint +1 point, `talk to owner` → "Good day, merchant. Can you tell me what land this
   is?", `give ring to owner` with nothing carried → back to the shop. From `sg.44`: `trade` →
   nightingale → "Would you mind if I traded this in?" → counter item screen → hand-over → back in
   the shop, no menu. **Confirmed by the user's play (2026-10-19)**: talk, coin, flute, brush, bird. Still a menu: logic 234's `get.num` 1–4 for the counter item. Ferryman not done.

17. **M2 menu from the mouse pointer** (user request 2026-10-19, `tools/slots/menu.c`, linked in by
   `patchslots.py`). The interpreter already has a hover-driven mouse menu (text `0x54d0`): the per-cycle
   event poll (`0xca86`) runs it instead of the keyboard menu (`0x532c`) when a menu is requested
   (`$14014`, set by `menu.input` `0x52f8` if flag 14) and `$14082 == 2`. The right button sets
   `$14082 = 2` and posts event `0x101`, which ST games map to the menu; KQ6 maps `0x101` to controller 30
   and Esc/`0x201` to 19 (logic 91), so the right button never opened it. In play the pointer is clamped
   to `y >= $ebf6` (play-area top = 8, set in `0x64f4`), so it cannot enter the status line; "reaching the
   status line" = sitting on that limit. Fix: the only call of the poll (`0x4c34 jsr $ca86`) now calls
   `menu_poll`, which, when the pointer y (`$fe3c`) is at `$ebf6`, sets `$14082 = 2` and calls
   `menu.input`, then the poll. Conditions copied from the right-button path: no menu open (`$140f2`), no
   request pending, not text mode (`$14018`, unless `$f46c`), mouse limits on (`$f22e == 0`); plus KQ6's own
   guard flag 33 (logic 0 only calls `menu.input` when it is clear; set during the title/intro). The pointer
   must go below the line before it opens again (else it would reopen right after a pick).
   Menu behaviour (interpreter's own): title row is y 0–7, hovering a title drops it down, hovering an
   item highlights it, left click or Return picks, Esc closes, a click on nothing closes.
   Tested in Hatari (pointer set through the debugger, `w $<text+0xfe3c> 0 8`; text = `0x1268c` in `-hd`):
   bar opens at the top, File/Action drop down on hover, Esc closes and it stays closed until the pointer
   leaves, Action → Inventory with Return shows the inventory. A real single left click on an item is
   not tested (hconsole only has `doubleclick`; its second click closed the inventory again).
   KQ4 ST (SCI, `KQ4-sci-atari-st/`) does this in the SCI kernel, not in the scripts; its help text says
   "Click at the top of the screen or press ESC to use the menus".
   **hconsole gotcha**: `keypress 1` sends the character `1`; Esc is `keypress 01`.

18. **Esc skips the disclaimers / title texts** (user request 2026-10-19, `p097`, `p255` in
   `logicpatches.py`). Both rooms skip on `have.key()`, but Esc is `set.key(27,0,19)` (logic 91,
   controller 19 = menu) and in room 97 `have.key()` never saw it (unpatched game: Esc does nothing).
   Room 97: `if || have.key() controller(19) ||` → `new.room(255)`. Room 255: a block inserted before
   `0x01b0`: Esc while the choice is not shown yet (flag 41 clear or a text timer v106–v109 running) sets
   v104 = 1 (Esc pending), zeroes the disclaimer timers and sets 37/38; the music starts as usual, the
   30-cycle wait (v103) is cut to 1, and once the picture is drawn (41) the timers are zeroed and v109 = 1,
   so `0x01f9` shows Opening / Play / Restore in the same cycle. On the choice itself any key picks the
   highlighted entry, and Esc picked "Opening" (the user, too), so that `have.key()` test (`0x02ec`)
   now also needs `! controller(19) ! equaln(v19,27)`. **Why both**: `have.key` (text `0x1b0c`) returns
   true if v19 (`$f93d`) is set, else pulls events itself (`0x2612`: key events give their code, others
   are dropped) and stores the key in v19. So Esc reaches the logic either as controller 19 (the
   per-cycle poll mapped it) or as raw 27 in v19 (have.key got it first); the right mouse button is
   also 27 (`0x26ce`). The Esc-skip block therefore starts with `if have.key() {}` (pulls a pending key
   into v19; any other key stays there for `0x02ec`) and tests `|| controller(19) equaln(v19,27) ||`.
   **Play is preselected** (user request): where the choice is first drawn, `position(0,13,162)
   set(43)` became `position(0,63,162) set(44)` (43/44/45 = Opening/Play/Restore, x 13/63/120). Tested in Hatari (`-hd` scratch copy): Esc, Esc → choice, Esc
   ignored there, right + Return → Play → beach. The intro movie itself already skips with Esc
   (logics 222–254, `controller(19)` → room 2).
   **Mouse on the choice** (user request 2026-10-20): the interpreter now writes the pointer position into the
   `get.mse.posn` vars every cycle also while the pointer is shown (`menu_poll`, findings 21; logic 0 names
   v240/v241 every cycle, flag 19 is still "clicked this cycle"). Logic 255, inserted before `get.dir(0,v110)`
   (original `0x0224`, where the "choice drawn" block jumps, so it runs every cycle): while flag 43/44/45 is set,
   pointer on a word (v241 150–163; v240 10–45 Opening, 60–90 Play, 116–154 Restore; text row 20, columns 4/18/31
   = x col·4) and moved since the last cycle (v246/v247, free in this room) → that entry is marked (flags, marker
   ego at x 13/63/120, words redrawn, as the arrow-key blocks do); a click (flag 19) marks it too, cancels the
   click walk of the marker (`stop.motion`/`start.motion`) and sets v19 = 13, so the `have.key()` test starts it.
   Tested in Hatari: hover Restore / Opening moves the frame, click Play → beach, click Restore → restore dialog,
   click Opening → opening movie; arrow keys alone and with the pointer resting on a word still move the frame.

19. **M1 pointer erases text** (2026-10-19, `tools/slots/pointer.c`, linked in by `patchslots.py`). Text
   is drawn by TOS's VT52 console (GEMDOS `Cconws`, `0x5a66` fn 9, with `ESC Y`/`ESC b`/`ESC c` strings
   built at `0xa9c0`/`0xaa9a`), straight onto the screen, never into the interpreter's buffers. The pointer
   routines are a driver table: mono at `0x4162–0x41b0` (`0xd8xx–0xdexx`, 80-byte lines), colour at
   `0x41b6–0x4204` (`0xdfxx–0xe5xx`), copied over the mono table at startup (`0x1ea4`) unless `Getrez` = 2.
   Colour: `0xe48e` save-under (to `$139d2`, 256 B, sets bit 1 of `$ebfe`), `0xe4c2` hide, `0xe510` draw (sets
   bit 0), `0xe572` area (a0 screen, a1 back buffer, 8 or 16 bytes × up to 16 rows). Hide copies from the
   **back buffer `$13f04`** (the picture area in screen format, rebuilt from the 160×168 pixel buffer
   `$13f08`) unless a save-under was made, which `0x63ac` only does in text mode (`$14018`) or a menu
   (`$140f2`). So in play the pointer's erase painted the picture back over any `display` text it had
   covered. The back-buffer erase exists because sprite blits (`0xdf40`, from `0xe224`, called per object
   in the update loop `0x6fb2`) write to the screen without hiding the pointer; the loop then calls `0x63ac`,
   which redraws the pointer over the new pixels. A plain save-under would restore stale sprite pixels.
   **Fix**: the colour vectors save/hide/show (`0x41d4`/`0x41da`/`0x41e0`) go to `ptr_save`/`ptr_hide`/
   `ptr_show`. Show always saves the area first (if not drawn), draws via `0xe510`, then keeps a copy of what
   it drew. Hide restores a byte from the saved copy only if the screen still holds the drawn byte; a changed
   byte (sprite, text, dissolve) is new content and stays. A redraw while drawn (the sprite loop) first copies
   changed bytes into the saved copy. Saved copy = the interpreter's `$139d2` buffer (the blob had no room
   for a second 256-byte array under `BSHIFT`). The mono driver is unchanged (KQ6 runs in colour).
   **gcc 15 `-mshort` gotchas**: besides the ivopts ICE, pointer + short offset and scev-cprop's
   pointer-after-loop are computed in 16 bits (`and.l #65535` on an address → 4 bombs in ROM); `pointer.c`
   steps rows with `long` integers and turns off `ivopts` and `tree-scev-cprop`. Check the `-S` output for
   `and.l #65535` on pointers.
   Tested in Hatari (`-hd` scratch copy; pointer moved via `w $224c6 x y`): title buttons – old build loses
   all three labels, new keeps them; spell book (debug `get object` 5, `open spell book`) – old has holes
   along the pointer path, new intact; Alexander walking under a still pointer then the pointer moving – no
   ghost (same picture as the old build); pointer menu open/close – clean.
   Probably the same cause: the "old text box under the pointer" patches (`bug-images/mouse-text-glitch*.png`,
   all taken 2026-10-19 before this fix) are exactly the pointer's save-under area (2 × 16-pixel groups ×
   16 rows) restored stale – text saved while an earlier box was open, put back after it closed. With
   `pointer.c` the erase only restores bytes that still hold the drawn pointer, so a closed box (repainted
   from the picture) can no longer come back. Not reproduced on purpose; awaiting the user.

20. **M5 black text on blue** (2026-10-19, `p012`, `p014`, `p072` in `logicpatches.py`). Brandon uses
   `set.text.attribute(0,1)` before `display` text that should look like plain black print. DOS AGI in graphics
   mode draws any non-zero text background as white (so DOS shows black on white); the ST interpreter passes the
   colour straight to the VT52 console (`ESC c`), and pen n is the same colour as picture colour n, so 1 = blue.
   Fix: give each text the picture colour under it – logic 12 (ferryman list on the white `print.at` window) and
   72 (vizier's letter on white paper) `(0,15)`; logic 14 (magic map) `(0,14)` for the island names on the
   parchment and `(0,6)` before the numbers on the brown islands (incl. "5" Isle of the Mists, flag 117, and the
   redrawn "3" after the "only four islands" message). DOS output is unchanged (still white). Left alone: logic
   233 (spell book) uses `(0,1)` for the *selected* button among white-on-black ones, a highlight that still
   reads as one on the ST. Tested in Hatari from `sg.99` (debug `tp 14`, `set flag 117`, `tp 12`, `tp 72`).
   Script `type maplemeatloaf` needs a `sleep 1` before its Return or the word gets lost.

21. **Mouse-driven game cursors** (user request 2026-10-20; `cursor()`, `cursor_room`, `p111`, `p000`, `p088` in
   `logicpatches.py`, `menu_poll` in `slots/menu.c`). Rooms 210 (ebony box), 211 (trunk of letters), 212 (beach
   box) and the sword fight 111 make ego a cursor moved by the arrow keys; `have.key()` runs `posn()` tests on it.
   Cursors: view 154 = 11×12 arrow pointing left, tip column 0 row 5 (ego = tip x, tip y + 6); view 194 cel 1 =
   18×33 sword, tip column 16 row 2 (ego = tip x − 16, tip y + 30). Fight steps: 1 grab the sword (flag 41,
   `block(98,78,109,125)`, test `0x013e`, before the vizier arrives), 2 strike (`v103 == 1`, `block(79,114,102,158)`,
   `0x01c7`, 100 cycles), 3 WHAP (flag 139, `block(19,106,39,165)`, `0x03f7`); the cursor is clamped strictly inside
   the block (ego moved outside a block is trapped there, keys and all).
   **ST interpreter commands that differ from DOS AGI**: 174 `hide.mouse` (`0x67bc`, clears bit 6 of `$ebfe` and
   `$f929`), 175 `show.mouse` (`0x67dc`), 177 `get.mse.posn(va,vb)` (`0x67fc`, 2 vars: clears flag 19, stores the var
   numbers at `$13f00`/`$13f02` BSS); `agipatch.py`/`agidis.py` override `ACT[174,175,177]`. A single left click in
   play (`0x65d6`, from the tick `0x247e` when `$f22e == 0`), if flag 19 is clear, stores x/4 and y − `$ebf6` in
   those vars and sets 19, and always walks ego to the click (`0x6236`, motion 4, if player control `$fa4a`).
   Before any `get.mse.posn` the var numbers are 0 (= v0/v1!). In text mode a click is posted as key event `0x201`
   instead (`0x6612`); the per-cycle poll applies the `set.key` table first (`0x25c6`) and logic 91's
   `set.key(1,2,19)` turns it into controller 19 (menu/Esc), so the portrait dialogues (`print.at` + flag 15, the
   game keeps running, logic 88 closes them on `have.key()`) ignored clicks. Plain `print()` boxes wait in `0x2560`,
   which skips that table, so they always closed on a click.
   **Changes**: interpreter: while the pointer is hidden, the var numbers are set and `$f22e == 0`, `menu_poll`
   writes pointer x/4, y − top into those vars every cycle (no menu from the pointer meanwhile). Logic 0: re-arm
   `get.mse.posn(v240,v241)` at the end of every cycle (so flag 19 = clicked this cycle, for the room and logic 88;
   clicks never write v0), `show.mouse` on room change / restore (flags 5, 12). Logic 88: `|| have.key()
   controller(19) isset(19) ||`. Cursor rooms: arm + `hide.mouse` where a cursor appears (`show.mouse` at each
   `erase(0)` in 111), and at the end of each cycle: if the mouse moved (v240/v241 ≠ last v246/v247) or clicked,
   tip to the mouse (clamped), `stop.motion`/`start.motion` cancel the click walk; a click sets v242 = 1 and the
   next cycle (cursor drawn there) the test runs as `|| have.key() equaln(v242,1) ||`. The keys still move the
   cursor (it only jumps when the mouse moves); v248 = 2 cycles of settling after a hide so the cursor does not
   jump to wherever the mouse was. 111: v243 = step whose cursor is drawn (set at each draw, 0 at each `erase(0)`),
   needed because after a time-out flag 41 / v103 stay set with the cursor erased. v240–v248 are used by no other
   logic. Mouse detection is not needed (no mouse, no clicks; `$f22e != 0` input mode skips the tracking).
   **Tested in Hatari** (`-hd` scratch copy). Mouse simulated by writing the pointer (`w $224c6 x_hi x_lo y_hi y_lo`,
   x 0–639, y screen rows) and a click by the click counter + short timer (`w $21b6f 2 1`; writing `$f4e0` does not
   work); `tools/hatari/click-coin.ini`. 212 by the real beach path (pos 75 115, move board, open box): pointer
   hidden, arrow stays until the mouse moves, then follows (clamped), click on the coin → coin, score +1, back on
   the beach with the pointer shown. 210 → "ZEBU", 211 → letter, misses → "nothing of interest". 111 (debug `set
   flag 199` = Cassima has the dagger, `tp 111`) played with clicks only: sword grabbed, every dialogue closed by a
   click, strike, WHAP, points; pointer hidden during the steps. Keyboard in 211: the arrow moves as before (the
   cursor block stays idle). `start-debug.txt` updated for the preselected Play (Esc, Esc, Return).

22. **M9 animations draw over message boxes** (2026-10-20, `tools/slots/blit.c`, linked in by `patchslots.py`).
   Brandon sets flag 15 before many prints (room 226 gnomes: set at `0x0364` and later, so even the plain
   `print(m8)`/`print(m19)` after it do not wait; room 49 Lord of the Dead `0x0135`), so the window stays up while
   the game cycles. The sprite update (colour vector `0x41c8` → `0xe224`) copies the union of each object's old and
   new rectangle from the picture buffer (`$13f04`, screen format, 160 bytes/row) to the screen with `0xdf40`
   (registers: d0 bottom row, d2 rows, d3 game x rounded down to 8 = byte offset, d4 width in bytes; copies
   upwards), straight over the box. DOS/ScummVM work the same way (`render_Block` has no window check), so
   whether DOS shows it too is not known.
   Window state (set by `open_win` `0x8b04`): `$14018` = 1 while open (cleared by `close_win` `0x8d1a` and
   `0x7d62`), `$14000` = x << 8 | bottom row, `$14300` = height << 8 | width (game pixels / play rows, border
   included; x = column·4 − 5, width = columns·4 + 10). The box is drawn by the fill `0xdf88` (`0xe44`), closed by
   copying the area back from the picture buffer with `0xdf40` (`0xf4a`). In screen modes other than
   `$ed7e == 1` (low res) `0xe44`/`0xf4a` convert the coordinates; the fix only clips in mode 1.
   **Fix**: the `jsr $df40` at `0xe2a4` goes to `sprite_blit` (asm stub: window closed or not low res → the
   original `0xdf40`; else `blit_clip` with all registers saved), which copies row by row and leaves out the
   box: 16-pixel groups fully inside are skipped, partly covered ones merged per plane word with a mask. The picture
   buffer still gets the sprites, so the close (or the next window, which closes the old one first) shows them
   as they are. `close_win` calls `0xdf40` through `0x4192`, not through `0xe2a4`, so it is not clipped.
   The blob grew to 8380 bytes, over `BSHIFT` 0x2000; **`BSHIFT` is now 0x3000**, so BSS addresses quoted in
   older notes (e.g. findings 15, the heap counters in the harness section) are 0x1000 higher in this build;
   data-segment addresses (`$fe3a`, pointer `$224c6`, click `$21b6f`) are unchanged.
   Tested in Hatari (`-hd` scratch copy, user's `sg.86` "long, lord of death", debug `set flag 15`, `look`):
   old build – flames drawn over the box text; new build – box intact, flames outside it still animate; the
   next message (walk up → "approaches the throne") redraws the old box area cleanly. The gnome room was not
   replayed (same code path).

23. **Click to interact, pilot = beach** (user request 2026-10-20, `tools/slots/act.c` + `hotspots.h`, linked in by
   `patchslots.py`; plan and per-room outlook in `CLICK-TO-INTERACT.md`). KQ6 sets flag 8 (logic 91), which turns on the
   interpreter's double-click detection (window max(v15, 15) ticks): the IKBD code (`0x6862`) counts left clicks in
   `$f4e4` → `$f4e0` and right clicks in `$f4e7` → `$f4e1` once the window ends, so the first click of a double click
   never walks. The tick (`0x65d6`) then handles them: left double click in play posts a stop (`ev_post(2, 0)`, text
   `0x24ac`, call at `0x66ea`; in text mode `0x401`); right click posts `0x101` (call at `0x673c`, controller 30,
   unused) – right clicks are left pending while a window is open and fire once it closes. Both calls now go to
   `act_dbl` / `act_right`, which post as before (right click also posts the stop) and, in play (no window `$14018`,
   `$ff0a != 1`, no menu `$140f2`, no save/restore dialog `$140b8`), note the live pointer (`$fe3a`/4, `$fe3c` −
   `$ebf6`). `menu_poll` calls `act_poll` after the per-cycle event poll (flags 2/4 were reset at `0x4c1a`), which,
   if input is accepted (`$fa6a`, set by `accept.input` `0xcd9a`) and nothing was typed that cycle (flag 2), finds the
   hotspot and runs the interpreter's own parser `0x7022(char *)` on its command: it fills the word groups `$1401a`
   (count `$14016`) and word strings `$1405a` that `said()` (test 14, `0x1b3a`) and `%w` use, sets v9 and flag 2.
   (`0x710e` is AGI `parse`; Return in the input line calls `0x7022` at `0xcbc6`.)
   Hotspots: `OBJ(room, obj, view, cmd)` hit-tests the object's cel pixels (±1 pixel) while drawn with that view;
   `AREA(room, x1, y1, x2, y2, cmd)` a play-area box; first hit in the list wins; a click on ego's pixels does nothing.
   Object table (`*$142bc`, 0x48 bytes each, last ≤ `*$14282`): +6 x, +8 y (bottom), +0xa view, +0x10 loop,
   +0x1c cel pointer (raw AGI cel: width, height, mirror/loop/transparent, RLE rows), +0x2c width, +0x2e height,
   +0x3e flags (bit 0 drawn). Beach: ring = obj 2 view 201 (5×5 at 52,117), plank = obj 1 view 1 (30×34 box, diagonal
   plank inside), box = obj 6 view 237 (after the plank is moved).
   **gcc `-mshort` miscompile**: `while (*c++) ;` was compiled as `tst.b (a0,d1.l)` with d1 = `0x0000ffff` (meant −1),
   reading 64 KB off. Write such loops as `while (*c) c++; c++;` and check the asm (`-S`) for `.l)` indexes.
   `SLOTS_MAP=<file> patchslots.py …` writes the blob's symbol map (runtime address = `0x1268c` + value in `-hd` runs).
   Tested in Hatari (`-hd` scratch copy; pointer `w $224c6 …`, double click `w $21b6f 2 2`, right click
   `w $21b72 2 1`, debug `pos` then X and Y at two prompts): ring from afar → "not close enough to pick up…"; ocean →
   ocean text; plank → pushed aside (+1), box appears; box → "Alexander opens the box."; ring after `pos 60 124` →
   picked up (+1), the same spot then gives "look sand"; right click while walking → text and Alexander stays put;
   right click on Alexander or the sky → nothing.
   **All rooms (2026-10-20, after the user confirmed the beach)**: 347 hotspots in 51 rooms, one file per island in
   `tools/slots/hotspots/` (crown, wonder, beast, sacred, mists, dead, castle; written by five parallel sub-agents
   from the room template, each entry checked). Commands are `const char *` now; the blob is ~18 KB, so **`BSHIFT` is
   0x6000** (BSS addresses in this build are another 0x3000 higher than in findings 22; data-segment addresses are
   unchanged; the user's `sg.98` restores fine). Left out on purpose: the catacomb maze (31/102: picture per cell),
   rooms 32–35, 54/55 (wedding/finale), cursor rooms 210–212, and actions that kill or spoil (room 25 berries, room
   52 north door → "look").
   New stand mode **`DIST(obj, d)`** for logics that test `distance(0, obj, v) < d` (beggar, Jollo, ghosts, Styx
   gate, Rotten Tomato): the interpreter's distance (`0x1400`) is |dy| + |d(x + width/2)|, 255 if either object is not
   drawn; ego aims 3 pixels below the object's bottom centre (two objects on one baseline block each other) and is
   re-aimed every cycle, as the object may walk.
   Tools: `agipic.py` (AGI picture renderer, port of ScummVM `picture.cpp`; visual + priority/control screen with a
   grid → `work/pic/`), `roomsaid.py` (per room: objects with views/positions, every said() with words, posn box and
   first message → `work/roomsaid.txt`), `checkhotspots.py` (words known, logic 0 does not take the command, the
   room's first matching said() accepts it, NEAR = a posn box of that branch incl. one on the said line, DIST = its
   distance test, OBJ view set in the logic; reads the PATCHED listings: `agidis.py build/KQ6ST-HD
   work/logic-patched`), `hotspotmap.py` (boxes over each room's picture → `work/hotspots/roomNNN.png`).
   Tested in Hatari: beach unchanged; room 3 castle/tree; room 20 Rotten Tomato by DIST (walks over, picked up, +1);
   room 7 pawn door: walks, gets stuck at the stalls (stand box fenced by control lines, gap at the side) → "not close
   enough". Harness gotchas: `tp` from some rooms leaves ego undrawn (then nothing walks); wandering NPCs (the
   peddler) and their `print.at` boxes eat clicks – test on static objects.
   **Walk first (level 1, 2026-10-20, after the user's test: "would not move to pick up the ring")**: a hotspot can
   carry the room logic's stand-here box (`NEAR(x1,y1,x2,y2)` = its `posn(0, …)` test; `ANY` = no walk). If ego is
   outside it, under player control (`$fa4a`) and drawn, `act_poll` starts the click walk itself (ego +0x38 = 4,
   target +0x40/+0x42 = nearest point of the box, one pixel inside, step +0x44 = +0x30; see `0x6236`; arrival
   `0x616e` clears the motion and sets player control) and types the command once ego is in the box (then stops him:
   +0x38, +0x36, v6 = 0). No movement for 10 cycles (an obstacle) → typed anyway. Dropped when the motion or target
   changes (another click, arrow key), on a room change or when ego is erased; a message window pauses it.
   Tested in Hatari: ring from the start position → walks over and picks it up; box from the far right → walks over,
   "Alexander opens the box."; ring double click, then a single click elsewhere while walking → goes to the click,
   no message, ring stays.

24. **Clickable number menus: ferryman + pawn shop** (user request 2026-10-20; `tools/slots/getnum.c`, `p008`).
   The ferryman's menu (logic 12) is a blank `print.at` box (flag 15 → `$f46c` = 1), `display()`ed lines "1. The
   kingdom" … and `get.num(22, v105)` (command 118, `0x1720`: prompt, then the line input `0xc22c` for 4 chars,
   then atoi). With `$f46c` set the pointer is not drawn at all (`0x6408`), and a left click in text mode is event
   `0x201`, which the key fetch `0x265a` → `0x2612` → `0x26ce` turns into Return: a click entered an empty answer.
   **Interpreter**: command 103 `display` (`0x9144`; command table entry `0xe77a + 103·6`) → `disp_hook`, which
   remembers per text row a line starting with "N." (column, length; message text from `0x90ca(n)`); get.num's
   call of the line input (`0x1784`) → `getnum_line`: shows the pointer (clears `$f46c` and calls `0x63ac`, unless
   the logic hid it), runs the line input, hides it and restores `$f46c`, forgets the lines; the line input's key
   fetch (`0xc29e`) → `line_key`: outside get.num the original; inside, it redraws the pointer (`0x644a`), and a
   left click (event 1/`0x201`) on a remembered line (row = y/8, column = x/16, ±1 column) returns 4 backspaces, the
   number and Return; other clicks are ignored; keys as before (`0x26ce` mapping). get.string / the input line use
   the same line input and are unaffected (flag `active`).
   **gcc `-mshort` again**: `s[k]` with a 16-bit `k` was computed as a 16-bit address, and `while (*s++)` got the
   `0x0000ffff` index (findings 23) – walk strings with `s++` only and check the `-S` output.
   **Pawn shop** (`p008`, logic 8): new command `use counter` (use 22, counter 66; free in this room) at the
   counter box opens the same kind of menu: `set(15) print.at(box, 2, 10, 21)`, "Pawn shop" + "1. Talk" … "4.
   Leave" (rows 3–11, black on white), `get.num("Please Select: ", v249)` (v249 used by no logic), `close.window`;
   v249 = 1/2/3 → `assignn(v249,0)` + goto the TALK / TRADE / MINT bodies (the same targets the typed verbs use).
   Away from the counter it says m44 "step up to the counter". New messages m73–m79 via `agipatch.add_messages`
   (`messages`/`encode_messages` round-trip all 164 logics byte for byte); patch functions may now take
   `(code, msgs)` and return both. The owner hotspot (room 8, object 2) types `use counter`, so a click walks
   Alexander to the counter and opens the menu.
   Tested in Hatari: ferryman (`tp 12`): pointer drawn over the list, click "2. The ferry" → "Tell me more about the
   ferry.", typing 3 → "His job". Pawn shop (user's `sg.99`): right click on the owner → menu with pointer, click "3.
   Take mint" → mint, +1; menu again, typed 4 → closes; typed `talk` → "Good day, merchant…". Talk and Trade by
   click not tried (same jump targets as the typed verbs, checked in the listing).

25. **Pointer in the inventory picker** (user request 2026-10-20, `tools/slots/inv.c`). `status()` (command 124,
   `0xba12` → `0xba5e`) with flag 13 lets the player pick an item (`v25` = item, via `$f943`). The screen is a
   list of 10-byte entries `{W item; char *name; W row; W col}` built on the stack (`-0x1f8(a6)`, last entry at
   `-0x200(a6)`, highlighted one in a5). The interpreter has a mouse mode (`0xbb70`, `$140f4`): hover selects, a
   click picks, but it is only switched on by flag 18 (`0x678c`, never set by KQ6; `$140f4` is cleared every
   cycle by `0x14e8`), and in it the arrow keys do nothing and leaving a name deselects. So the keyboard loop
   (`0xbcb8`: `ev_wait`, Return picks a5, Esc = none, direction events → `0xbe82`) stays, and its `jsr $2560`
   goes to `inv_stub` (asm: passes list, last, a5 to `inv_wait`, puts `inv_cur` back in a5): pointer shown,
   when the mouse moves onto a name (row = y/8, col = x/16 within the name, length by the interpreter's
   `0x205e`) that entry is highlighted with `0xbf00(old, new)`, a left click (1/`0x201`) on a name highlights
   and returns a Return event; the pointer is hidden on Return/Esc; other events go to the loop unchanged.
   `0xba66 jsr $678c` → `inv_start` (original + "fresh": the first call takes the current pointer position as
   "not moved", so a resting pointer does not override the keyboard preselection). No mouse (`$f22e`) → the
   original wait. gcc notes: the ivopts pragma (as in `pointer.c`) was needed again, and a hand-written
   length loop became a call to `strlen` (not linked) – use `0x205e`.
   Tested in Hatari (user's `sg.99`, debug `get object` 1, 2, 6, 9): owner click → menu → click "2. Trade" →
   picker with pointer; hover "Mechanical Nightingale" → highlighted; left arrow → "Rabbit Foot"; click "Royal
   Insignia Ring" → "Can you tell me, merchant, what the value of this ring might be?". (Debug `gimme gimme`
   gives everything and warns that the inventory screen then crashes – don't use it for tests.)

26. **Pathfinding for click to interact** (2026-10-20, `tools/slots/pf.c`, called from `act.c`). The walk rules are
   the interpreter's own: each step, `0x1076` checks ego's baseline (y, x … x + width − 1) in the priority screen
   `*$13f08` (160 bytes a row, priority in the high nibble, rows via `$13ad2`): 0 = barrier fails; 1 fails unless
   ignore.blocks (object flag 0x0002); 3 = water (all water: fails with object.on.land 0x0800; not all: fails with
   object.on.water 0x0100); fixed priority (0x0004) 15 skips the test. Horizon `$fa4e` (y ≤ horizon pushed down unless
   ignore.horizon 0x0008), block `$fa50` on / `$fa52` x1 / `$fa54` y1 / `$fa56` x2 / `$fa58` y2 (strictly inside test
   `0x93a`). move.obj direction (`0x61b6`/`0x6204`): per axis −step if Δ ≤ −step, +step if Δ ≥ step, else 0 — with
   step 1 it ends exactly on the target.
   `pf.c`: a walkable map of all ego positions (sliding window per row, screen edges left out with a pixel to spare),
   then a breadth-first search *backwards* from the stand spot (NEAR: the box's rim; DIST: every position closer than
   d, not inside the object) until it reaches ego; each searched cell stores its next step. Ego walks it in pieces,
   each the farthest path point (≤ 120 steps) that move.obj reaches over walkable cells; next piece when he is on the
   piece's end. Only used when the straight line fails (`pf_line`), so most walks are as before. Memory: grid 26880
   bytes + queue 8 KB **after the old BSS end** (`0x14380`, PRG header BSS length + `PFSIZE` in `patchslots.py`; the
   startup Mshrinks to text + data + BSS from the basepage, so this is free). Search seen at 6–8 k cells in room 7, no
   noticeable pause in Hatari. Host test: `pf.c` compiles natively; harness in the session scratchpad rendered room
   7's priority screen with `agipic.render` and printed paths (worth recreating as a tool if more rooms need checks).
   **gcc 15 `-mshort`**: internal compiler error in ivopts on `pf_plan`, and a fill loop became a `memset` call:
   `#pragma GCC optimize ("no-ivopts", "no-tree-loop-distribute-patterns")` at the top of `pf.c`.
   **The game taking over** (found while testing room 7): the stall gap is a signal line (priority 2); touching it
   makes logic 7 `prevent.input` and `move.obj` ego to 109,129 → 112,120 → 118,113 (v162 states), and logic 88's
   portrait talk does `prevent.input` + `stop.motion(0)` + a window. Both used to look like "the player took over" and
   dropped the walk. Now, while player control (`$fa4a`) or input (`$fa6a`) is off, the walk waits; when both are
   back, it types the command if ego is in the spot, else sets off again from where he is (at most 5 times). A click
   or arrow key by the player (which needs control) still cancels it.
   Tested in Hatari (`-hd` scratch copy, debug `tp 7`, `pos`): from 140,150 and from 40,120 (beggar talk in between,
   closed with Esc) → around the stalls, scripted entry, door opens, pawn shop "Good day!"; beach ring from the start
   position unchanged (+1). Not yet: DIST targets behind barriers, rooms with water or blocks.

27. **Pathfinding for plain clicks** (user request 2026-10-20, `click_walk`/`click_poll` in `act.c`, `pf_click` in
   `pf.c`). The click walk call `jsr 0x6236` at text `0x668c` (in the tick, after `$f4e0` = settled single-click count;
   0x6236 sets motion 4 to `$ec00`/4 − width/2, `$ec02` − `$ebf6`, only with player control) goes to `click_walk`,
   which calls the original and notes ego's target; `act_poll` then calls `pf_click`. A click above the horizon is an
   exit at the top, within 4 pixels of the left/right/bottom edge an exit there; otherwise, if the straight line is
   clear, the interpreter's walk stays. `pf_click` searches *forward* from ego (BFS) to the first goal: the target, or
   for an exit the walkable positions on the edge row/column within 16 of the click; with no goal reachable, the
   reachable position nearest the click (still an exit if that lies on the edge row – e.g. clicking the sky over the
   beach). The path is then reversed into pf_plan's next-step format, so `pf_next` pieces are reused; after the last
   piece ego is sent one pixel past the edge (top: y = horizon; bottom: y = 168; left: x = −1; right: x + width > 160),
   which sets v2 and the room logic changes room. The game taking control pauses and resumes it (≤ 5 times) like
   hotspot walks; another click, an arrow key or a double/right click cancels it; 10 cycles without movement stops ego.
   Room 3's castle path is a 1-row `posn(0,124,78,134,78)` trigger: a click on the castle (above the horizon) finds no
   reachable top edge there, so he goes to the nearest point (132,78) and the room's own walk takes him to room 4 (~20 s).
   Host test: `pf.c` + a stub `main` with the priority screen from `agipic.render` (`<< 4`) prints the pieces and
   checks every step (recreate from this description if needed). Hatari: single click = `w $21b6f 2 1` after writing
   the pointer to `$224c6`/`$224c8` (x × 4, y + 8). The blob is 24.9 KB, so **`BSHIFT` is 0x7000** (BSS addresses in
   this build are 0x1000 higher than in findings 23–26).

## Logic numbers found so far

| Logic | What |
|---|---|
| 0 | main loop; checks `maplemeatloaf` (word 175), sets flag 32, calls logic 99 each cycle |
| 89 | descriptions for "look at" items (book, skull, lamp contents …) |
| 99 | debug commands |
| 2 | Isle of the Crown beach |
| 6 | castle wall with the magic (painted) door; `open door` → room 51 |
| 7, 8 | village street / pawn shop room (counter menu, v102); 234 = pawn shop conversation screen |
| 15 | Isle of the Sacred Mountain, base of the Cliffs of Logic; 240 first etching; 239/238 climb; 235–237, 100 etchings |
| 25 | top of the cliffs (Night Mare, nightshade); Brandon: also "outside the lair" for the debug skip |
| 38 | mentions the Minotaur |
| 30, 31, 35, 36, 37, 78, 102, 243 | mention Charon / Styx (to be sorted out) |
| 46, 47, 48, 89, 94, 233 | mention the druids |

## The bug list, sorted by kind of fix

| # | Bug | Probable cause | Fix route | Feasibility |
|---|---|---|---|---|
| G1–G3, G6 | "No memory": Minotaur, druids, Charon, Jollo/lamp | fixed 64 KB AGI heap (not the machine's RAM) | **heap 128 KB** (in `build/KQ6ST-HD`) | **Confirmed** (full playthrough 2026-10-19) |
| G4 | 4 bombs after the Nightmare lands | unknown (not the heap) | reproduce, read the crash PC from TOS's save area `$380` | Not seen in full Hatari playthrough |
| G5 | 4 bombs opening the magic door (room 6) | unknown; suspect sound 246 or the room change | same | Not seen in full Hatari playthrough |
| M1 | Title buttons / spell book lose letters on mouse hover; old box text left under the pointer | pointer erased from the picture back buffer, text is only on screen | interpreter: save-under pointer (`slots/pointer.c`, findings 19) | **Done**, tested in Hatari |
| M2 | Menu only via Esc (KQ3/KQ4 ST open it at the top) | AGI 1.422 behaviour; Brandon did not know the ST has a pointer | interpreter: pointer at the status line → mouse menu (`slots/menu.c`, findings 17) | **Done**, tested in Hatari |
| M3 | Saving with HAG2 state save crashes; AGI saves fill the image (4 KB free) | PP's loader | **hard-disk folder `build/KQ6ST-HD`** (saves in the folder), or free room in the image | HD folder done |
| M4 | Quit crashes instead of returning to TOS | HAG2 (quit is clean without it) | hard-disk folder | Done (HD folder) |
| M5 | Some text on blue background | `set.text.attribute(0,1)`: DOS shows white, ST shows colour 1 | logics 12, 14, 72: background = colour under the text (findings 20) | **Done**, tested in Hatari |
| M6 | Bookshop sign hard to read | picture data | edit PIC resource | Easy with WinAGI |
| M7 | Bookdealer talk is slow | logic | look at the logic | Unknown |
| M8 | Pawnshop / ferryman menus → typed commands | logic design | logic 8: typed talk/trade/give/take mint jump into the menu branches (findings 16) | **Pawn shop done, confirmed by the user**; ferryman open |
| G8 | 2 bombs giving milk to the baby's tears (after playing the flute there) | sound node freed by `discard.view(40)`, still linked | heap release cuts the sound list (`release.c`) | **Fixed**, confirmed by full playthrough |
| G7 | Pawn shop: trading the nightingale after using the arrow keys gives the map talk, bird lost | stale `v100` (logic 8) | clear `v100` before the item conversation | **Fixed** |
| M9 | Oyster / flame animation draws over text | flag-15 / `print.at` windows stay open while the game cycles; the sprite blit copies straight to the screen | interpreter: sprite blit leaves out the open box (`slots/blit.c`, findings 22) | **Done**, tested in Hatari |
| M10 | `/` (state save key) makes Alex fall at the Cliffs | the cliff logic reacts to any key | logic: ignore that key | Likely easy |
| M11 | Return on etchings text auto-presses a button | the Return that closes the message is seen again by the logic | logic: clear input after the message | Likely easy |
| M12 | Return on stair texts makes Alex lose balance | same | same | Likely easy |
| M13 | Stairs are tedious | design | **skip to the top** after the first etching (logic 15) | Done |
| M14 | No hint that the treasure-room door needs "talk" | message text | change message | Easy |

Fixes in the game data (logics, pictures) would also help the DOS version, so they are worth
sending to Brandon. Interpreter fixes are ST-only (PP's side).

## How a fix gets onto the ST

- **Interpreter** (`SIERRA.PRG`): a patch script in `tools/` (`patchheap.py`, `patchslots.py`), each
  checks the original bytes first. Add it to `mkhd.sh`. Copy the result to `MG.PRG` as well.
- **Scripts**: add a function to `tools/logicpatches.py` (`replace(code, offset, old_hex, new_hex)`,
  comment why), then `python3 -I tools/logicpatches.py work/est build/KQ6ST-HD`. It always starts from
  the original `LOGDIR`/`VOL.1` in `work/est`, so it can be re-run any time. Check the result with
  `agidis.py` (and compare jump targets) before testing.
- **Test** in Hatari, ideally from one of the user's saves (copy it into a scratch folder as `sg.1`;
  never test in the user's folder).
- Writing `LOGDIR`/`VOL.1` while the user's Hatari runs from the folder was harmless (VOL.1 only grows
  at the end; LOGDIR is read once at startup), but the user must restart the game to get the change.
- The floppy image route (`E.ST` + PP's loader) would need a FAT12 writer; not done, not needed now.
- AGI byte encoding for patches: action opcode then args (`assignn` 03 vv nn, `addn` 05 vv nn,
  `set` 0C ff, `reset` 0D ff, `new.room` 12 rr, `print` 65 mm, `erase` 24 oo); conditions inside
  `FF … FF <s16 skip>`; `FE <s16>` = goto; `FC` = or-bracket, `FD` = not.

## Tools

In `tools/` (Python 3, run with `python3 -I`):

| Tool | Use |
|---|---|
| `fat12.py <img> [outdir]` | list / extract a FAT12 ST image |
| `tosprg.py` | load a TOS PRG (text+data, relocation offsets) |
| `findtab.py <prg> <toolsdir>` | find runs of relocated pointers (dispatch tables) |
| `agires.py <gamedir>` | AGI v2 dir/vol reader, logic message decryption |
| `agidis.py <gamedir> <outdir>` | disassemble all logics → `work/logic/logicNNN.txt`, prints opcode usage (`work/usage.txt`) |
| `words.py WORDS.TOK` | vocabulary dump (`work/words.txt`) |
| `wordpatches.py <srcdir> <outdir>` | vocabulary additions (`ADD` table: word → group) → new `WORDS.TOK` |
| `patchheap.py <in> <out> [kb]` | enlarge the ST AGI heap in `SIERRA.PRG` or a disk image (checks the original bytes) |
| `mkhd.sh <E.ST> <outdir> [kb]` | build a hard-disk game folder (no PP loader): game files, heap + slot patches, MG.PRG, saves from the image |
| `agipatch.py`, `logicpatches.py <src> <dst>` | patch logic bytecode (jump fix-ups), append to VOL; the list of script fixes |
| `patchslots.py <in.prg> <out.prg>` | 99 save slots with a scrolling list (`slots/slots.c`, `slots/slots.ld.in`) + heap-release sound-list fix (`slots/release.c`) + menu from the pointer (`slots/menu.c`) + pointer that keeps text (`slots/pointer.c`) + sprite blit that leaves out an open message box (`slots/blit.c`) + click to interact (`slots/act.c`, `slots/hotspots.h`) + clickable number menus (`slots/getnum.c`) + pointer in the inventory picker (`slots/inv.c`) |
| `slots/pf.c` | pathfinding for click to interact (findings 26); compiles natively too, for host tests |
| `prgdis.py <prg> <out.s>` | 68000 disassembly of a TOS PRG |
| `agipic.py <gamedir> <outdir> [pic…]` | render AGI pictures (visual + priority/control, grid) → `picNNN.png` |
| `roomsaid.py <logicdir> <words.txt> [room…]` | per room: objects, said() commands, posn boxes, first message |
| `checkhotspots.py <gamedir> <patched logicdir>` | validate `slots/hotspots/*.h` against vocabulary and room logics |
| `hotspotmap.py <gamedir> <logicdir> <outdir> [room…]` | draw hotspot boxes and stand boxes over each room picture |
| `hatari/` | headless Hatari test harness (see below) |
| `ops.json` | AGI v2 command names/args, taken from ScummVM `opcodes.cpp` |

Available on this machine: Hatari 2.6.1 (`~/.local/bin/hatari`), Python capstone 5.0.7 (68000
disassembly), m68k cross gcc (`source ~/.local/opt/m68k-gcc/env.sh`), ScummVM source with the AGI
engine at `../KingsQuest5ST/scummvm-master/engines/agi/`, Steem SSE in `../KingsQuest5ST/`.

Still wanted:
- **Brandon's source** (the AGI Studio / WinAGI project with the logic source files; `KQ6AGI/` is
  only the compiled release). Editing
  logic in source and recompiling is far easier and safer than patching bytecode.
- Not needed: the original Sierra KQ6 (SCI 1.1) or its decompiled scripts (sluicebox/sci-scripts).
  Brandon rewrote the game from scratch in AGI, so no code carries over. At most it is a reference
  for how a scene behaved in the original.
- **WinAGI GDS** (runs under Wine) for picture/view editing and logic compiling, if no source.
- From the user: exact crash text (or number of bombs), and AGI save games (or the debug steps)
  just before each crash, to reproduce in Hatari.
- Possibly from PP: what `HAG2`/`KQ6HE*.TOS` patch in memory, and whether the "memory error" text
  comes from his exception handler.

## What carries over from the KQ5 ST project (`../KingsQuest5ST`)

- Hatari headless runs + debugger breakpoints/screenshots for testing without a window.
- Capstone-based, relocation-aware 68000 disassembly (`port/tools/disasm.py`) – adapt for TOS PRG.
- Byte patches that check the original bytes before patching (`port/st/patches/prog.patch` style).
- ScummVM's engine source as the reference for resource formats and interpreter behaviour (AGI
  here instead of SCI).
- Keep notes in this file; checkpoint before moving to the next issue.

### Headless Hatari harness (`tools/hatari/`)

- `hc.sh <script> <disk.st>` drives Hatari through `hconsole` (keypresses via the control socket).
  Output in `work/hatari/` (screenshots, `hc.log`). Script extras: `shot NAME`, `type TEXT`. Notes on
  timing are in the script header: keys must be ~0.15 s apart, ~3 s after Return, ~12 s after a
  room change.
- `start-debug.txt` – title → Play → debug on. `heap-check.txt` + `heap.ini` + `heap.py` – read the
  heap counters from memory (fixed addresses: with this boot the text segment loads at `0xaa9a`,
  so heap end is at `0x1ea8e`, base `0x1eb1e`, top `0x1eb30`, max `0x1eb40`; check with debugger
  `info basepage` if the setup changes). `door.txt` – magic-door attempt; `bomb.ini` dumps `$380`
  (TOS crash save area: magic, D0–D7, A0–A7, exception number, USP, stack with SR/PC).
- `show mem` typed in-game did not show up in the screenshots; reading memory is more reliable.
- `hc.sh <script> -hd <dir> 'C:\PROG.PRG'` runs from a GEMDOS folder (4 MB); `HLOG=warn HEXTRA="--trace
  gemdos"` logs every GEMDOS call (how the `MG.prg` check and HAG2's RAM copy were found).
- `keypress N` with one digit sends that character: Esc is `keypress 01`. No mouse moves in hconsole: write
  the pointer position (`$fe3a` x, `$fe3c` y, words) with the debugger `w` command.
- Lessons: runs are **timing-flaky** (a missed key shifts everything; rerun before concluding). Keys:
  Return 28, Esc 01 (not 1), Space 57, Backspace 14, arrows up 72 / down 80 / left 75 / right 77; `type` sends
  characters (digits and letters work). With the Swedish TOS, `\` and `:` do not come out of the
  obvious scancodes – avoid typing paths (the default save path `\` is the game folder). Number
  prompts (`get.num`) treat an empty answer as 0 – don't leave a stray Return in the script.
- Restoring a save in a script: `type restore game`, Return, Return (path), Return (slot), Return (OK),
  ~10 s. Debug `pos X Y` is the quickest way to put ego on a trigger.
- Decoding the user's saves (`sg.N`, see 13): vars at block 1 offset 8, flags at offset 264. Diffing two
  saves around an event shows what the event changes (used for the cliffs skip).
- `tools/prgdis.py` – capstone disassembly of a TOS PRG text segment (`work/sierra.s`).

### Testing like real hardware (ACSI image)

Hatari's GEMDOS drive (`-d`) is not what a real ST does: TOS's own GEMDOS on a hard disk loads the program
at a different address (2 mod 4 instead of 0 mod 4 – this crashed KQ5 ST v0.1.1 on real hardware and Steem),
is slower, and has fewer file handles. Before a release, also run the release folder from an ACSI image with a
real driver (AHDI 6.061 image from Steem SSE in `../KingsQuest5ST`):

    cp ../KingsQuest5ST/Steem.SSE.4.2.0.Linux64/ACSI/SH204.img hd.img
    python3 -I tools/acsiput.py hd.img <unpacked release>/KQ6ST KQ6ST
    tools/hatari/hc.sh <script> -acsi hd.img 'C:\KQ6ST\SIERRA.PRG'

`-acsi` turns off Hatari's GEMDOS drive (a copy of `~/.config/hatari/hatari.cfg`, else it takes C:). Script:
~40 s to the first disclaimer, Esc, Esc, Return (Play), ~20 s, Return (welcome box).
Checked 2026-10-20 for v0.1.0 against the three KQ5 ST real-hardware bugs: no NatFeats in our code (KQ5's
unguarded `$7300` gave 4 bombs); all addresses in `slots/` are relocated from the load address (`B +` in
`slots.ld.in`, nothing assumes long alignment); 10 files in the folder, so no slow lookups. From the ACSI image
the game boots, plays, saves (`\kq6st\sg.1`, default path = the game folder) and restores.

### Brandon's debug steps

`debugging-steps.txt` (from Brandon, added 2026-10-18): how to skip each crash scene with debug
mode (`set var 3` = score, `set flag`, `get object`, `tp <room>`, then `reset flag 32` and walk to
another screen to free the debugger's memory). Useful room numbers from it: 25 lair exit, 2 Crown
beach, 232 druid cage, 48 gate (Styx), 228 ending animation, 51 inside the castle (magic door),
111 sword fight, 112 wedding. Flags: 163, 117, 75, 114 (Minotaur done), 166 (Realm of the Dead
done, long path), 247 (genie saved in lamp). Objects 43 (dagger), 33 (sacred water).

## Next steps

0. Done 2026-10-19: full playthrough in Hatari, no bombs, no errors (G1–G8 all OK).
1. Only if the magic door / Nightmare bombs reappear (e.g. on the real 1040ST): get the save from just before,
   reproduce in Hatari (`-hd` scratch copy), read the crash PC from `$380` (`tools/hatari/bomb.ini`)
   and map it into `work/sierra.s`.
2. Druids, Charon, Jollo: confirmed OK (if "No memory" ever returns: `patchheap.py ... 192`).
3. Easy script fixes with `logicpatches.py`: M10–M12 (keys at the cliffs – partly moot now that the
   climb is skipped, but the first etching still uses them), M14 (door hint text, needs a message
   change: `agipatch` does not edit messages yet).
4. Brandon report written: `BUGS-FOR-BRANDON.txt` (G7 pawn shop `v100`, logic 90 "take mint" = eat, typos in
   logics 8/234, typed-commands suggestion, unverified M9/M11/M12/M14). The user sends it.
5. Untested: the mouse loop of the new slot list; running `build/KQ6ST-HD` on the real 1040ST.
6. Click to interact: all 51 rooms with commands have hotspots (findings 23), pathfinding done (findings 26). Next:
   the user's play – correct hotspot boxes, report spots where he still gets stuck (other objects are not avoided). Plan in `CLICK-TO-INTERACT.md`.
