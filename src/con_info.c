#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "internal.h"
#include "rez.h"

int
xyz_get_con_info(struct xyz_con_info *coni)
{
    if (coni == NULL)
        return XYZ_ERR;

    memset(coni, 0, sizeof(*coni));
    const struct rezdef *rez = NULL;
    if ((rez = rez_current()) == NULL)
        return XYZ_ERR;

    coni->num_colors = rez->num_colors;

    long lines = 0;
    if (xyz_envvar_long("LINES", &lines) == XYZ_ERR)
        if (xyz_envvar_long("ROWS", &lines) == XYZ_ERR)
            lines = 0;
    if (lines < 1 || lines > SHRT_MAX)
        lines = rez->console_lines;
    coni->lines = (short)lines;

    long cols = 0;
    if (xyz_envvar_long("COLS", &cols) == XYZ_ERR)
        if (xyz_envvar_long("COLUMNS", &cols) == XYZ_ERR)
            cols = 0;
    if (cols < 1 || cols > SHRT_MAX)
        cols = rez->console_cols;
    coni->cols = (short)cols;

    return XYZ_OK;
}
