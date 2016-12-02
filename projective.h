#include <X11/Xlib.h>

#include "xplot.h"

#define TargetDim 2

int applyAffine (xpdata_t* res,
                 xpdata_t* pnt, const size_t nDim, xpdata_t* tAffine);

int projectToX (xpdata_t* pnts,
                xpdata_t* vals, const size_t nDim, const size_t nCnt,
                xpdata_t* tAffine);

int perspective (XPoint* pnts,
                 xpdata_t* vals, const size_t nDim, const size_t nCnt,
                 xpdata_t offset, size_t xo, size_t yo);
