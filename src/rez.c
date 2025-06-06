#include <osbind.h>
#include <stdbool.h>
#include <string.h>
#include <mint/cookie.h>
#include <mint/falcon.h>
#include <sys/types.h>
#include "rez.h"
#include "internal.h"

static struct rezdef
rezdefs[] = 
{
    {"st-low",  REZ_ST_LOW, 25, 40, 16, COM_ALL_MCH|COM_ST_COLOR|COM_TT_VGA|COM_FALCON_VGA, 0, 32000, 0},
    {"st-med",  REZ_ST_MED, 25, 80, 4,  COM_ALL_MCH|COM_ST_COLOR|COM_TT_VGA|COM_FALCON_VGA, 0, 32000, 0},
    {"st-hi",   REZ_ST_HI,  25, 80, 2,  COM_ALL_MCH|COM_ST_MONO |COM_TT_VGA|COM_FALCON_VGA, 0, 32000, 0},
    {"tt-low",  REZ_TT_LOW, 30, 40, 16, COM_TT|COM_TT_VGA,  0, 153600, 0},
    {"tt-med",  REZ_TT_MED, 30, 80, 16, COM_TT|COM_TT_VGA,  0, 153600, 0},
    {"tt-hi",   REZ_TT_HI,  60, 160,2,  COM_TT|COM_TT_MONO, 0, 153600, 0},
    {"320x200", REZ_FALCON, 25, 40, 16, COM_FALCON|COM_ST_COLOR,        M_4P, 0, 0},
    {"384x240", REZ_FALCON, 30, 48, 16, COM_FALCON|COM_FALCON_TV,       M_OV|M_4P, 0, 0},
    {"320x400", REZ_FALCON, 25, 40, 16, COM_FALCON|COM_ST_COLOR,        M_IN|M_4P, 0, 0},
    {"384x480", REZ_FALCON, 30, 48, 16, COM_FALCON|COM_FALCON_TV,       M_OV|M_IN|M_4P, 0, 0},
    {"320x480", REZ_FALCON, 30, 40, 16, COM_FALCON|COM_FALCON_VGA,      M_VGA|M_4P, 0, 0},
    {"320x240", REZ_FALCON, 30, 40, 16, COM_FALCON|COM_FALCON_VGA,      M_LD|M_VGA|M_4P, 0, 0},
    {"640x200", REZ_FALCON, 25, 80, 16, COM_FALCON|COM_ST_COLOR,        M_80|M_4P, 0, 0},
    {"768x240", REZ_FALCON, 30, 96, 16, COM_FALCON|COM_FALCON_TV,       M_OV|M_80|M_4P, 0, 0},
    {"640x400", REZ_FALCON, 25, 80, 16, COM_FALCON|COM_ST_COLOR,        M_80|M_IN|M_4P, 0, 0},
    {"768x480", REZ_FALCON, 30, 96, 16, COM_FALCON|COM_FALCON_TV,       M_OV|M_80|M_IN|M_4P, 0, 0},
    {"640x480", REZ_FALCON, 30, 80, 16, COM_FALCON|COM_FALCON_VGA,      M_80|M_VGA|M_4P, 0, 0},
    {"640x240", REZ_FALCON, 30, 80, 16, COM_FALCON|COM_FALCON_VGA,      M_80|M_LD|M_VGA|M_4P, 0, 0},
    {NULL}
},
/* used when we can't match the startup rez. avoids a null pointer */
rezinvalid = {"unknown",  REZ_ST_LOW, 25, 40, 0, 0, 0, 0, 0};

static void init(void);
static void validate_rezdefs(void);
static struct rezdef *match_rezdef(int rez, int mode);

static bool was_initialised = false;
static struct
{
    /* from cookie _VDO */
    long    vdo;
    /* display compatibility flags */
    int     compat;
    int     startup_rez;
    int     startup_mode;
    /* bits from the startup Falcon mode that we propagate */
    int     mode_fixed;
    /* max bytes required for all compatible display modes */
    int     max_vram_size;
    /* current rez */
    struct rezdef *rezdef;
} m;

const struct rezdef *
rez_current(void)
{
    if (!was_initialised)
        init();
    return m.rezdef;
}

long
rez_vdo(void)
{
    if (!was_initialised)
        init();
    return m.vdo;
}

static void
init(void)
{
    if (was_initialised)
        return;

    memset(&m, 0, sizeof(m));
    was_initialised = true;

    m.rezdef = &rezinvalid;

    if (Getcookie(C__VDO, &m.vdo) == C_NOTFOUND)
        m.vdo = VDO_ST;
    else
        m.vdo >>= 16;

    m.startup_rez = Getrez();

    /* set some flags that determine which modes are valid */
    switch (m.vdo) {
    case VDO_FALCON:
        m.startup_mode = VsetMode(-1);
        m.mode_fixed = m.startup_mode & M_FIXED;
        m.compat = COM_FALCON;
        switch (VgetMonitor()) {
        case 0:
            m.compat |= COM_ST_MONO;
            break;
        case 1:
            m.compat |= COM_ST_COLOR;
            break;
        case 3:
            m.compat |= (COM_ST_COLOR | COM_FALCON_TV);
            break;
        default:
            m.compat |= COM_FALCON_VGA;
            break;
        }
        break;
    case VDO_TT:
        m.compat = COM_TT;
        m.compat |= (m.startup_rez == REZ_TT_HI ? COM_TT_MONO : COM_TT_VGA);
        break;
    default:
        m.compat = COM_ST;
        m.compat |= (m.startup_rez == REZ_ST_HI ? COM_ST_MONO : COM_ST_COLOR);
        break;
    }

    validate_rezdefs();
    m.rezdef = match_rezdef(m.startup_rez, m.startup_mode);
}

static void
validate_rezdefs(void)
{
    m.max_vram_size = 0;

    for (int i = 0; rezdefs[i].name != NULL; ++i) {
        struct rezdef *r = &rezdefs[i];
        r->valid = false;

        if ((r->compat & COM_MASK_MCH) & (m.compat & COM_MASK_MCH))
            if ((r->compat & COM_MASK_MON) & (m.compat & COM_MASK_MON))
                r->valid = true;

        if (r->valid && r->mode > 0 && m.vdo == VDO_FALCON) {
            /* propogate these flags (e.m. PAL/NTSC) to all modes */
            r->mode |= m.mode_fixed;
            r->vram_size = VgetSize(r->mode);

            if (r->vram_size > m.max_vram_size)
                m.max_vram_size = r->vram_size;
        }
    }
}

static struct rezdef *
match_rezdef(int rez, int mode)
{
    /*  When matching Falcon modes, ignore the number of color planes.
        We'll switch to 16 colors when palvim() is called */
    if (m.vdo == VDO_FALCON && mode > 0) {
        int my_mode = mode & M_MATCH;

        for (int i = 0; rezdefs[i].name != NULL; ++i) {
            struct rezdef *r = &rezdefs[i];

            if (!(r->valid && r->mode > 0))
                continue;
            if (my_mode == (r->mode & M_MATCH))
                return r;
        }
    }

    for (int i = 0; rezdefs[i].name != NULL; ++i) {
        struct rezdef *r = &rezdefs[i];

        if (!r->valid)
            continue;
        if (r->rez == rez)
            return r;
    }

    return &rezinvalid;
}
