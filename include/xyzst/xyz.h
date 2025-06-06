#ifndef XYZ_H
#define XYZ_H

#include <stdbool.h>

#define XYZ_ERR	(0)
#define XYZ_OK	(1)

enum xyz_os_type
{
    XYZ_OS_TOS = 0,
    XYZ_OS_MINT,
    XYZ_OS_MAGIC,
    XYZ_OS_GENEVA
};

struct xyz_os_info
{
    enum xyz_os_type	type;
    float				version;
};

struct xyz_con_info
{
    short lines;
    short cols;
    short num_colors;
};

struct xyz_palette
{
    int		count;
    char	data[];
};

/**
 * GENERAL:
 * All functions returning int will return XYZ_OK on success, XYZ_ERR on error
 * Functions returning a pointer, will return NULL on error.
 */

/**
 * Get info about the installed OS. If TOS, we are not multitasking.
 */
int xyz_get_os_info(struct xyz_os_info *);

/**
 * Get info about the console by checking the environment vars (LINES/ROWS, COLS/COLUMNS)
 * and the resolution.
 * n.b. It does not call ioctl() so may not be accurate under MiNT.
 */
int xyz_get_con_info(struct xyz_con_info *);

/**
 * Gets the specified environment var and sets v to it's value.
 */
int xyz_envvar_long(const char *name, long *v);

/**
 * Gets the specified environment var and attempts to convert
 * it to bool. It checks the first non-space character and treats
 * [YyTt1] as true.
 */
int xyz_envvar_bool(const char *name, bool *v);

/**
 * Tests the specified env var for truthiness and returns the value.
 * If the env var is not found, it returns defval instead.
 */
bool xyz_envvar_true(const char *name, bool defval);

/**
 * Save the palette. On Falcon, count specifies the number of entries to save.
 * On other machines, count is ignored and 16 is used.
 * This is malloc'd and must be freed by the caller.
 */
struct xyz_palette *xyz_save_palette(short count);

/**
 * Set the palette
 */
int xyz_set_palette(struct xyz_palette *p);

/**
 * Swap two active palette entries. Upper bounds not checked
 */
int xyz_swap_palette_entries(long a, long b);

#endif
