#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"
#include <bitset>
#include <stdint.h>

class PREDICTOR{
 private:
  bitset<70> lhr;
  bitset<52> ghr;
  UINT32 sum;
  UINT32 table[512][123];
 public:
  PREDICTOR(void);
  // UINT32  divide(bitset<122> n,int m);
  // UINT32  divide2(bitset<64> n,int m);
  // UINT32  HashPC(UINT32 PC);
  // UINT32  HashPCl(UINT32 PC);
  UINT32  index[123];
  bool    GetPrediction(UINT32 PC);
  void    UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget);
  void    TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget);
};
#endif