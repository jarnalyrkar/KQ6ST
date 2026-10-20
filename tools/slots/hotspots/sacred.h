/* Isle of the Sacred Mountain. Format: see ../hotspots.h. */

/* 25: top of the cliffs (logic 25). Night Mare = object 3 view 45 (until flag 116), old woman = object 10
   view 204. The nightshade bush is only looked at: "get berries" there poisons Alexander. */
OBJ(25, 3, 45, ANY, "look horse"),
OBJ(25, 10, 204, ANY, "talk woman"),
AREA(25, 148, 85, 159, 102, NEAR(140, 96, 157, 111), "enter hole"),
AREA(25, 118, 78, 147, 100, ANY, "look bush"),
AREA(25, 20, 20, 70, 100, NEAR(24, 92, 69, 110), "open door"),
AREA(25, 100, 15, 150, 55, ANY, "look mountain"),
AREA(25, 110, 60, 125, 100, ANY, "look path"),
AREA(25, 15, 100, 145, 150, ANY, "look path"),

/* 26: dark cave (logic 26). Without light everything else answers "can't see". */
AREA(26, 15, 94, 28, 117, ANY, "enter hole"),

/* 27: lit cave (logic 27). */
AREA(27, 15, 88, 28, 112, NEAR(13, 109, 31, 122), "enter hole"),
AREA(27, 0, 45, 159, 140, ANY, "look cave"),

/* 28: second cave room with the mint plant (logic 28). */
AREA(28, 107, 130, 126, 156, NEAR(107, 138, 127, 159), "get mint"),
AREA(28, 2, 22, 14, 68, ANY, "look window"),
AREA(28, 0, 0, 159, 167, ANY, "look cave"),

/* 29: gate of the City of the Winged Ones (logic 29). Guards = objects 1 and 2, view 49. */
OBJ(29, 1, 49, NEAR(58, 131, 106, 155), "talk guard"),
OBJ(29, 2, 49, NEAR(58, 131, 106, 155), "talk guard"),
AREA(29, 52, 10, 110, 45, ANY, "look mountain"),
AREA(29, 50, 46, 105, 130, ANY, "look gate"),
AREA(29, 110, 2, 159, 70, ANY, "look palace"),

/* 31/102 catacomb maze, 32 acid room, 33 crushing ceiling, 34 dark, 35 hole in the wall: none. Room 31 changes
   its picture per maze cell (v100) and its commands live in logic 102, 33/35 are timed or item puzzles, 34 is dark. */

/* 36: catacombs, tapestry room (logic 36, picture 31). Tapestry = object 2 view 67. */
OBJ(36, 2, 67, NEAR(116, 119, 131, 145), "look tapestry"),

/* 37: minotaur's lair (logic 37). Fire pit = object 1 view 68, minotaur = 2 view 66, Winged One girl = 3 view 70
   (both only before the fight is won). */
OBJ(37, 2, 66, ANY, "look minotaur"),
OBJ(37, 3, 70, ANY, "look girl"),
OBJ(37, 1, 68, ANY, "look fire"),
AREA(37, 120, 30, 145, 72, ANY, "look skull"),
AREA(37, 34, 37, 62, 125, ANY, "look altar"),
