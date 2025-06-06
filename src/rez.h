#ifndef REZ_H
#define REZ_H

#include <stdbool.h>

#define COM_ST          (1)
#define COM_TT          (2)
#define COM_FALCON      (4)
#define COM_ALL_MCH     (COM_ST|COM_TT|COM_FALCON)
#define COM_ST_MONO     (8)
#define COM_ST_COLOR    (16)
#define COM_TT_MONO     (32)
#define COM_TT_VGA      (64)
#define COM_FALCON_VGA  (128)
#define COM_FALCON_TV   (256)
#define COM_MASK_MCH    (COM_ALL_MCH)
#define COM_MASK_MON    (COM_ST_MONO|COM_ST_COLOR|COM_TT_MONO|COM_TT_VGA|COM_FALCON_VGA|COM_FALCON_TV)
#define COM_MASK_COLOR  (COM_ST_COLOR|COM_TT_VGA|COM_FALCON_VGA|COM_FALCON_TV)

#define REZ_ST_LOW      (0)
#define REZ_ST_MED      (1)
#define REZ_ST_HI       (2)
#define REZ_FALCON      (3)
#define REZ_TT_MED      (4)
#define REZ_TT_HI       (6)
#define REZ_TT_LOW      (7)

/* Falcon mode flags */
#define M_IN            (1<<8)
#define M_LD            (1<<8)
#define M_ST            (1<<7)
#define M_OV            (1<<6)
#define M_PAL           (1<<5)
#define M_VGA           (1<<4)
#define M_80            (1<<3)
#define M_1P            (0)
#define M_2P            (1)
#define M_4P            (2)
/* we propogate these flags when changing mode */
#define M_FIXED         (M_PAL)
/* used to match the current rez with our defs */
#define M_MATCH         (M_IN|M_ST|M_OV|M_VGA|M_80)

struct rezdef
{
    /* descriptive name */
    const char *name;
    /* one of REZ_ */
    short rez;
    short console_lines;
    short console_cols;
    short num_colors;
    /* compatibility flags COM_ */
    short compat;
    /* Falcon mode flags M_ */
    short mode;
    /* vram_size for falcon modes */
    long vram_size;
    /* true if we could change to this */
    bool valid;
};

const struct rezdef *rez_current(void);
long rez_vdo(void);

#endif
