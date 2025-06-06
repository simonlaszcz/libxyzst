#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <mint/falcon.h>
#include "rez.h"
#include "internal.h"

static long get_ncolors_for_mode(int mode);

struct xyz_palette *
xyz_save_palette(short count)
{
    struct xyz_palette *p = NULL;
    size_t sz = sizeof(*p);

    if (rez_vdo() == VDO_FALCON) {
        int max = get_ncolors_for_mode(VsetMode(-1));

        if (count < 1 || count > max)
            count = max;

        sz += sizeof(__int32_t) * count;

        if ((p = malloc(sz)) == NULL)
            return NULL;

        memset(p, 0, sz);
        p->count = count;
        VgetRGB(0, count, (__int32_t *)p->data);

        return p;
    }

    count = 16;
    sz += sizeof(__int16_t) * count;

    if ((p = malloc(sz)) == NULL)
        return NULL;

    memset(p, 0, sz);
    p->count = count;

    for (int i = 0; i < count; ++i)
        ((__int16_t *)p->data)[i] = Setcolor(i, COL_INQUIRE);

    return p;
}

static long
get_ncolors_for_mode(int mode)
{
    switch (mode & 0x7) {
    case 0:     return 2;
    case 1:     return 4;
    case 2:     return 16;
    case 3:     return 256;
    default:    return 65536;
    }
}
