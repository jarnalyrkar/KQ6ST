/* Realm of the Dead. Format: see ../hotspots.h. */

/* 43: surface of the Realm of the Dead (logic 43). Ghouls = objects 1 (view 205) and 2 (206); Cassima's
   parents = object 4 (view 207; talking needs them within distance 10, checked by the logic). */
OBJ(43, 4, 207, DIST(4, 10), "talk ghosts"),
OBJ(43, 1, 205, ANY, "look zombies"),
OBJ(43, 2, 206, ANY, "look zombies"),

/* 44: Realm of the Dead, mother's ghost (logic 44): object 1, view 205. */
OBJ(44, 1, 205, ANY, "talk ghost"),

/* 45: entrance to the Underworld (logic 45). Skeletons, key and bones are added to the picture (view 209):
   guard loop 0 at 46,134 (17x62), loop 1 at 87,118 (12x44), key loop 2 at 59,128 (on the skeleton) and
   84,151 (ground), bones loop 9 at 110,164; the bone marimba is in the picture. */
AREA(45, 82, 142, 89, 153, NEAR(79, 137, 93, 161), "get key"),
AREA(45, 58, 120, 63, 129, NEAR(79, 137, 93, 161), "get key"),
AREA(45, 108, 134, 138, 164, NEAR(103, 141, 129, 166), "get bones"),
AREA(45, 46, 73, 62, 134, NEAR(45, 122, 65, 148), "talk skeleton"),
AREA(45, 87, 75, 98, 118, NEAR(45, 122, 65, 148), "talk skeleton"),
AREA(45, 56, 60, 80, 92, ANY, "look door"),

/* 46: inside the Underworld (logic 46). Torches = objects 1-3 (view 55); the knight's remains are in the
   picture (the ghost, object 8, only appears in the cutscene). */
OBJ(46, 1, 55, ANY, "look flames"),
OBJ(46, 2, 55, ANY, "look flames"),
OBJ(46, 3, 55, ANY, "look flames"),
AREA(46, 124, 90, 159, 130, NEAR(132, 102, 157, 136), "look knight"),
AREA(46, 20, 92, 130, 140, ANY, "look path"),
AREA(46, 40, 141, 159, 167, ANY, "look river"),

/* 47: River Styx (logic 47). Boat = object 9 (view 210 loop 0), Charon = object 10 (view 210 loop 1). */
OBJ(47, 10, 210, NEAR(48, 112, 93, 142), "talk charon"),
OBJ(47, 9, 210, ANY, "look boat"),
AREA(47, 0, 50, 159, 125, ANY, "look river"),

/* 48: gate of the Underworld (logic 48). The gate = objects 5 and 6 (view 211); opening it starts the
   gate's scene (the logic checks distance 25 itself). */
OBJ(48, 5, 211, DIST(5, 25), "open gate"),
OBJ(48, 6, 211, DIST(5, 25), "open gate"),
AREA(48, 5, 95, 60, 167, ANY, "look path"),
AREA(48, 55, 65, 115, 100, ANY, "look path"),
AREA(48, 0, 60, 159, 167, ANY, "look river"),

/* 49: hall of the Lord of the Dead (logic 49). Guards = objects 5 and 6 (view 212); the Lord is in the
   picture. Other objects are cutscene only. */
OBJ(49, 5, 212, ANY, "look guard"),
OBJ(49, 6, 212, ANY, "look guard"),
AREA(49, 60, 0, 100, 90, ANY, "look lord"),

/* 50: before the Lord of the Dead (logic 50). He fills the picture; his moving parts are objects 2-7. */
AREA(50, 40, 0, 135, 167, ANY, "talk lord"),
