#ifndef ACEDD_QUANT_H
#define ACEDD_QUANT_H
#include <opencv2/core/core.hpp>

namespace ACEDD_DESC {
	class CEDDQuant
	{
	public:
		static void Apply(cv::Mat &descQuant, const cv::Mat &desc);
	};
}
#endif // ACEDD_QUANT_H
