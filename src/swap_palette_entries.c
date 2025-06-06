#include <mint/falcon.h>
#include <sys/types.h>
#include <osbind.h>
#include "rez.h"
#include "internal.h"

int
xyz_swap_palette_entries(long a, long b)
{
    if (a < 0 || b < 0)
        return XYZ_ERR;

    if (rez_vdo() == VDO_FALCON) {
        __int32_t t32a, t32b;
        VgetRGB(a, 1, &t32a);
        VgetRGB(b, 1, &t32b);
        VsetRGB(a, 1, &t32b);
        VsetRGB(b, 1, &t32a);
    }
    else {
        __int16_t t16 = Setcolor(a, COL_INQUIRE);
        Setcolor(a, Setcolor(b, COL_INQUIRE));
        Setcolor(b, t16);
    }
    
    return XYZ_OK;
}
