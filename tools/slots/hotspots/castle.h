/* Castle of the Crown. Format: see ../hotspots.h. */

/* 51: basement hallway (logic 51). Dungeon door = object 1, strange west door = 2 (both view 185); the guard
   dogs are drawn by logic 200, so they get no hotspot. "open door" picks the door by where ego stands and its
   first posn box is the west door's, so the dungeon door is only looked at. */
OBJ(51, 2, 185, NEAR(24, 110, 38, 123), "talk door"),
OBJ(51, 1, 185, ANY, "look door"),
AREA(51, 20, 74, 33, 121, NEAR(24, 110, 38, 123), "talk door"),
AREA(51, 104, 92, 119, 148, ANY, "look door"),
AREA(51, 45, 40, 80, 100, ANY, "look hall"),

/* 52: basement, north door (logic 52). Door = object 1, suit of armor = 3 (view 187). The door is the
   dangerous one, so a click only looks at it (plaque). */
OBJ(52, 3, 187, NEAR(2, 123, 18, 133), "move armor"),
OBJ(52, 1, 187, ANY, "look door"),
AREA(52, 0, 0, 159, 121, ANY, "look hall"),

/* 53: grand hall (logic 53). Kitchen door = object 2, east door = 1, throne room doors = 5 (view 166). "open door"
   picks the door by where ego stands and its first posn box is the kitchen door's, so the others are looked at. */
OBJ(53, 2, 166, NEAR(34, 116, 48, 134), "open door"),
OBJ(53, 1, 166, ANY, "look door"),
OBJ(53, 5, 166, ANY, "look door"),
AREA(53, 70, 10, 94, 45, ANY, "look chandelier"),
AREA(53, 19, 116, 28, 143, ANY, "look urn"),
AREA(53, 136, 116, 144, 143, ANY, "look urn"),
AREA(53, 0, 80, 18, 167, ANY, "look stairs"),
AREA(53, 142, 60, 159, 167, ANY, "look stairs"),

/* 56: treasure room (logic 56). Drapery on the table = object 1 (view 193, until removed). */
OBJ(56, 1, 193, NEAR(48, 131, 84, 137), "remove drapery"),
AREA(56, 47, 104, 87, 132, NEAR(42, 113, 85, 145), "look treasures"),
AREA(56, 17, 60, 26, 110, ANY, "look shields"),
AREA(56, 117, 60, 127, 105, ANY, "look shields"),
AREA(56, 3, 120, 17, 155, ANY, "look urns"),
AREA(56, 131, 122, 148, 155, ANY, "look urns"),

/* 57: vizier's bedroom (logic 57). Trunk = object 3 (view 237), wardrobe = 7, door = 2, table with fireplace
   = 5, bed = 4 (view 161), fire = 1 (view 7). */
OBJ(57, 3, 237, NEAR(66, 130, 82, 136), "open trunk"),
OBJ(57, 7, 161, NEAR(16, 134, 33, 142), "open wardrobe"),
OBJ(57, 2, 161, NEAR(134, 133, 144, 142), "open door"),
OBJ(57, 1, 7, ANY, "look fire"),
OBJ(57, 5, 161, ANY, "look table"),
OBJ(57, 4, 161, ANY, "look bed"),

/* 59: secret passage, bottom of the stairs (logic 59). Secret doorway = object 1 (view 190). */
OBJ(59, 1, 190, NEAR(72, 155, 112, 167), "open door"),
AREA(59, 118, 62, 159, 132, NEAR(131, 133, 153, 160), "look hole"),
AREA(59, 42, 5, 117, 140, ANY, "look stairs"),
AREA(59, 0, 5, 159, 140, ANY, "look wall"),

/* 60: secret passage, top of the stairs (logic 60). */
AREA(60, 118, 66, 159, 128, NEAR(120, 126, 137, 154), "look hole"),
AREA(60, 52, 108, 100, 137, ANY, "look stairs"),
AREA(60, 0, 10, 159, 140, ANY, "look wall"),

/* 62: secret passage by Cassima's room (logic 62). Hidden door = object 1 (view 190). */
OBJ(62, 1, 190, NEAR(83, 108, 92, 125), "open door"),
AREA(62, 84, 70, 112, 115, NEAR(83, 108, 92, 125), "open door"),
AREA(62, 57, 90, 67, 140, NEAR(56, 149, 75, 167), "look hole"),
AREA(62, 0, 0, 159, 167, ANY, "look wall"),

