#!/bin/sh
# Package a playable game folder for a GitHub release: tools/mkrelease.sh <E.ST> VERSION
# Builds a fresh folder from PP's unmodified E.ST with mkhd.sh (never from the play folder with its
# saves) -> dist/KQ6ST-VERSION.zip with KQ6ST/ (the game folder) and READ_ME.TXT.
set -e
T=$(cd "$(dirname "$0")" && pwd)
IMG=${1:?usage: tools/mkrelease.sh E.ST VERSION}; VER=${2:?usage: tools/mkrelease.sh E.ST VERSION}
OUT=$T/../dist/KQ6ST-$VER
rm -rf "$OUT" "$OUT.zip"
"$T/mkhd.sh" "$IMG" "$OUT/KQ6ST" > /dev/null
rm -f "$OUT"/KQ6ST/SG.* "$OUT"/KQ6ST/sg.*
sed "s/@VERSION@/$VER/" "$T/READ_ME.TXT" | sed 's/$/\r/' > "$OUT/READ_ME.TXT"
(cd "$OUT" && zip -qr -X "../KQ6ST-$VER.zip" READ_ME.TXT KQ6ST)
ls -l "$OUT.zip"
