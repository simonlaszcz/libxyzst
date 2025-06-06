#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "internal.h"

int
xyz_envvar_bool(const char *name, bool *v)
{
    static char *affirmative = "YyTt1";

    if (name == NULL || *name == NULLCH || v == NULL)
        return XYZ_ERR;

    *v = false;
    char *p = NULL;

    if ((p = getenv(name)) == NULL)
        return XYZ_ERR;

    while (isspace(*p)) ++p;

    if (strrchr(affirmative, *p) != NULL)
        *v = true;

    return XYZ_OK;
}
