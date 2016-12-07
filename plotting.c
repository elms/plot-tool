#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <math.h>

#include "xplot.h"
#include "plotting.h"

static void sec_extremum(xpdata_t* data, size_t cnt,
                         xpdata_t* pmin, xpdata_t* pmax, xpdata_t* pavg) {
  xpdata_t max = data[1];
  xpdata_t min = data[1];
  xpdata_t avg = 0;

  for (int ii=1; ii<cnt; ii++) {
    max = fmax(data[2*ii + 1], max);
    min = fmin(data[2*ii + 1], min);
    avg += data[2*ii + 1]/cnt;
  }

  if (NULL != pmin) {
    *pmin = min;
  }

  if (NULL != pmax) {
    *pmax = max;
  }

  if (NULL != pavg) {
    *pavg = avg;
  }

  return;
}

int plotmax(XPoint* xpnt, xpdata_t* data, size_t npnts,
            xpgeo_t* geo, xpdata_t ysc, xpdata_t yoff,
            size_t* xnpnts) {

  xp_decimate_t dec_type = XP_MAX;
  xpdata_t pnt_per_pixel = (double)(npnts-1)/(double)geo->width;

  xpdata_t* transd = calloc(npnts * 2, sizeof(xpdata_t));

  printf("%s: %g\n", __func__, pnt_per_pixel);

  // decimate
  if (npnts < geo->width) {
    printf("%s: %zu\n", __func__, npnts);
    // plot all points, data scaled
    *xnpnts = npnts;
    for (size_t ii=0; ii<npnts; ii++) {
      transd[2*ii + 0] = ii/pnt_per_pixel;
      transd[2*ii + 1] = data[2*ii + 1];
    }
  } else {
    *xnpnts = geo->width;
    xpdata_t step = pnt_per_pixel;
    xpdata_t off = 0;
    xpdata_t sec_max;
    xpdata_t sec_min;
    xpdata_t sec_avg;

    printf("%s: s %zu %g %g\n", __func__, *xnpnts, step, off);
    fflush(stdout);

    for (int ii=0; ii<geo->width; ii++) {
      transd[2*ii + 0] = ii;
      sec_extremum(data + 2*(size_t)round(off), round(off+step) - round(off),
                   &sec_min, &sec_max, &sec_avg);

      xpdata_t yval = data[2*(size_t)round(off) + 1];
      switch (dec_type) {
        case XP_MAX:
          yval = sec_max;
          break;
        case XP_MIN:
          yval = sec_min;
          break;
        case XP_AVG:
          yval = sec_avg;
          break;
      }

      off += step;

      transd[2*ii + 1] = yval;
    }
  }

  // auto-scale
  if (ysc <= 0) {
    xpdata_t ymax;
    xpdata_t ymin;
    sec_extremum(transd, *xnpnts, &ymin, &ymax, NULL);
    ysc = -1 * ((xpdata_t)geo->height) / (ymax - ymin);
    yoff = ysc * -ymin + geo->height;
  }

  // scale, offset, and round to XPoints
  for (size_t ii=0; ii<*xnpnts; ii++) {
    xpnt[ii].x = round(transd[2*ii + 0]);
    xpnt[ii].y = round(ysc * transd[2*ii + 1] + yoff);
  }

  free(transd);

  return 0;
}
