/* Isle of the Mists. Format: see ../hotspots.h. */

/* 39: misty beach (logic 39). Objects 1-4 (views 247, 78) are waves / fog, no noun. */
AREA(39, 44, 92, 74, 106, ANY, "look rocks"),
AREA(39, 88, 94, 118, 114, ANY, "look rocks"),
AREA(39, 75, 152, 110, 167, ANY, "look rocks"),
AREA(39, 120, 128, 159, 145, ANY, "look rocks"),
AREA(39, 30, 70, 120, 115, ANY, "look path"),
AREA(39, 45, 115, 159, 167, ANY, "look ocean"),
AREA(39, 118, 72, 159, 128, ANY, "look ocean"),
AREA(39, 0, 0, 159, 68, ANY, "look trees"),
AREA(39, 0, 69, 40, 167, ANY, "look trees"),

/* 40: druid village (logic 40). Scythe = object 1 (view 76); the fire pit (coal) is in the picture. */
OBJ(40, 1, 76, NEAR(3, 71, 21, 102), "get scythe"),
AREA(40, 40, 116, 73, 142, NEAR(37, 113, 79, 148), "get coal"),
AREA(40, 112, 60, 155, 110, ANY, "look house"),
AREA(40, 18, 40, 62, 105, ANY, "look house"),
AREA(40, 0, 110, 30, 167, ANY, "look rocks"),
AREA(40, 30, 60, 135, 167, ANY, "look path"),
AREA(40, 0, 0, 159, 59, ANY, "look trees"),

/* 41: circle of stones (logic 41). Bonfire = object 1 (view 68 burning, 38 embers), cage = 2 (view 77);
   objects 3-8 are the druids (ceremony cutscene). */
OBJ(41, 1, 68, ANY, "look fire"),
OBJ(41, 1, 38, ANY, "look fire"),
OBJ(41, 2, 77, ANY, "look cage"),
AREA(41, 62, 115, 108, 132, ANY, "look fire"),
AREA(41, 60, 5, 120, 60, ANY, "look tree"),
AREA(41, 30, 40, 122, 108, ANY, "look stones"),
AREA(41, 0, 0, 30, 167, ANY, "look stones"),
AREA(41, 122, 0, 159, 167, ANY, "look stones"),
