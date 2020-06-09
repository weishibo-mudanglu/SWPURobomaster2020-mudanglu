#ifndef IMGPRODUCEPROCESSING_H
#define IMGPRODUCEPROCESSING_H
#include <iostream>
#include <unistd.h>
#include <arrmor.h>
#include <opencv2/opencv.hpp>
#include<algorithm.h>
#include <camera.h>
class imgProduceProcessing
{
public:
    imgProduceProcessing();
    imgProduceProcessing(const arrmor& a,const camera& v );
    void ImageProducing();
    void ImageProcessing();
    void ImageProducing2ImageProcessing();
public:
    std::vector<cv::Mat> producedImage;
    std::vector<cv::Mat> processingImage;
    unsigned int producingFrames;
    unsigned int processingFrames;
    arrmor arrmorDection;
    bool BreakFlag;
    camera video;
};

#endif // IMGPRODUCEPROCESSING_H
