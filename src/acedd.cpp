#include <math.h>
#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>
#include "acedd.h"
#include "fuzzy_10bin.h"
#include "fuzzy_24bin.h"
#include "acedd_quant.h"

#define HIST10BIN_LENGTH 10
#define HIST24BIN_LENGTH 24
#define DESC_LENGTH 144
#define EDGEHIST_LENGTH 6
#define SQRT2 std::sqrt((double)2)

namespace ACEDD_DESC{
	void processSubBlock(cv::Scalar &acc, int xs, int xe, int ys, int ye, const cv::Mat *img);
	void getTextureMasks(const cv::Scalar &intensities, double masks[]);
	void calculateHSVperBlock(const cv::Scalar& sb0, const cv::Scalar& sb1, const cv::Scalar& sb2, const cv::Scalar& sb3, cv::Scalar& hsv);
	void getEdgeEncoding(const double masks[], int edges[]);
	void processBlock(double* hist24bin, int* edgeEncoding, const cv::Mat *img, int xs, int xe, int ys, int ye);
	void computeSteps(int gridStep, int* pStepX, int* pStepY, int* pStepXY, int imgWidth, int imgHeight);

	CEDD::CEDD(int gridSize_)
	{
		gridSize = gridSize_;
		numBlocks = gridSize * gridSize;
	}

	void CEDD::Apply(const cv::Mat &img, cv::Mat &desc) const
	{
		cv::Mat descF;
		computeBlocks(img, descF);
		CEDDQuant::Apply(desc,descF);
	}

	class ParallelProcessBlocks : public cv::ParallelLoopBody
	{
	public:
		ParallelProcessBlocks(const cv::Mat *img, cv::Mat *edges, 
			cv::Mat *hists, const cv::Mat *coordinates) :
		pImg(img), pEdges(edges), pHist24bin(hists),
			pCoordinates(coordinates) {};

		void operator()(const cv::Range &range) const {
			for(int i = range.start; i != range.end; ++i){
				double *hist24bin = new double[HIST24BIN_LENGTH];
				int* edgeEncoding = new int[EDGEHIST_LENGTH];
				processBlock(hist24bin, edgeEncoding, pImg,
					pCoordinates->at<int>(i,0), pCoordinates->at<int>(i,1), 
					pCoordinates->at<int>(i,2), pCoordinates->at<int>(i,3));

				for(int j = 0; j < HIST24BIN_LENGTH; j++){
					pHist24bin->at<double>(i, j) = hist24bin[j];
				}
				for(int j = 0; j < EDGEHIST_LENGTH; j++){
					pEdges->at<int>(i,j) = edgeEncoding[j];
				}
				delete[] hist24bin;
				delete[] edgeEncoding;
			}
		}

	protected:
		const cv::Mat *pImg;
		cv::Mat *pEdges;
		cv::Mat *pHist24bin;
		const cv::Mat *pCoordinates;
	};

	void CEDD::computeBlocks(const cv::Mat &img, cv::Mat &descFull) const
	{
		cv::Mat edges(numBlocks, EDGEHIST_LENGTH, CV_32S);
		cv::Mat hists(numBlocks, HIST24BIN_LENGTH, CV_64F);
		cv::Mat coordinates(numBlocks, 4, CV_32S);
		int stepX, stepY, stepXY;
		computeSteps(gridSize, &stepX, &stepY, &stepXY, img.cols, img.rows);
		int blockItr = 0;
		int x = 0;
		int y = 0;
		for(int xs = 0; xs != gridSize; ++xs, x += stepX){
			y = 0;
			for(int ys = 0; ys != gridSize; ++ys, y += stepY, blockItr++){
				coordinates.at<int>(blockItr, 0) = x;
				coordinates.at<int>(blockItr, 1) = x + stepX;
				coordinates.at<int>(blockItr, 2) = y;
				coordinates.at<int>(blockItr, 3) = y + stepY;
			}
		}
		cv::parallel_for_(cv::Range(0, numBlocks), ParallelProcessBlocks(&img, &edges, &hists, &coordinates));
		fillInDescriptorAndNormalize(descFull, edges, hists);
	}

	void computeSteps(int gridStep, int* pStepX, int* pStepY, int* pStepXY, int imgWidth, int imgHeight)
	{
		*pStepX = (int)std::floor((float)imgWidth/(float)gridStep);
		*pStepY = (int)std::floor((float)imgHeight/(float)gridStep);
		*pStepXY = *pStepX * (*pStepY);
	}

