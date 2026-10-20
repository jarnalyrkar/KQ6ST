#!/bin/sh
# Headless Hatari run driven by an hconsole script.  hc.sh <script> <disk.st>
# Or hc.sh <script> -hd <dir> <autostart>  (GEMDOS hard disk, e.g. PP's loader: -hd KQVIST C:\RUNME2.TOS)
# Or hc.sh <script> -acsi <hd.img> <autostart>  (ACSI image with a real driver, as on real hardware; Hatari's
#   GEMDOS drive is turned off. Make the image as in PROJECT.md "Testing like real hardware".)
# Output (screenshots, hc.log) goes to work/hatari/. Extra script commands:
#   shot NAME   -> screenshot work/hatari/NAME.png
#   type TEXT   -> one keypress per character, 0.15 s apart (hconsole "text" is too fast; the game drops keys)
# Plain hconsole commands pass through: keypress 28 (Return), keypress 77 (right arrow), sleep N,
# setopt --fast-forward on/off, parse FILE (debugger commands, e.g. memdump).
# @HATARI@ in a script line is replaced by this folder (e.g. parse @HATARI@/bomb.ini).
# Env: HLOG=warn (Hatari log level), HEXTRA="--trace gemdos" (extra Hatari args),
#      HCONSOLE=path to Hatari's hconsole.py.
# Waits: ~3 s after each Return, ~12 s after a room change (even with --fastfdc).
T=$(cd "$(dirname "$0")" && pwd); W=$T/../../work/hatari; mkdir -p "$W"; W=$(cd "$W" && pwd)
IN=$1
CFG=
if [ "$2" = -hd ]; then MEDIA="-d $(readlink -f "$3") --auto $4 --memsize 4096"
elif [ "$2" = -acsi ]; then MEDIA="--acsi 0=$(readlink -f "$3") --auto $4 --memsize 4096"
  CFG=$W/acsi.cfg; sed 's/^bUseHardDiskDirectory = TRUE/bUseHardDiskDirectory = FALSE/' \
    "${HATARI_CFG:-$HOME/.config/hatari/hatari.cfg}" > "$CFG"; CFG="--configfile $CFG"
else MEDIA="--memsize 1024 --disk-a $(readlink -f "$2") --protect-floppy on --fastfdc on"; fi
OUT=$W/hc_run.txt; : > "$OUT"
while IFS= read -r l; do
  case "$l" in
    type\ *) t=${l#type }; i=0; while [ $i -lt ${#t} ]; do c=$(printf '%s' "$t" | cut -c$((i+1)));
             if [ "$c" = " " ]; then echo "keypress 57"; else echo "keypress $c"; fi; echo "sleep 0.15"; i=$((i+1)); done >> "$OUT";;
    shot\ *) n=${l#shot }; printf 'screenshot %s/%s.png\n' "$W" "$n" > "$W/shot_$n.ini"; echo "parse $W/shot_$n.ini" >> "$OUT";;
    *) printf '%s\n' "$l" | sed "s|@HATARI@|$T|g" >> "$OUT";;
  esac
done < "$IN"
echo kill >> "$OUT"
cd "$W"; SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 300 python3 \
  "${HCONSOLE:-$HOME/.local/opt/hatari-pkg/extracted/usr/share/hatari/hconsole/hconsole.py}" "$OUT" --exit -- \
  $CFG --confirm-quit no --machine ste $MEDIA \
  --sound off --log-level ${HLOG:-error} $HEXTRA < /dev/null > "$W/hc.log" 2>&1
