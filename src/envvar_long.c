#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "internal.h"

int
xyz_envvar_long(const char *name, long *v)
{
    if (name == NULL || *name == NULLCH || v == NULL)
        return XYZ_ERR;

    *v = 0;
    char *p = NULL;

    if ((p = getenv(name)) == NULL)
        return XYZ_ERR;

    *v = strtol(p, NULL, 10);

    if ((*v == LONG_MIN || *v == LONG_MAX) && errno == ERANGE) {
        *v = 0;
        return XYZ_ERR;
    }

    return XYZ_OK;
}