	void CEDD::fillInDescriptorAndNormalize(cv::Mat &descFull, const cv::Mat &edges, const cv::Mat &hists) const 
	{
		descFull.create(DESC_LENGTH, 1, CV_64F);
		descFull.setTo(cv::Scalar(0));
		int i;
		for(int blockId = 0; blockId != numBlocks; ++blockId){
			i = 0;
			while( edges.at<int>(blockId,i) >= 0 && ( i < edges.cols ) ){
				for(int j = 0; j < HIST24BIN_LENGTH; ++j){
					int index = HIST24BIN_LENGTH * (edges.at<int>(blockId,i)) + j;
					/*std::cout << "Block id " << blockId 
					<< " i " << i << " j " << j << " edges " << edges.at<int>(blockId,i) 
					<< " index " << index << " hist " << hists.at<float>(blockId,j) << "\n";*/
					descFull.at<double>(index) += hists.at<double>(blockId,j);
				}
				i++;
			}
		}
		double sum = cv::sum(descFull)[0];
		descFull /= sum;
	}

	void processBlock(double* hist24bin, int* edgeEncoding, const cv::Mat *img, const int xs, const int xe, const int ys, const int ye)
	{
		float xme = ((float)xe - (float)xs)/2.f + 0.5f;
		int xm = xs + (int)xme;
		float yme = ((float)ye-(float)ys)/2.f + 0.5f;
		int ym = ys + (int)yme;
		cv::Scalar subblock0, subblock1, subblock2, subblock3, hsv;
		processSubBlock(subblock0, xs, xm, ys, ym, img);
		processSubBlock(subblock1, xm, xe, ys, ym, img);
		processSubBlock(subblock2, xs, xm, ym, ye, img);
		processSubBlock(subblock3, xm, xe, ym, ye, img);
		cv::Scalar intensities = cv::Scalar(subblock0[3], subblock1[3], subblock2[3], subblock3[3]);
		double* textureMasks = new double[6];
		getTextureMasks(intensities, textureMasks);
		calculateHSVperBlock(subblock0, subblock1, subblock2, subblock3, hsv);
		getEdgeEncoding(textureMasks, edgeEncoding);
		double* hist10bin = new double[HIST10BIN_LENGTH];
		double hsvArr[] = {hsv[0], hsv[1], hsv[2]};
		Fuzzy10BIN::ApplyFilter(hsvArr, hist10bin);
		Fuzzy24BIN::ApplyFitler(hsvArr, hist10bin, hist24bin);
		delete[] hist10bin;
		delete[] textureMasks;
	}

	void processSubBlock(cv::Scalar &acc, const int xs, const int xe, const int ys, const int ye, const cv::Mat *img)
	{
		long aR = 0;
		long aG = 0;
		long aB = 0;
		for(int x = xs; x != xe; ++x){
			for(int y = ys; y != ye; ++y){
				aB += img->at<cv::Vec3b>(y,x)[0];
				aG += img->at<cv::Vec3b>(y,x)[1];
				aR += img->at<cv::Vec3b>(y,x)[2];
			}
		}
		acc[0] = (double)aB;
		acc[1] = (double)aG;
		acc[2] = (double)aR;
		acc[3] = 0.299f * acc[2] + 0.587f * acc[1] + 0.114f * acc[0];
		double area = (xe-xs) * (ye-ys);
		acc[0] /= area;
		acc[1] /= area;
		acc[2] /= area;
		acc[3] *= 4.f / area;
	}
	
