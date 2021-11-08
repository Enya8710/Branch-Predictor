#include "predictor.h"
#include <bitset>
#include <stdint.h>
#include <cmath>
/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
//My version
// Total storage budget: 64KB ~ 512000bits
// Total bits in history register:  122 bits
// Total number of weights per perceptron:  122 + 1 = 123
// Total bits to store weight: 8
// Total number of perceptrons: 512
// Threshold = 1.93*h + 14 = 1.93*122 + 14 = 207 249.46 ~ 250
// Total perceptron table size = 512*8*123= 503,808 bits
// Total perceptron steps table size = 512 * 8 = 4,096 bits
// Total Size = perceptron table size + history register size + perceptron steps table = 503,808 + 4,096 + 122 = 508,026 bits
/////////////////////////////////////////////////////////////
UINT32 divide(bitset<122> n,int m){
    // calculate Remainder by
    // n AND (m-1)
    bitset<122> bs(m-1);
    UINT32 res = ((n)&(bs)).to_ulong();
    return res;
}
UINT32 PREDICTOR::HashPC(UINT32 PC){
	UINT32 PCend = PC % 512;
	UINT32 ghrend = divide(ghr,512);
	return PCend ^ ghrend;
}

PREDICTOR::PREDICTOR(void){
  s = 0;
  ghr = bitset<122>();
  for(UINT32 i=0; i < 512; i++){
    for(UINT32 j=0; j < 122; j++){
	    table[i][j] = 0;                
    }
  }
}

bool   PREDICTOR::GetPrediction(UINT32 PC){
  UINT32 index = HashPC(PC);
  INT32 p = 0;
  p += table[index][0];
  for(UINT32 i=1; i < 123; i++){
    if(ghr[i - 1] == 1){            //if taken
	    p += table[index][i];          // add weight
    }
    else{
	    p -= table[index][i];          // subtract weight
    }
  }
  s = abs(p);
  if(p >= 0){
    return TAKEN;                 //greater than zero predtict taken
  }
  else{
    return NOT_TAKEN;           //less than zero predict not taken
  }
}

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){
  UINT32 index = HashPC(PC);
  if(resolveDir != predDir || s <= 249){ //update if less than Threshold
    if(resolveDir == TAKEN){
	    int32_t updateVal = table[index][0] + 1;
	    if (updateVal > 250){
		    table[index][0] = 250;
	    }
	    else{
		    table[index][0]++;
	    }
	  }
    else{
	    int32_t updateVal = table[index][0] - 1;
	    if (updateVal < -250){
		    table[index][0] = -250;
	    }
	    else{
		    table[index][0]--;
	    }
	  }
	  for(UINT32 i = 1; i < 123; i++){
      if((resolveDir == TAKEN && ghr[i - 1] == 1) || (resolveDir == NOT_TAKEN && ghr[i - 1] == 0)){
		    int32_t updateVal = table[index][i] + 1;
	      if (updateVal > 249){
		      table[index][i] = 249;
	      }
	      else{
			    table[index][i]++;
		    }
      }
	    else{
	      int32_t updateVal = table[index][i] - 1;
	      if (updateVal < -249){
		      table[index][i] = -249;
	      }
		    else{
			    table[index][i]--;
		    }
	    }
	  }
  }
  ghr = (ghr << 1);
  if(resolveDir == TAKEN){
    ghr.set(0, 1);
  }
  else{
	  ghr.set(0, 0);
  }
}

void    PREDICTOR::TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget){

  // This function is called for instructions which are not
  // conditional branches, just in case someone decides to design
  // a predictor that uses information from such instructions.
  // We expect most contestants to leave this function untouched.

  return;
}

