/* Heap release for Sierra ST AGI 1.422 (replaces text 0x4ed4, which only did heap_top = p).
   The heap is a stack: discard.view / discard.pic / the end of a call() free everything above the
   discarded resource. The interpreter cuts the view (or pic, logic) list there, but not the sound
   list, so a sound loaded after a view that is discarded later stays linked in freed memory; the next
   load.sound of a new number walks into whatever was drawn there and bus-errors (KQ6 room 20: flute =
   load.view(40) load.sound(18), logic 90 discard.view(40); then "give milk" loads sound 62).
   Sound nodes are appended in address order, so the list is cut at the first node at or above p.
   A sound still playing from freed memory is stopped first (sets its done-flag, as when it ends). */
typedef short W;
struct snd { struct snd *next; W num; };
struct chan { char *data; char rest[10]; };    /* 14 bytes per channel, 4 channels */

extern char *heap_top;
extern struct snd *snd_list;
extern volatile W snd_playing;
extern struct chan snd_chan[4];
extern void stop_sound(void);

void heap_release(char *p)
{
    struct snd **l;

    /* written out: a loop over snd_chan crashes gcc 15 -mshort (ivopts) */
    if (snd_playing && (snd_chan[0].data >= p || snd_chan[1].data >= p ||
                        snd_chan[2].data >= p || snd_chan[3].data >= p))
        stop_sound();
    heap_top = p;
    for (l = &snd_list; *l; l = &(*l)->next)
        if ((char *)*l >= p) { *l = 0; break; }
}
