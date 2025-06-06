#include <stdbool.h>
#include <string.h>
#include <mint/cookie.h>
#include "internal.h"
#include "magic.h"
#include "geneva.h"

static float get_magic_version(void);
static float get_mint_version(void);
static float get_geneva_version(void);
static float decode_bcd(int v);

int
xyz_get_os_info(struct xyz_os_info *osi)
{
    if (osi == NULL)
        return XYZ_ERR;

    memset(osi, 0, sizeof(*osi));

    if ((osi->version = get_magic_version()) > 0) {
        osi->type = XYZ_OS_MAGIC;
        return XYZ_OK;
    }

    if ((osi->version = get_mint_version()) > 0) {
        osi->type = XYZ_OS_MINT;
        return XYZ_OK;
    }

    if ((osi->version = get_geneva_version()) > 0) {
        osi->type = XYZ_OS_GENEVA;
        return XYZ_OK;
    }

    osi->type = XYZ_OS_TOS;
    osi->version = 0;

    return XYZ_OK;
}

static float
get_magic_version(void)
{
    MAGX_COOKIE *cv;

    if (Getcookie(C_MagX, (long *)&cv) == C_NOTFOUND)
        return 0;
    /* if aesvars is null, we were started from AUTO and aes is not ready */
    if (cv->aesvars == NULL)
        return -1;

    return decode_bcd(cv->aesvars->version);
}

static float
get_mint_version(void)
{
    long v;

    if (Getcookie(C_MiNT, &v) == C_NOTFOUND)
        return 0;

    return decode_bcd(v);
}

static float
get_geneva_version(void)
{
    /* cookie value points to a struct where the first member is the version */
    G_COOKIE *cv;

    if (Getcookie(C_Gnva, (long *)&cv) == C_NOTFOUND)
        return 0;
    if (cv == NULL)
        return 0;

    return decode_bcd(cv->ver);
}

static float
decode_bcd(int v)
{
    float major = (v >> 8) & 0xFF;
    float minor = v & 0xFF;

    while (minor > 0)
        minor /= 10.0;

    return major + minor;
}
