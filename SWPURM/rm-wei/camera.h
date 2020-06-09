#ifndef CAMERA_H
#define CAMERA_H
#include <math.h>
#include "GenICam/System.h"
#include "Media/VideoRender.h"
#include "Media/ImageConvert.h"
#include <iostream>
#include "GenICam/Camera.h"
#include "GenICam/GigE/GigECamera.h"
#include "GenICam/GigE/GigEInterface.h"
#include "Infra/PrintLog.h"
#include "Memory/SharedPtr.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace Dahua::GenICam;
using namespace Dahua::Infra;
using namespace Dahua::Memory;
using namespace std;
using namespace cv;

static uint32_t gFormatTransferTbl[] =
    {
        // Mono Format
        gvspPixelMono1p,
        gvspPixelMono8,
        gvspPixelMono10,
        gvspPixelMono10Packed,
        gvspPixelMono12,
        gvspPixelMono12Packed,

        // Bayer Format
        gvspPixelBayRG8,
        gvspPixelBayGB8,
        gvspPixelBayBG8,
        gvspPixelBayRG10,
        gvspPixelBayGB10,
        gvspPixelBayBG10,
        gvspPixelBayRG12,
        gvspPixelBayGB12,
        gvspPixelBayBG12,
        gvspPixelBayRG10Packed,
        gvspPixelBayGB10Packed,
        gvspPixelBayBG10Packed,
        gvspPixelBayRG12Packed,
        gvspPixelBayGB12Packed,
        gvspPixelBayBG12Packed,
        gvspPixelBayRG16,
        gvspPixelBayGB16,
        gvspPixelBayBG16,
        gvspPixelBayRG10p,
        gvspPixelBayRG12p,

        gvspPixelMono1c,

        // RGB Format
        gvspPixelRGB8,
        gvspPixelBGR8,

        // YVR Format
        gvspPixelYUV411_8_UYYVYY,
        gvspPixelYUV422_8_UYVY,
        gvspPixelYUV422_8,
        gvspPixelYUV8_UYV,
};
#define gFormatTransferTblLen sizeof(gFormatTransferTbl) / sizeof(gFormatTransferTbl[0])
class camera
{
public:
    camera();                                                   //无参构造，默认调用摄像头
    camera(const string& fileName);                             //有参构造，根据文件名判断视频，摄像头二选一
    ~camera()
    {
        videoStopStream(); //断流
        videoClose();	  //析构断开与相机的链接
    }
    //枚举触发方式
    enum ETrigType
    {
        trigContinous = 0, //连续拉流
        trigSoftware = 1,  //软件触发
        trigLine = 2,	   //外部触发
    };
    void cameraInit();                                           //摄像头初始化
    bool IGetFrame(Mat& src);                                    //获取一帧图像
    bool videoCheck();                                           //检查摄像头
    bool videoOpen();                                            //连接摄像头
    int setGrabMode(ICameraPtr& cameraSptr, bool bContious);     //设置摄像头抓取数据方式
    void CameraChangeTrig(ETrigType trigType = trigContinous);    //设置触发模式，一般为软触发
    void ExecuteSoftTrig();								//执行一次软触发
    bool videoStart();									//创建流对象

    void videoStopStream();								//断开拉流
    void videoClose();									//断开相机

    void startGrabbing();                               //
    void SetExposeTime(double exp);						//设置曝光
    void SetAdjustPlus(double adj);						//设置增益
    void setBufferSize(int nSize);
    void setBalanceRatio(double dRedBalanceRatio, double dGreenBalanceRatio, double dBlueBalanceRatio);
    void setResolution(int height = 720, int width = 1280); //设置分辨率
    void setROI(int64_t nX, int64_t nY, int64_t nWidth, int64_t nHeight);
    void setBinning();
    bool loadSetting(int mode);
    void setFrameRate(double rate = 210);					//设置帧率
    void restartConnect();
public:
    ICameraPtr IICameraPtr;                   // 相机指针对象
    bool CameraState;
    bool isCamera;                            //摄像头与视频判断标志位
private:

    IStreamSourcePtr streamPtr;               //流数据对象
    TVector<ICameraPtr> ICameraPtrVector;     // 相机指针列表对象

    string stringName;                        //输入文件名
    VideoCapture Video;                       //video变量，用于播放视频

    Size imageSize;                           //设置处理图片尺寸
};

#endif // CAMERA_H
