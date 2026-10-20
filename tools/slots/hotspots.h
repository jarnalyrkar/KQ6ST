/* Click-to-interact hotspots (act.c), first hit in a room wins, so objects and small areas go first.
   OBJ(room, object, view, stand, command): the object's cel pixels, while it is drawn with that view.
   AREA(room, x1, y1, x2, y2, stand, command): a box in play-area coordinates (x 0-159, y 0-167), inclusive.
   stand: NEAR(x1, y1, x2, y2) = the room logic's posn(0, ...) box for that command (ego walks into it
   first), DIST(object, d) = its distance(0, object, v) < d test (ego walks toward that object), or ANY
   (no walk).
   Commands are typed through the game's parser, so they use the room logic's own words.
   One file per island, roughly in game order; the room number is in each entry, so the order of the files
   does not matter. Check with tools/checkhotspots.py (words known, command handled by that room). */

#include "hotspots/crown.h"
#include "hotspots/wonder.h"
#include "hotspots/beast.h"
#include "hotspots/mists.h"
#include "hotspots/sacred.h"
#include "hotspots/dead.h"
#include "hotspots/castle.h"
