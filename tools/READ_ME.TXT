KING'S QUEST VI AGI DEMAKE - ATARI ST          fixed version @VERSION@
=====================================================================

Brandon Kouri's King's Quest VI AGI Demake (https://kq6agi.com), on the
Atari ST. Marakatti made the first working ST disk image of it, Petari (PP)
its hard-disk conversion. This version fixes the crashes found on the ST
("memory error" at the Minotaur, the druids, Charon and Jollo; 2 bombs at
the baby's tears) and adds mouse controls.

Project page and source: https://github.com/jarnalyrkar/KQ6ST


REQUIREMENTS
------------
- Atari ST or STE, colour monitor, low resolution.
- Tested with 4 MB RAM in Hatari (STE, TOS 1.62). Less RAM, other TOS
  versions, EmuTOS and real hardware are untested.
- A hard disk, or a GEMDOS hard-disk folder in an emulator. About 2 MB.


INSTALLING
----------
Copy the KQ6ST folder anywhere on a hard-disk partition and run SIERRA.PRG
from it. Keep MG.PRG next to SIERRA.PRG: the interpreter checks for it.
Do not use the HAG2 loader / RUNME.TOS from the older ST release with
this folder.

Hatari (tested):
  hatari --machine ste --memsize 4 --tos <your TOS image> \
         --harddrive KQ6ST --auto 'C:\SIERRA.PRG'
  Or in the GUI: System = STE, Memory = 4 MB, Hard disks -> GEMDOS drive =
  the KQ6ST folder, then double-click SIERRA.PRG on drive C.

Steem SSE (untested): Disk Manager -> Hard Drives -> map C: to the KQ6ST
  folder, then run SIERRA.PRG from drive C.

Real hardware (untested): copy the KQ6ST folder to your hard disk and run
  SIERRA.PRG.


CONTROLS
--------
Type commands as in any Sierra AGI game ("look", "get ring", "talk").
- Click: Alexander walks there, around obstacles. Click near the left,
  right or bottom edge, or above the horizon, to leave the room that way.
- Right click or double click on something: Alexander walks to it and
  does the obvious thing (take, open, look ...).
- Move the pointer to the top of the screen, or press Esc: menu bar.
- Esc skips the disclaimers and title texts.
- Menus, the inventory picker, the ferryman's questions, the boxes and the
  sword fight work with the mouse; the keys still work too.
- 99 save slots. Saved games are written to the game folder (SG.*).
  "load" works the same as "restore".
- The climb up the Cliffs of Logic is skipped after the first etching.


KNOWN PROBLEMS
--------------
- Played from start to finish in Hatari without crashes. Not yet tested
  on a real ST.
- Click to interact: some hotspots may be slightly off, and Alexander can
  still get stuck at some spots (other characters are not walked around).
  Typing the command always works.
Please report problems on the project page.


COPYRIGHT
---------
King's Quest is (c) Sierra On-Line, Inc.; the rights now belong to
Activision (Microsoft). The interpreter SIERRA.PRG (Sierra's Atari ST AGI
interpreter, (c) 1987 Sierra On-Line) and the graphics and sounds the game
borrows from Sierra's games are Sierra's. The game itself (LOGDIR, VOL.*,
WORDS.TOK, OBJECT and the other game files) is Brandon Kouri's work. This is
a non-commercial fan project, not affiliated with or endorsed by Sierra,
Activision or Microsoft. If you enjoy it, buy the original King's Quest VI
(GOG, Steam). This project's own code is GPL-3.0-or-later; source on the
project page.

Thank you, Brandon Kouri, for eighteen years of work on this game.
