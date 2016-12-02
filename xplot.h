#ifndef _XPLOT_H_
#define  _XPLOT_H_

#include <stdlib.h>

typedef float xpdata_t;

typedef struct {
  size_t width;
  size_t height;
} xpgeo_t;

typedef enum {
  XP_NONE,
  XP_MIN,
  XP_MAX,
  XP_AVG
} xp_decimate_t;

#endif
