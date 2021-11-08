#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"
#include <bitset>
#include <stdint.h>

class PREDICTOR{
 private:
  bitset<122> ghr;
  UINT32 s;
  UINT32 table[512][123];
 public:
  PREDICTOR(void);
  UINT32  divide(bitset<122> n,int m);
  UINT32  HashPC(UINT32 PC);
  bool    GetPrediction(UINT32 PC);
  void    UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget);
  void    TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget);
};
#endif