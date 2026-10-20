/* Isle of the Crown. Format: see ../hotspots.h. */

/* 2: Crown beach (logic 2). Ring = object 2, plank = 1, box = 6 (only after the plank is moved). */
OBJ(2, 2, 201, NEAR(50, 104, 66, 128), "get ring"),
OBJ(2, 6, 237, NEAR(69, 107, 90, 126), "open box"),
OBJ(2, 1, 1, NEAR(67, 104, 89, 129), "move plank"),
AREA(2, 30, 84, 52, 116, ANY, "look rocks"),
AREA(2, 0, 12, 40, 82, ANY, "look ship"),
AREA(2, 60, 56, 96, 100, ANY, "look path"),
AREA(2, 40, 100, 125, 140, ANY, "look sand"),
AREA(2, 120, 141, 159, 167, ANY, "look rocks"),
AREA(2, 50, 8, 159, 140, ANY, "look plants"),
AREA(2, 0, 83, 125, 167, ANY, "look ocean"),

/* 3: crossroads (logic 3). Nightingale = object 2, ribbon = 6, letter = 7 (all view 19); objects 1, 3, 5, 9
   only appear in cutscenes (input off). */
OBJ(3, 6, 19, NEAR(74, 104, 110, 127), "get ribbon"),
OBJ(3, 7, 19, NEAR(49, 106, 98, 132), "get letter"),
OBJ(3, 2, 19, ANY, "look bird"),
AREA(3, 132, 25, 158, 48, ANY, "look castle"),
AREA(3, 2, 48, 30, 66, ANY, "look village"),
AREA(3, 28, 45, 58, 100, ANY, "look rocks"),
AREA(3, 18, 118, 45, 140, ANY, "look rocks"),
AREA(3, 60, 3, 95, 95, ANY, "look tree"),
AREA(3, 0, 62, 35, 95, ANY, "look path"),
AREA(3, 35, 96, 75, 140, ANY, "look path"),
AREA(3, 75, 85, 140, 118, ANY, "look path"),
AREA(3, 50, 140, 70, 167, ANY, "look path"),
AREA(3, 95, 45, 159, 167, ANY, "look ferns"),
AREA(3, 0, 66, 159, 167, ANY, "look ferns"),

/* 4: castle gate (logic 4). Door = object 1 (view 2), guard dogs = 2, 3 (view 215). */
OBJ(4, 2, 215, NEAR(43, 86, 78, 101), "talk guard"),
OBJ(4, 3, 215, NEAR(43, 86, 78, 101), "talk guard"),
OBJ(4, 1, 2, ANY, "look door"),
AREA(4, 44, 38, 67, 84, ANY, "look door"),
AREA(4, 128, 80, 152, 150, ANY, "look hut"),
AREA(4, 40, 92, 120, 167, ANY, "look path"),
AREA(4, 0, 96, 40, 167, ANY, "look bushes"),
AREA(4, 88, 42, 115, 90, ANY, "look bushes"),
AREA(4, 0, 0, 150, 95, ANY, "look castle"),

/* 6: castle side wall (logic 6). Guidebook = object 1 (view 235), painted door = 2 (view 249); object 3
   is the paint cutscene. */
OBJ(6, 1, 235, NEAR(128, 94, 142, 111), "get book"),
OBJ(6, 2, 249, NEAR(48, 90, 83, 106), "open door"),
AREA(6, 45, 95, 95, 128, ANY, "look path"),
AREA(6, 80, 65, 125, 95, ANY, "look path"),
AREA(6, 0, 118, 80, 167, ANY, "look rocks"),
AREA(6, 0, 0, 115, 125, ANY, "look wall"),
AREA(6, 90, 60, 159, 167, ANY, "look bushes"),

/* 7: village street (logic 7). Pawn shop door = object 1, book shop door = 2 (view 4), beggar = 3 (view 5);
   the proclamation sign is added to the picture (view 41) once flag 94 is set. The book shop door's posn is
   the second one in the said(33,43) branch, so checkhotspots reports it although it is right. */
OBJ(7, 3, 5, DIST(3, 10), "talk beggar"),
OBJ(7, 1, 4, NEAR(114, 109, 125, 122), "open door"),
OBJ(7, 2, 4, NEAR(83, 99, 92, 115), "open door"),
AREA(7, 38, 66, 52, 84, NEAR(36, 78, 55, 100), "read sign"),
AREA(7, 100, 108, 124, 125, NEAR(95, 121, 104, 131), "look trash"),
AREA(7, 125, 111, 132, 130, ANY, "look chest"),
AREA(7, 15, 40, 38, 90, ANY, "look arch"),
AREA(7, 0, 0, 40, 30, ANY, "look trees"),
AREA(7, 68, 5, 85, 25, ANY, "look trees"),
AREA(7, 0, 100, 45, 167, ANY, "look trees"),
AREA(7, 0, 10, 159, 105, ANY, "look buildings"),

