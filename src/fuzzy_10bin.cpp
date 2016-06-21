#include "fuzzy_10bin.h"
#include <opencv2/core/core.hpp>
#include <math.h>

using namespace ACEDD_DESC;

static const double HueThresholdValues[] = {
	      0,   0,   5,  10,
	      5,  10,  35,  50,
	     35,  50,  70,  85,
	     70,  85, 150, 165,
	    150, 165, 195, 205,
	    195, 205, 265, 280,
	    265, 280, 315, 330,
	    315, 330, 360, 360
	};

const double SaturationThresholdValues[] = {
		 0.f/255.f,  0.f/255.f,  10.f/255.f,  75.f/255.f,
		10.f/255.f, 75.f/255.f, 255.f/255.f, 255.f/255.f
	};

const double ValueThresholdValues[] = {
		  0.f/255.f,   0.f/255.f,  10.f/255.f,  75.f/255.f,
		 10.f/255.f,  75.f/255.f, 180.f/255.f, 220.f/255.f,
		180.f/255.f, 220.f/255.f, 255.f/255.f, 255.f/255.f
	};

const int BinRulesDefinition[][4] = {
	    {0,0,0,2}, {0,1,0,2}, {0,0,2,0}, {0,0,1,1}, {1,0,0,2}, {1,1,0,2},
	    {1,0,2,0}, {1,0,1,1}, {2,0,0,2}, {2,1,0,2}, {2,0,2,0}, {2,0,1,1},
	    {3,0,0,2}, {3,1,0,2}, {3,0,2,0}, {3,0,1,1}, {4,0,0,2}, {4,1,0,2},
	    {4,0,2,0}, {4,0,1,1}, {5,0,0,2}, {5,1,0,2}, {5,0,2,0}, {5,0,1,1},
	    {6,0,0,2}, {6,1,0,2}, {6,0,2,0}, {6,0,1,1}, {7,0,0,2}, {7,1,0,2},
	    {7,0,2,0}, {7,0,1,1}, {0,1,1,3}, {0,1,2,3}, {1,1,1,4}, {1,1,2,4},
	    {2,1,1,5}, {2,1,2,5}, {3,1,1,6}, {3,1,2,6}, {4,1,1,7}, {4,1,2,7},
	    {5,1,1,8}, {5,1,2,8}, {6,1,1,9}, {6,1,2,9}, {7,1,1,3}, {7,1,2,3}
	};
	
 void Fuzzy10BIN::ApplyFilter(const double hsv[], double hist10bin[] )
 {
	 double* HueActivation = new double[8];
	 double* SaturationActivation = new double[2];
	 double* ValueActivation = new double[3];
	 findMembershipValue(hsv[0], 32, HueThresholdValues, HueActivation);
	 findMembershipValue(hsv[1], 8, SaturationThresholdValues, SaturationActivation);
	 findMembershipValue(hsv[2], 12, ValueThresholdValues, ValueActivation);
	 for (int i = 0; i != 10; ++i){
		 hist10bin[i] = 0;
	 }
	 MultiParticipateDefazzificator(HueActivation, SaturationActivation, ValueActivation, hist10bin);
	 delete[] HueActivation;
	 delete[] SaturationActivation;
	 delete[] ValueActivation;/*
	 std::cout << "HSV input " << hsv[0] << " " << hsv[1] << " " << hsv[2] << "\n";
	 std::cout << "hist " << 
		 hist10bin[0] << " " <<
		 hist10bin[1] << " " <<
		 hist10bin[2] << " " <<
		 hist10bin[3] << " " <<
		 hist10bin[4] << " " <<
		 hist10bin[5] << " " <<
		 hist10bin[6] << " " <<
		 hist10bin[7] << " " <<
		 hist10bin[8] << " " <<
		 hist10bin[9] << "\n";*/

 }
	
 void Fuzzy10BIN::findMembershipValue(double in, int tSize, const double thresholds[], double actArr[])
 {
	 float temp;
	 int idx = 0;
	 for(int i = 0; i < tSize; i += 4)
	 {
		 temp = 0.f;
		 if(in >= thresholds[i+1] && in <= thresholds[i+2])
			 temp = 1.f;
		 if(in > thresholds[i] && in < thresholds[i+1])
			 temp = (in - thresholds[i])/(thresholds[i+1] - thresholds[i]);
		 if(in > thresholds[i+2] && in <= thresholds[i+3])
			 temp = (in - thresholds[i+2])/(thresholds[i+2] - thresholds[i+3]) + 1;
		 actArr[idx++] = temp;
	 }
 }
	
 void Fuzzy10BIN::MultiParticipateDefazzificator(double hA[], double sA[], double vA[], double resultTable[])
 {
	 int ruleActivation = -1;
	 for(int i = 0; i < 48; ++i)
	 {
		 if( ( hA[BinRulesDefinition[i][0]] > 0 ) && ( sA[BinRulesDefinition[i][1]] > 0 ) && ( vA[BinRulesDefinition[i][2]] > 0 ) )
		 {
			 ruleActivation = BinRulesDefinition[i][3];
			 float min = std::min(hA[BinRulesDefinition[i][0]], 
				 std::min(sA[BinRulesDefinition[i][1]], vA[BinRulesDefinition[i][2]]));
			 resultTable[ruleActivation] += min;
		 }
	 }
 }
