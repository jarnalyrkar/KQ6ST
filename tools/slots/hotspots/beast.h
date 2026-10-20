/* Isle of the Beast. Format: see ../hotspots.h. */

/* 22: Isle of the Beast shore (logic 22). Fawn = object 3 (view 218), dangling participle = 4, squirrel = 5
   (view 37); 1, 2, 6 are waves. */
OBJ(22, 4, 37, NEAR(75, 75, 106, 100), "talk creature"),
OBJ(22, 5, 37, ANY, "look squirrel"),
OBJ(22, 3, 218, ANY, "look fawn"),
AREA(22, 0, 95, 45, 150, ANY, "look stump"),
AREA(22, 85, 32, 118, 88, ANY, "look rocks"),
AREA(22, 48, 80, 125, 132, ANY, "look path"),
AREA(22, 0, 130, 159, 167, ANY, "look ocean"),
AREA(22, 0, 0, 159, 95, ANY, "look trees"),

/* 23: boiling pond (logic 23). Rabbit = object 3, hunter's lamp = 4 (view 38); 1, 2 animate the pond. */
OBJ(23, 4, 38, ANY, "get lamp"),
OBJ(23, 3, 38, ANY, "look rabbit"),
AREA(23, 37, 99, 100, 131, ANY, "look pond"),
AREA(23, 85, 133, 105, 160, ANY, "look mushrooms"),
AREA(23, 133, 140, 142, 160, ANY, "look mushrooms"),
AREA(23, 70, 68, 110, 130, ANY, "look rocks"),
AREA(23, 12, 85, 50, 120, ANY, "look rocks"),
AREA(23, 48, 62, 85, 98, ANY, "look path"),
AREA(23, 20, 130, 80, 167, ANY, "look path"),
AREA(23, 0, 0, 15, 167, ANY, "look trees"),
AREA(23, 115, 0, 159, 167, ANY, "look trees"),

/* 24: gated wall with the archer statues (logic 24). Gate = object 1, brick = 2, gardener = 3 (view 39);
   4, 5 only in cutscenes. */
OBJ(24, 2, 39, NEAR(116, 136, 141, 155), "get brick"),
OBJ(24, 3, 39, ANY, "talk gardener"),
OBJ(24, 1, 39, ANY, "look gate"),
AREA(24, 10, 50, 65, 76, NEAR(10, 73, 63, 88), "get rose"),
AREA(24, 20, 18, 60, 49, ANY, "look gazebo"),
AREA(24, 75, 55, 90, 118, ANY, "look statue"),
AREA(24, 105, 78, 142, 125, ANY, "look gate"),
AREA(24, 15, 80, 90, 135, ANY, "look wall"),
AREA(24, 90, 115, 135, 167, ANY, "look path"),
AREA(24, 0, 0, 18, 167, ANY, "look trees"),
AREA(24, 140, 0, 159, 167, ANY, "look trees"),

/* 42: fountain clearing by Beast's hedge maze (logic 42). Gate = object 1, fountain water = 3 (view 83);
   2 (Beast) only in the cutscene. */
OBJ(42, 3, 83, ANY, "look fountain"),
OBJ(42, 1, 83, ANY, "look gate"),
AREA(42, 60, 78, 140, 112, ANY, "look fountain"),
AREA(42, 63, 18, 115, 48, ANY, "look castle"),
AREA(42, 40, 85, 80, 167, ANY, "look path"),
AREA(42, 0, 20, 40, 167, ANY, "look hedge"),
AREA(42, 140, 20, 159, 167, ANY, "look hedge"),
AREA(42, 40, 49, 140, 77, ANY, "look hedge"),