	static const double FiltersCoeffs[][4] = {{2, -2, -2, 2}, {1, 1, -1, -1}, {1, -1, 1, -1},  {SQRT2, 0, 0, -SQRT2}, {0, SQRT2, -SQRT2, 0}};
	void getTextureMasks(const cv::Scalar &intensities, double masks[])
	{
		masks[0] = std::abs( intensities[0] * FiltersCoeffs[0][0] + intensities[1] * FiltersCoeffs[0][1] + intensities[2] * FiltersCoeffs[0][2] + intensities[3] * FiltersCoeffs[0][3] );
		masks[1] = std::abs( intensities[0] * FiltersCoeffs[1][0] + intensities[1] * FiltersCoeffs[1][1] + intensities[2] * FiltersCoeffs[1][2] + intensities[3] * FiltersCoeffs[1][3] );
		masks[2] = std::abs( intensities[0] * FiltersCoeffs[2][0] + intensities[1] * FiltersCoeffs[2][1] + intensities[2] * FiltersCoeffs[2][2] + intensities[3] * FiltersCoeffs[2][3] );
		masks[3] = std::abs( intensities[0] * FiltersCoeffs[3][0] + intensities[1] * FiltersCoeffs[3][1] + intensities[2] * FiltersCoeffs[3][2] + intensities[3] * FiltersCoeffs[3][3] );
		masks[4] = std::abs( intensities[0] * FiltersCoeffs[4][0] + intensities[1] * FiltersCoeffs[4][1] + intensities[2] * FiltersCoeffs[4][2] + intensities[3] * FiltersCoeffs[4][3] );
		masks[5] = std::max( masks[0],  std::max( masks[1] , std::max( masks[2], std::max( masks[3], masks[4] ) ) ) );
		masks[0] /= masks[5];
		masks[1] /= masks[5];
		masks[2] /= masks[5];
		masks[3] /= masks[5];
		masks[4] /= masks[5];
	}

	void calculateHSVperBlock(const cv::Scalar& sb0, const cv::Scalar& sb1, const cv::Scalar& sb2, const cv::Scalar& sb3, cv::Scalar& hsv)
	{
		float aR = (float)(sb0[2] + sb1[2] + sb2[2] + sb3[2])/(255.f * 4.f);
		float aG = (float)(sb0[1] + sb1[1] + sb2[1] + sb3[1])/(255.f * 4.f);
		float aB = (float)(sb0[0] + sb1[0] + sb2[0] + sb3[0])/(255.f * 4.f);
		cv::Mat rgb = cv::Mat(1,1,CV_32FC3);
		rgb.at<cv::Vec3f>(0)[0] = aB;
		rgb.at<cv::Vec3f>(0)[1] = aG;
		rgb.at<cv::Vec3f>(0)[2] = aR;
		cv::Mat hsvM;
		cv::cvtColor(rgb, hsvM, CV_RGB2HSV);
		hsv[0] = hsvM.at<cv::Vec3f>(0)[0];
		hsv[1] = hsvM.at<cv::Vec3f>(0)[1];
		hsv[2] = hsvM.at<cv::Vec3f>(0)[2];
	}

	static const double DirectivityThresholds[] = {14, 0.68, 0.98, 0.98};
	void getEdgeEncoding(const double masks[], int edges[])
	{
		for(int i = 0; i < 6; i++){
			edges[i] = -1;
		}
		int T = 0;
		if(masks[5] < DirectivityThresholds[0]){
			edges[T++] = 0;
		} else {
			if(masks[0] > DirectivityThresholds[1])
				edges[T++] = 1;
			if(masks[1] > DirectivityThresholds[2])
				edges[T++] = 2;
			if(masks[2] > DirectivityThresholds[2])
				edges[T++] = 3;
			if(masks[3] > DirectivityThresholds[3])
				edges[T++] = 4;
			if(masks[4] > DirectivityThresholds[3])
				edges[T++] = 5;
		}
	}

	float CEDD::computeDistance(const cv::Mat &descOne, const cv::Mat &descTwo)
	{
		CV_Assert((descOne.rows == descTwo.rows) 
			&& (descOne.cols == descTwo.cols) 
			&& (descOne.type() == CV_8U) && (descTwo.type() == CV_8U));
		float sumOne = cv::sum(descOne)[0];
		float sumTwo = cv::sum(descTwo)[0];
		float cnt1 = 0;
		float cnt2 = 0;
		float cnt3 = 0;
		cv::MatConstIterator_<unsigned char> descOneItr = descOne.begin<unsigned char>();
		cv::MatConstIterator_<unsigned char> descTwoItr = descTwo.begin<unsigned char>();
		for(; descOneItr != descOne.end<unsigned char>(); ++descOneItr, ++descTwoItr){
			cnt1 += ((float) *descOneItr / sumOne) * ((float) *descTwoItr / sumTwo);
			cnt2 += ((float) *descTwoItr / sumTwo) * ((float) *descTwoItr / sumTwo);
			cnt3 += ((float) *descOneItr / sumOne) * ((float) *descOneItr / sumOne);
		}
		float distance = 100.f - 100.f * cnt1 / (cnt2 + cnt3 - cnt1);
		return distance;
	}
}

