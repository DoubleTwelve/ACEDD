#ifndef ACEDD_H
#define ACEDD_H
#include <opencv2/core/core.hpp>

namespace ACEDD_DESC{
	class CEDD
	{
	public:
		CEDD(int gridSize = 40);
		void Apply(const cv::Mat& srcImg, cv::Mat &CEDDDesc) const;
		static float computeDistance(const cv::Mat &descOne, const cv::Mat &descTwo);
	private:
		void computeBlocks(const cv::Mat &img, cv::Mat &descFull) const;
		void fillInDescriptorAndNormalize(cv::Mat &descFull, const cv::Mat &edges, const cv::Mat &hists) const;
	
		int gridSize;
		int numBlocks;

	};
}
#endif // ACEDD_H
