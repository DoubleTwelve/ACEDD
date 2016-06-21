#include <math.h>
#include <algorithm>
#include "fuzzy_24bin.h"


#define HIST24_LENGTH 24
#define HIST10_LENGTH 10

using namespace ACEDD_DESC;

const double SaturationThresholds[] = {
			0.f/255.f,   0.f/255.f,  68.f/255.f, 188.f/255.f,
		68.f/255.f, 188.f/255.f, 255.f/255.f, 255.f/255.f
	};

const double ValueThresholds[] = {
			0.f/255.f,   0.f/255.f,  68.f/255.f, 188.f/255.f,
		68.f/255.f, 188.f/255.f, 255.f/255.f, 255.f/255.f
	};

const int BinRules[][3] = {
		{1, 1, 1}, {0, 0, 2}, {0, 1, 0}, {1, 0, 2}
	};
	

void Fuzzy24BIN::ApplyFitler(const double hsv[], const double fuzzy10BinHist[], double fuzzy24BinHist[])
{
	double* sActivation = new double[2];
	double* vActivation = new double[2];
	findMembershipValue(hsv[1], 8, SaturationThresholds, sActivation);
	findMembershipValue(hsv[2], 8, ValueThresholds, vActivation);
	computeHistogram(fuzzy10BinHist, sActivation, vActivation, fuzzy24BinHist);
	delete[] sActivation;
	delete[] vActivation;/*
	std::cout << "HSV " << hsv[0] << " " << hsv[1] << " " << hsv[2] << "\n";
	std::cout << "hist10 ";
	for(int i = 0; i != 10; ++i){
		std::cout << fuzzy10BinHist[i] << " ";
	}
	std::cout << "\nhist24 ";
	for(int i = 0; i != 24; ++i){
		std::cout << fuzzy24BinHist[i] << " ";
	}
	std::cout << "\n";*/
}
	
void Fuzzy24BIN::computeHistogram(const double hist10bin[], const double sA[], const double vA[], double hist24bin[])
{
	for(int i = 0; i != HIST24_LENGTH; ++i)
		hist24bin[i] = 0;
	float temp = 0;
	float resultTable[] = {0, 0, 0}; 
	for(int i = 3; i < HIST10_LENGTH; ++i) {
		temp += hist10bin[i];
	}
	if(temp > 0) {
		int ruleActivation = -1;
		for(int i = 0; i < 3; ++i) 
		{
			if( ( sA[BinRules[i][0]] > 0 ) && ( vA[BinRules[i][1]] > 0) ) {
				ruleActivation = BinRules[i][2];
				float min = std::min(sA[BinRules[i][0]], vA[BinRules[i][1]]);
				resultTable[ruleActivation] = min;
			}
		}
	}
	for(int i = 0; i < 3; ++i){
		hist24bin[i] += hist10bin[i];
	}
	for(int i = 3; i < 10; i++) {
		hist24bin[(i-2) * 3] += hist10bin[i] * resultTable[0];
		hist24bin[(i-2) * 3 + 1] += hist10bin[i] * resultTable[1];
		hist24bin[(i-2) * 3 + 2] += hist10bin[i] * resultTable[2];
	}
}
	
void Fuzzy24BIN::findMembershipValue(double in, int tSize, const double thresholds[], double result[])
{
	float temp;
	int idx = 0;
	for(int i = 0; i < tSize; i += 4)
	{
		temp = 0;
		if(in >= thresholds[i+1] && in <= thresholds[i+2])
			temp = 1;
		if(in >= thresholds[i] && in < thresholds[i+1])
			temp = (in - thresholds[i]) / (thresholds[i + 1] - thresholds[i]);
		if(in > thresholds[i+2] && in < thresholds[i+3])
			temp = (in - thresholds[i+2]) / (thresholds[i + 2] - thresholds[i + 3]) + 1;
		result[idx++] = temp;
	}
}
