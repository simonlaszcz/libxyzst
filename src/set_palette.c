#include <sys/types.h>
#include <mint/falcon.h>
#include "rez.h"
#include "internal.h"

int
xyz_set_palette(struct xyz_palette *p)
{
    if (p == NULL)
        return XYZ_ERR;

    if (rez_vdo() == VDO_FALCON)
        VsetRGB(0, p->count, p->data);
    else
        Setpalette(p->data);
    
    return XYZ_OK;
}
