#include <stdbool.h>
#include "internal.h"

bool
xyz_envvar_true(const char *name, bool defval)
{
    bool v;

    if (xyz_envvar_bool(name, &v) == XYZ_OK)
        return v;

    return defval;
}
