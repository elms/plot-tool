#include <stdlib.h>
#include <math.h>

#include "projective.h"

int applyAffine(xpdata_t* res,
                xpdata_t* pnt, const size_t nDim, xpdata_t* tAffine) {

  xpdata_t* tmp = malloc(sizeof(xpdata_t) * nDim);

  for (size_t ii=0; ii < nDim; ii++) {
    tmp[ii] = pnt[ii] + tAffine[ii];
  }

  for (size_t ii=0; ii < nDim; ii++) {
    res[ii] = 0;
    for (size_t jj=0; jj < nDim; jj++) {
      // ii+1 to step over the offset in the affine transform
      res[ii] += tmp[jj] * tAffine[jj + (ii+1) * nDim];
    }
  }

  free(tmp);
  return 0;
}

int projectToX(xpdata_t* pnts,
               xpdata_t* vals, const size_t nDim, const size_t nCnt,
               xpdata_t* tAffine) {

  for (size_t ii=0; ii<nCnt; ii++) {
    applyAffine(pnts + ii*nDim, vals + ii*nDim, nDim, tAffine);
    //printf("%f %f %f\t %f\n", pnts[ii*nDim + 0], pnts[ii*nDim + 1], pnts[ii*nDim + 2],
    //       sqrt(pnts[ii*nDim + 0]*pnts[ii*nDim + 0] + pnts[ii*nDim + 1]*pnts[ii*nDim + 1] +  pnts[ii*nDim + 2]*pnts[ii*nDim + 2]));
  }

  return 0;
}

int perspective(XPoint* pnts,
                xpdata_t* vals, const size_t nDim, const size_t nCnt,
                xpdata_t offset, size_t xo, size_t yo) {

  xpdata_t* tmp = malloc(sizeof(xpdata_t) * TargetDim);

  for (size_t ii=0; ii<nCnt; ii++) {
    tmp[0] = 100*offset*(vals[ii*nDim + 0])/(vals[ii*nDim + 2] + offset);
    tmp[1] = 100*offset*(vals[ii*nDim + 1])/(vals[ii*nDim + 2] + offset);

    pnts[ii].x = round(tmp[0]) + xo;
    pnts[ii].y = round(tmp[1]) + yo;

    //printf("pnt: %f %f %d %d\n", tmp[0], tmp[1], pnts[ii].x, pnts[ii].y);
  }

  free(tmp);
  return 0;
}
