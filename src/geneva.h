#ifndef GENEVA_H
#define GENEVA_H

#include <sys/types.h>

typedef struct G_vectors        /* Release 004 */
{
  __int16_t used;
  __int16_t (*keypress)( __int32_t *key );
  __int16_t (*app_switch)( __int8_t *process_name, __int16_t apid );
  __int16_t (*gen_event)(void);
} G_VECTORS;

typedef struct
{
  __int16_t ver;
  __int8_t *process_name;
  __int16_t apid;
  __int16_t (**aes_funcs)();
  __int16_t (**xaes_funcs)();
  struct G_vectors *vectors;    /* Release 004 */
} G_COOKIE;

#endif
