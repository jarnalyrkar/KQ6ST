/* Isle of Wonder. Format: see ../hotspots.h. */

/* 15: sandy cove below the etched cliffs (logic 15). Feather = object 1, flower = 2 (view 22); 3-5 are waves. */
OBJ(15, 1, 22, NEAR(100, 111, 119, 120), "get feather"),
OBJ(15, 2, 22, NEAR(34, 95, 62, 128), "get flower"),
AREA(15, 50, 30, 115, 99, NEAR(60, 99, 89, 109), "look cliff"),
AREA(15, 30, 101, 118, 128, ANY, "look sand"),
AREA(15, 30, 118, 140, 167, ANY, "look ocean"),
AREA(15, 0, 60, 30, 167, ANY, "look ocean"),
AREA(15, 0, 0, 159, 100, ANY, "look rocks"),
AREA(15, 118, 100, 159, 167, ANY, "look rocks"),

/* 16: oyster beach (logic 16). Oyster = object 3 (view 23), floating letters = 7 (view 227); 1, 2 are waves. */
OBJ(16, 7, 227, ANY, "get letters"),
OBJ(16, 3, 23, NEAR(28, 88, 57, 117), "talk oyster"),
AREA(16, 0, 30, 30, 135, ANY, "look rocks"),
AREA(16, 125, 65, 159, 150, ANY, "look rocks"),
AREA(16, 30, 68, 130, 130, ANY, "look sand"),
AREA(16, 0, 130, 159, 167, ANY, "look ocean"),
AREA(16, 0, 15, 159, 67, ANY, "look jungle"),

/* 17: book garden (logic 17). Bees = objects 1-7 (view 30); 8-13 only in cutscenes. */
OBJ(17, 1, 30, ANY, "look bees"),
OBJ(17, 2, 30, ANY, "look bees"),
OBJ(17, 3, 30, ANY, "look bees"),
OBJ(17, 4, 30, ANY, "look bees"),
OBJ(17, 5, 30, ANY, "look bees"),
OBJ(17, 6, 30, ANY, "look bees"),
OBJ(17, 7, 30, ANY, "look bees"),
AREA(17, 18, 115, 55, 167, NEAR(23, 130, 46, 158), "look web"),
AREA(17, 62, 56, 84, 125, ANY, "look books"),
AREA(17, 105, 55, 159, 140, ANY, "look books"),

/* 19: swamp (logic 19). Stick-in-the-mud = object 5, bump-on-a-log = 6 (view 34); 2-4 animate the swamp.
   The milkweed is drawn into the picture (view 10 at 25,110). */
OBJ(19, 5, 34, ANY, "look stick"),
OBJ(19, 6, 34, ANY, "look log"),
AREA(19, 22, 92, 42, 111, NEAR(20, 110, 40, 125), "get milk"),
AREA(19, 0, 128, 50, 167, ANY, "look log"),
AREA(19, 0, 0, 40, 100, ANY, "look tree"),
AREA(19, 40, 78, 159, 167, ANY, "look swamp"),

/* 20: garden (logic 20). Gate = object 1 (view 35), hole in the wall = 5 (225), rotten tomato = 6 (222),
   teacup = 10 (130), potion bottle = 11 (157). The lettuce is drawn into the picture (view 221 at 77,123). */
OBJ(20, 10, 130, NEAR(144, 71, 159, 89), "get cup"),
OBJ(20, 11, 157, NEAR(105, 111, 141, 140), "get bottle"),
OBJ(20, 6, 222, DIST(6, 10), "get rotten tomato"),
OBJ(20, 5, 225, NEAR(97, 52, 156, 90), "look hole"),
OBJ(20, 1, 35, NEAR(56, 83, 87, 99), "open gate"),
AREA(20, 57, 75, 87, 95, NEAR(56, 83, 87, 99), "open gate"),
AREA(20, 76, 110, 92, 124, NEAR(67, 103, 91, 130), "get lettuce"),
AREA(20, 148, 45, 159, 78, ANY, "look chair"),
AREA(20, 117, 117, 140, 133, ANY, "look table"),
AREA(20, 8, 48, 30, 76, ANY, "look grapes"),
AREA(20, 48, 100, 72, 140, ANY, "look tomato vines"),
AREA(20, 24, 80, 48, 112, ANY, "look snapdragons"),
AREA(20, 0, 95, 30, 150, ANY, "look baby"),
AREA(20, 40, 85, 118, 167, ANY, "look path"),
AREA(20, 0, 10, 159, 85, ANY, "look wall"),
AREA(20, 0, 85, 159, 167, ANY, "look bushes"),

/* 21: Chessboard Land entrance (logic 21). Knights = objects 1, 2 (view 36), red queen's scarf = 3 (view 18). */
OBJ(21, 3, 18, NEAR(75, 110, 100, 130), "get scarf"),
OBJ(21, 1, 36, NEAR(33, 111, 120, 130), "talk knight"),
OBJ(21, 2, 36, NEAR(33, 111, 120, 130), "talk knight"),
AREA(21, 18, 22, 35, 110, ANY, "look pillars"),
AREA(21, 120, 25, 140, 110, ANY, "look pillars"),
AREA(21, 45, 85, 110, 112, ANY, "look path"),
AREA(21, 0, 110, 159, 167, ANY, "look steps"),
AREA(21, 0, 50, 159, 110, ANY, "look"),
AREA(21, 0, 0, 159, 49, ANY, "look clouds"),
