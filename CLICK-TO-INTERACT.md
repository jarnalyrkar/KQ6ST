# Click to interact – investigation (not implemented)

Idea from the user (2026-10-20): double-click or right-click on something in a room and Alexander walks
there and does the obvious thing with it. Double-click the ring on the beach → he picks it up; the board →
he moves it aside; the chest → he opens it; a door → he opens it; a hole → he looks through it.

Status (2026-10-20): **mechanism built, beach (room 2) done, no walking yet** – see `PROJECT.md` findings 23.
`tools/slots/act.c` does the click → hotspot → parser part (the word injection question below is solved: the
interpreter's own parser `0x7022` is called on the hotspot's command string); hotspots are in
`tools/slots/hotspots.h`. Double click or right click; the room logic answers, including "not
close enough". Since the same day he first walks in a straight line into the room logic's stand-here box
(`NEAR(...)` per hotspot). Next: the other rooms; real pathfinding around obstacles (level 2: search over the
interpreter's control/priority map, est. 2–3 sessions) only if straight lines turn out not to be enough.
Later the same day: hotspots for all 51 rooms with commands (347, `tools/slots/hotspots/*.h`), `DIST(obj, d)` for
distance-checked commands, tools `agipic.py`, `roomsaid.py`, `checkhotspots.py`, `hotspotmap.py` (PROJECT.md findings
23). Known limit: straight lines got stuck where the stand spot is fenced by walk barriers (pawn-shop door) →
pathfinding added the same day (`tools/slots/pf.c`, PROJECT.md findings 26).
Original investigation, kept for reference: Read `PROJECT.md` first for the interpreter and tool context
(findings 17 and 21 in particular).

## Verdict

Medium-to-large job. The mechanism is a contained interpreter change (about one session including Hatari
testing). Most of the effort is the data: each room needs its clickable areas drawn by hand.

## What already makes it easier

- **The room logic already does the action from word numbers.** Beach, `work/logic/logic002.txt`:
  ```
  if said(33,36)               ; get/take/... + ring
     if posn(0,50,104,66,128)  ; ego must stand here
        "Alexander picks up his royal insignia ring..."
     else "not close enough"
  ```
  So a click only has to walk Alexander into the right spot and feed in the words as if typed. Item,
  score and "already picked up" stay with the room logic.
- **One word group covers most verbs.** Group 33 (`work/words.txt`) = aquire climb flip get grab knock
  lift move obtain open pick up pull push remove slide slip take touch uncover. Board, chest and door are all
  `33 + noun`. Group 24 = look/examine/read (the hole), 25 = talk/speak (people), 172 = put/place.
  Each hotspot only needs a noun and a verb type, which can mostly be picked from the noun.
- **Where Alexander must stand can be extracted automatically.** Of 947 `said()` tests in all logics,
  196 have a `posn(0, x1,y1,x2,y2)` check within the next few lines, across 52 rooms. A script can turn
  them into (room, verb, noun, stand-here box) tuples. Rooms with such pairs (room: count):
  0:1 2:6 3:2 4:5 6:4 7:3 8:2 9:6 10:5 11:3 15:4 16:1 17:4 19:8 20:20 21:2 22:3 23:5 24:5 25:8 26:2 27:2
  28:4 29:3 32:2 35:1 36:4 40:4 41:1 42:1 45:6 46:2 47:4 48:1 49:1 52:3 53:3 56:3 57:8 59:4 60:3 62:4 63:4
  64:4 65:2 67:4 68:3 90:1 99:1 102:3 103:3 213:8.
  Verb counts over all `said()`: 24 look 429, 33 get/open/… 236, 25 talk 151.
- **The mouse plumbing from the cursor rooms is reusable** (findings 21): `get.mse.posn(v240,v241)` runs
  every cycle, flag 19 = left click this cycle, `tools/slots/menu.c` runs every cycle. The right button
  posts event `0x101`, which KQ6 maps to controller 30 and never uses, so it is free.

## The work

1. **Mechanism (interpreter C, `tools/slots/`), about one session.**
   - Notice a right click, or two left clicks close in time and place (the first click's walk is
     simply replaced).
   - Find the hotspot under the pointer.
   - Walk Alexander to the nearest point in its stand-here box (motion like the click walk, `0x6236`).
   - On arrival, inject the two words and set the "input received" state.
   - Cancel on another click or typed input; stay off in cutscenes, text boxes / text mode and when the
     player has no control (`$fa4a`).
   - Unknown: how to inject the words. Either find the parsed-word buffer that `said()` reads in
     `work/sierra.s`, or call the interpreter's own `parse` routine (AGI command 122 exists in 1.422) on a
     string. Both look doable.
2. **Hotspot data – the bulk.** The logic gives where to stand, not where the thing is on screen.
   - Things that are animated objects (the ring is object 2) can be hit-tested live from the object
     table and vanish by themselves once taken.
   - Picture features (doors, holes, chests, …) need hand-drawn boxes: a guess of 300–400 over about 80
     rooms, 10–30 min per room. No PIC renderer in `tools/` yet – write one (~100 lines, AGI PIC format)
     or use Hatari screenshots.
   - Data as a compact table generated by a Python script from a hotspot list file, linked into the
     interpreter (a few KB; fine on a 1040ST).
   - A useful first cut: only the 52 rooms with stand-here checks, or only the main puzzle objects.
     Typing keeps working everywhere.

## Limits

- (Original plan; pathfinding now exists, findings 26.) Walking was a straight line with no pathfinding; if Alexander gets stuck, the room's own "not close
  enough" message appears (acceptable).
- "Use X on Y" with an inventory item is not covered (would need an inventory cursor on top).
- Nouns with two meanings (open vs knock on a door) need one chosen default per hotspot.

## KQ4 note

Not found in the KQ4 ST game scripts (`KQ4-sci-atari-st/src`): `User.sc` only walks to a click. So in
KQ4 it presumably comes from the SCI interpreter itself; nothing from KQ4 can be reused here.

## Suggested first step

Build the mechanism plus the beach (room 2: ring, board, box, hole) as a pilot. That shows how it feels in
play and gives a real per-room time before committing to the whole game.
