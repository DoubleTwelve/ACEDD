#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "acedd.h"

using namespace ACEDD_DESC;

int main(int argc, const char * argv[]) {
    CEDD cedd;
    cv::Mat img_a, img_b, desc_a, desc_b;
    if (argc != 3)
      std::cout << "Usage: ACEDD_APP img1.jpg img2.jpg\n";
    std::cout << "Image 1: " << argv[1] << "\nImage 2: " << argv[2] << "\n";
    img_a = cv::imread(argv[1]);
    img_b = cv::imread(argv[2]);
    cedd.Apply(img_a, desc_a);
    cedd.Apply(img_b, desc_b);
    float dist = cedd.computeDistance(desc_a, desc_b);
    std::cout << "Distance " << dist << "\n";
    return 0;
}