/* 63: dungeon cell with the ghost boy (logic 63). Ghost = object 3 (view 167), door = 1, torch = 2 (view 55). */
OBJ(63, 3, 167, ANY, "talk ghost"),
OBJ(63, 2, 55, ANY, "look torch"),
OBJ(63, 1, 167, NEAR(17, 110, 44, 137), "open door"),
AREA(63, 15, 45, 45, 137, NEAR(17, 110, 44, 137), "open door"),
AREA(63, 83, 104, 139, 130, ANY, "look cot"),
AREA(63, 5, 25, 150, 147, ANY, "look cell"),

/* 213: the same cell when Alexander is locked in (logic 213, picture 63). Door = object 1, torch = 2. */
OBJ(213, 2, 55, ANY, "look torch"),
OBJ(213, 1, 167, NEAR(17, 110, 44, 137), "open door"),
AREA(213, 15, 45, 45, 137, NEAR(17, 110, 44, 137), "open door"),
AREA(213, 83, 104, 139, 130, ANY, "look cot"),
AREA(213, 5, 25, 150, 147, ANY, "look cell"),

/* 64: basement, east end (logic 64). East door = object 1, door at the top of the stairs = 2 (view 186). */
OBJ(64, 1, 186, NEAR(124, 117, 140, 129), "open door"),
OBJ(64, 2, 186, NEAR(10, 38, 28, 56), "look keyhole"),
AREA(64, 8, 0, 34, 50, NEAR(10, 38, 28, 56), "look keyhole"),
AREA(64, 0, 51, 38, 155, ANY, "look stairs"),
AREA(64, 60, 40, 140, 110, ANY, "look hall"),

/* 65: upstairs hallway, west (logic 65). Door = object 4 (view 177), guard dogs = 1, 2 (view 215). */
OBJ(65, 1, 215, ANY, "look dog"),
OBJ(65, 2, 215, ANY, "look dog"),
OBJ(65, 4, 177, NEAR(25, 104, 40, 124), "open door"),
AREA(65, 26, 72, 36, 117, NEAR(25, 104, 40, 124), "open door"),
AREA(65, 89, 35, 101, 70, ANY, "look statue"),
AREA(65, 9, 110, 26, 145, ANY, "look urns"),
AREA(65, 99, 110, 115, 145, ANY, "look urns"),
AREA(65, 100, 60, 126, 125, ANY, "look alcove"),
AREA(65, 20, 100, 105, 167, ANY, "look floor"),

/* 66: upstairs hallway, east (logic 66). Guard dogs = objects 1, 2 (view 215). The east door always answers
   "not close enough", so it is only looked at. */
OBJ(66, 1, 215, ANY, "look dog"),
OBJ(66, 2, 215, ANY, "look dog"),
AREA(66, 125, 72, 133, 118, ANY, "look door"),
AREA(66, 44, 110, 61, 145, ANY, "look urns"),
AREA(66, 134, 113, 151, 146, ANY, "look urns"),

/* 67: north upstairs hallway, Cassima's door (logic 67). The guard dogs (objects 1, 2) have no "look" here. */
AREA(67, 47, 86, 62, 122, NEAR(42, 124, 60, 131), "talk door"),

/* 68: alcove with the portrait (logic 68). Portrait = object 3, nail behind it = 4 (view 168), guard dogs = 1,
   2 (view 215, no "look" here). The portrait comes first: the nail is under it until the portrait is moved. */
OBJ(68, 3, 168, NEAR(98, 92, 115, 109), "move portrait"),
OBJ(68, 4, 168, NEAR(98, 92, 115, 109), "get nail"),
AREA(68, 57, 33, 68, 108, ANY, "look column"),

/* 103: Jollo's bedroom (logic 103, picture 57; furniture is added with add.to.pic). Jollo = object 2 (view 16),
   door = 1 (view 186). */
OBJ(103, 2, 16, DIST(2, 20), "talk jollo"),
OBJ(103, 1, 186, NEAR(7, 124, 30, 148), "open door"),
AREA(103, 7, 66, 27, 146, NEAR(7, 124, 30, 148), "open door"),
AREA(103, 104, 72, 116, 101, ANY, "look notice"),
AREA(103, 99, 100, 129, 126, ANY, "look desk"),
AREA(103, 128, 97, 150, 146, ANY, "look fire"),
AREA(103, 73, 109, 89, 129, ANY, "look trunk"),
AREA(103, 31, 105, 68, 133, ANY, "look bed"),
AREA(103, 10, 133, 150, 155, ANY, "look rug"),