/* 8: pawn shop (logic 8, script-patched: typed talk/trade at the counter, findings 16). Door = object 1
   (view 4), owner = 2 (view 236), old man = 4 (view 8). "use counter" exists only in the patched logic:
   it opens the Talk / Trade / Take mint / Leave menu at the counter. */
OBJ(8, 2, 236, NEAR(92, 113, 100, 129), "use counter"),
OBJ(8, 4, 8, ANY, "look man"),
OBJ(8, 1, 4, NEAR(9, 103, 30, 123), "open door"),
AREA(8, 98, 100, 130, 128, ANY, "look counter"),
AREA(8, 45, 55, 75, 110, ANY, "look bear"),
AREA(8, 22, 55, 42, 110, ANY, "look skeleton"),
AREA(8, 30, 138, 53, 167, ANY, "look barrels"),
AREA(8, 0, 0, 25, 55, ANY, "look wings"),
AREA(8, 75, 55, 120, 100, ANY, "look shelves"),
AREA(8, 120, 40, 155, 115, ANY, "look shelves"),

/* 9: book shop (logic 9). Door = object 1 (view 4), book on the table = 2 (view 7), owner = 4 (view 233),
   Jollo = 5 (view 16), book on the counter = 8 (view 235). Love poems on the right-hand back shelf. */
OBJ(9, 4, 233, NEAR(130, 112, 149, 141), "talk owner"),
OBJ(9, 8, 235, NEAR(126, 107, 149, 141), "get spellbook"),
OBJ(9, 2, 7, NEAR(96, 111, 118, 128), "get book"),
OBJ(9, 5, 16, ANY, "look man"),
OBJ(9, 1, 4, NEAR(31, 109, 52, 127), "open door"),
AREA(9, 90, 113, 125, 125, ANY, "look table"),
AREA(9, 135, 112, 159, 145, ANY, "look counter"),
AREA(9, 122, 55, 159, 105, NEAR(126, 107, 159, 112), "look poems"),
AREA(9, 100, 50, 120, 110, ANY, "look fire"),
AREA(9, 50, 68, 72, 90, ANY, "look window"),
AREA(9, 53, 85, 100, 115, ANY, "look shelves"),
AREA(9, 80, 55, 100, 105, ANY, "look shelves"),

/* 10: village street by the house (logic 10). Gate = object 1, door = 2 (view 10), serving girl = 3
   (view 85), object 4 (view 231) appears with her. */
OBJ(10, 3, 85, NEAR(131, 130, 153, 155), "talk girl"),
OBJ(10, 4, 231, ANY, "look woman"),
OBJ(10, 2, 10, ANY, "look door"),
OBJ(10, 1, 10, ANY, "look gate"),
AREA(10, 57, 25, 80, 95, ANY, "look gate"),
AREA(10, 92, 20, 125, 38, ANY, "look windows"),
AREA(10, 80, 75, 115, 100, ANY, "look porch"),
AREA(10, 88, 5, 159, 90, ANY, "look house"),
AREA(10, 25, 128, 50, 150, ANY, "look rocks"),
AREA(10, 0, 0, 45, 145, ANY, "look trees"),
AREA(10, 45, 30, 80, 50, ANY, "look mountains"),
AREA(10, 80, 85, 159, 120, ANY, "look roses"),
AREA(10, 0, 98, 159, 167, ANY, "look path"),

/* 11: docks (logic 11). Cabin door = object 1 (view 12), swimming boy = 6 (view 14); objects 3-5 are
   waves, 9 and 10 belong to the boy cutscene. */
OBJ(11, 6, 14, NEAR(80, 65, 119, 117), "talk boy"),
OBJ(11, 1, 12, NEAR(10, 78, 21, 95), "open door"),
AREA(11, 0, 0, 60, 35, ANY, "look sail"),
AREA(11, 0, 35, 70, 160, ANY, "look boat"),
AREA(11, 55, 70, 120, 120, ANY, "look pier"),
AREA(11, 30, 20, 90, 45, ANY, "look mountains"),
AREA(11, 95, 30, 159, 90, ANY, "look path"),
AREA(11, 70, 90, 159, 167, ANY, "look ocean"),
