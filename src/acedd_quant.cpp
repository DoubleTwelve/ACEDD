#include <assert.h>
#include <math.h>
#include <algorithm>
#include "acedd_quant.h"
#include <opencv2/core/core.hpp>

namespace ACEDD_DESC {
	const double QuantTable1[] = {
		180.19686541079636, 23730.024499150866, 61457.152912541605, 113918.55437576842, 
		179122.46400035513, 260980.3325940354, 341795.93301552488, 554729.98648386425
	};

	const double QuantTable2[] = {
		209.25176965926232, 22490.5872862417345, 60250.8935141849988, 120705.788057580583, 
		181128.08709063051, 234132.081356900555, 325660.617733105708, 520702.175858657472
	};

	const double QuantTable3[] = {
		405.4642173212585, 4877.9763319071481, 10882.170090625908, 18167.239081219657, 
		27043.385568785292, 38129.413201299016, 52675.221316293857, 79555.402607004813
	};

	const double QuantTable4[] = {
		405.4642173212585, 4877.9763319071481, 10882.170090625908, 18167.239081219657, 
		27043.385568785292, 38129.413201299016, 52675.221316293857, 79555.402607004813
	};

	const double QuantTable5[] = {
		968.88475977695578, 10725.159033657819, 24161.205360376698, 41555.917344385321, 
		62895.628446402261, 93066.271379694881, 136976.13317822068, 262897.86056221306
	};

	const double QuantTable6[] = {
		968.88475977695578, 10725.159033657819, 24161.205360376698, 41555.917344385321,
		62895.628446402261, 93066.271379694881, 136976.13317822068, 262897.86056221306
	};

	void CEDDQuant::Apply(cv::Mat &result, const cv::Mat &desc)
	{
		float max = 1;
		result.create(desc.rows,1,CV_8U);
		//byte[] result = new byte[descHist.length];

		for(size_t i = 0; i < 24; ++i){
			max = 1;
			result.at<unsigned char>(i) = 0;
			for(size_t j = 0; j < 8; ++j) {
				float temp = std::abs(desc.at<double>(i) - QuantTable1[j] / 1000000);
				if(temp < max) {
					max = temp;
					result.at<unsigned char>(i) = j;
				}
			}
		}

		for(size_t i = 24; i < 48; ++i) {
			max = 1;
			result.at<unsigned char>(i) = 0;
			for(size_t j = 0; j < 8; ++j) {
				float temp = std::abs(desc.at<double>(i) - QuantTable2[j] / 1000000);
				if(temp < max) {
					max = temp;
					result.at<unsigned char>(i) = j;
				}
			}
		}

		for(size_t i = 48; i < 72; ++i) {
			max = 1;
			result.at<unsigned char>(i) = 0;
			for(size_t j = 0; j < 8; ++j) {
				float temp = std::abs(desc.at<double>(i) - QuantTable3[j] / 1000000);
				if(temp < max) {
					max = temp;
					result.at<unsigned char>(i) = j;
				}
			}
		}

		for(size_t i = 72; i < 96; ++i) {
			max = 1;
			result.at<unsigned char>(i) = 0;
			for(size_t j = 0; j < 8; ++j) {
				float temp = std::abs(desc.at<double>(i) - QuantTable4[j] / 1000000);
				if(temp < max) {
					max = temp;
					result.at<unsigned char>(i) = j;
				}
			}
		}

		for(size_t i = 96; i < 120; ++i) {
			max = 1;
			result.at<unsigned char>(i) = 0;
			for(size_t j = 0; j < 8; ++j) {
				float temp = std::abs(desc.at<double>(i) - QuantTable5[j] / 1000000);
				if(temp < max) {
					max = temp;
					result.at<unsigned char>(i) = j;
				}
			}
		}

		for(size_t i = 120; i < 144; ++i) {
			max = 1;
			result.at<unsigned char>(i) = 0;
			for(size_t j = 0; j < 8; ++j) {
				float temp = std::abs(desc.at<double>(i) - QuantTable6[j] / 1000000);
				if(temp < max) {
					max = temp;
					result.at<unsigned char>(i) = j;
				}
			}
		}
	}
}

