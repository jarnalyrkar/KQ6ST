#!/bin/sh
# Build a hard-disk game folder (no PP loader): mkhd.sh <source E.ST> <outdir> [heap_kb]
# Copies the game files out of the image, patches the heap (default 128 KB) and the save slots
# (99, scrolling list: patchslots.py), applies the script fixes (logicpatches.py), the vocabulary
# additions (wordpatches.py) and adds MG.PRG:
# on a hard disk the interpreter (from Mixed-Up Mother Goose, game ID "MG") checks 3 bytes at
# offset 62546 of "<current dir>\MG.prg"; on a floppy it checks A:\SIERRA.PRG instead.
# Save games already in the image (SG.n) are copied too. Saves go to the folder the game starts in.
set -e
T=$(cd "$(dirname "$0")" && pwd); TMP=$(mktemp -d)
python3 -I "$T/fat12.py" "$1" "$TMP" > /dev/null
mkdir -p "$2"
for f in LOGDIR PICDIR VIEWDIR SNDDIR VOL.0 VOL.1 WORDS.TOK OBJECT; do cp "$TMP/$f" "$2/"; done
for f in "$TMP"/SG.*; do [ -e "$f" ] && cp "$f" "$2/"; done
python3 -I "$T/patchheap.py" "$TMP/SIERRA.PRG" "$2/SIERRA.PRG" ${3:-128} 2>/dev/null || cp "$TMP/AUTO/SIERRA.PRG" "$2/SIERRA.PRG"
python3 -I "$T/patchslots.py" "$2/SIERRA.PRG" "$2/SIERRA.PRG"
cp "$2/SIERRA.PRG" "$2/MG.PRG"
python3 -I "$T/logicpatches.py" "$TMP" "$2"
python3 -I "$T/wordpatches.py" "$TMP" "$2"
rm -rf "$TMP"; ls -l "$2"
