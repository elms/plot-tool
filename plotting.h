#ifndef _PLOTTING_H_
#define _PLOTTING_H_

#include "xplot.h"

int plotmax(XPoint* xpnt, xpdata_t* data, size_t npnts,
            xpgeo_t* geo, xpdata_t ysc, xpdata_t yoff,
            size_t* xnpnts);

#endif
