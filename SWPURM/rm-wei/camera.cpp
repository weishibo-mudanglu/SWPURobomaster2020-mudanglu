#include <camera.h>

camera::camera()
{
//    imageSize.width=640;                           //设置处理格式
//    imageSize.height=512;

//    isCamera=true;
//    this->stringName="0";
//    cameraInit();
//    SetExposeTime(5000);
//    setFrameRate(210);
}

camera::camera(const string& fileName)
{
    imageSize.width=640;
    imageSize.height=640;

    this->stringName=fileName;
    if(this->stringName=="0")                      //打开文件名为“0”，则是打开摄像头
    {
        isCamera=true;
        cameraInit();
        SetExposeTime(3000);
        setFrameRate(210);
    }
    else
    {
        isCamera=false;
        Video.open(fileName);
    }

}


void camera::cameraInit()
{
    //1
    CSystem &ptrCSystem=CSystem::getInstance();

    //2.查找摄像头列表
    bool isDiscoverySuccess=ptrCSystem.discovery(ICameraPtrVector);
    if(isDiscoverySuccess==false)
    {
        cout <<"未发现摄像头"<<endl;
        exit(0);
    }
    else
    {
        if(ICameraPtrVector.size()==0)
        {
            cout <<"未发现摄像头"<<endl;
            exit(0);
        }
    }

    //默认摄像头列表中第一个摄像头为目标摄像头
    IICameraPtr=ICameraPtrVector[0];

    //3.连接摄像头
    bool isCameraConnectSuccess=IICameraPtr->connect();
    if(isCameraConnectSuccess==false)
    {
        cout << "摄像头连接失败"<<endl;
        exit(0);
    }
    CameraChangeTrig();
    //4.创建数据流（数据流是我们直接操作的对象）
    setGrabMode(IICameraPtr,1);
    streamPtr=ptrCSystem.createStreamSource(IICameraPtr);
    if(streamPtr==NULL)
    {
        cout<< "创建数据流失败"<<endl;
        exit(0);
    }
   // streamPtr->setBufferCount(1);                              //设置缓存buff为1个
    //5.开始采集
    bool  isStartGrabSetSuccess=streamPtr->startGrabbing();
    if(isStartGrabSetSuccess==false)
    {
        cout << "配置从数据流抓取方式失败"<<endl;
        videoStopStream(); //断流
        videoClose();	  //析构断开与相机的链接
        exit(0);
    }
    CameraState=1;
}

int camera::setGrabMode(ICameraPtr &cameraSptr, bool bContious)
{
    int32_t bRet;
    IAcquisitionControlPtr sptrAcquisitionControl = CSystem::getInstance().createAcquisitionControl(cameraSptr);
    if (NULL == sptrAcquisitionControl)
    {
        return -1;
    }

    CEnumNode enumNode = sptrAcquisitionControl->triggerSelector();
    bRet = enumNode.setValueBySymbol("FrameStart");
    if (false == bRet)
    {
        printf("set TriggerSelector fail.\n");
        return -1;
    }

    if (true == bContious)
    {
        enumNode = sptrAcquisitionControl->triggerMode();
        bRet = enumNode.setValueBySymbol("Off");
        if (false == bRet)
        {
            printf("set triggerMode fail.\n");
            return -1;
        }
    }
    else
    {
        enumNode = sptrAcquisitionControl->triggerMode();
        bRet = enumNode.setValueBySymbol("On");
        if (false == bRet)
        {
            printf("set triggerMode fail.\n");
            return -1;
        }

        /* 设置触发源为软触发（硬触发为Line1） */
        enumNode = sptrAcquisitionControl->triggerSource();
        bRet = enumNode.setValueBySymbol("Software");
        if (false == bRet)
        {
            printf("set triggerSource fail.\n");
            return -1;
        }
    }
    return 0;
}


bool camera::IGetFrame(Mat &src)
{
    if(this->stringName=="0")                                           //摄像头获取图像
    {
        CFrame frame;
        bool getFrameSuccess=streamPtr->getFrame(frame,300);
        if(getFrameSuccess==false)
        {
            cout << "相机读取图像数据失败"<<endl;
            streamPtr->stopGrabbing();
            IICameraPtr->disConnect();
            CameraState=false;
    //        exit(0);
            return false;
        }
        //判断帧的有效性
        bool isValid = frame.valid();
        if (!isValid)
        {
            printf("frame 是无效的!\n");
            return false;
        }
        int frameBGRSizes=frame.getImageHeight()*frame.getImageWidth()*3;
        uint8_t *frameBGRPtr=new(std::nothrow) uint8_t[frameBGRSizes];
        //设置转换配置参数
        IMGCNV_SOpenParam openParam;
        openParam.width= frame.getImageWidth();
        openParam.height= frame.getImageHeight();
        openParam.paddingX = frame.getImagePadddingX();
        openParam.paddingY = frame.getImagePadddingY();
        openParam.dataSize = frame.getImageSize();
        openParam.pixelForamt = frame.getImagePixelFormat();

        //数据转换
        IMGCNV_EErr status=IMGCNV_ConvertToBGR24((unsigned char *)frame.getImage(),
                                                 &openParam,
                                                 frameBGRPtr,
                                                 &frameBGRSizes);
        if (IMGCNV_SUCCESS != status)
        {
            delete[] frameBGRPtr;
            cout << "转换失败" << endl;
            return false;
        }

        int framewidth=frame.getImageWidth();
        int frameheight=frame.getImageHeight();
        Mat image=Mat(frameheight,framewidth,CV_8UC3,frameBGRPtr);
        resize(image,src,imageSize);
        delete[] frameBGRPtr;
        return true;
    }
    else                                                              //视频获取图像
    {
        Video.read(src);
        if(src.empty())
        {
            cout << "image load fail" <<endl;
            return false;
        }

        if((src.size().width!=imageSize.width)||(src.size().height!=imageSize.height))  //如果视频尺寸和设置尺寸不一致执行resize函数
        {
            resize(src,src,imageSize);
        }
        return true;

    }

}

bool camera::videoCheck()
{
    CSystem &systemObj = CSystem::getInstance();

    bool bRet = systemObj.discovery(ICameraPtrVector);
    if (false == bRet)
    {
        cout << "restart on camera"<<endl;
        //exit(-1);
        return false;
    }

//    // 打印相机基本信息（key, 制造商信息, 型号, 序列号）
//    for (uchar i = 0; i < ICameraPtrVector.size(); i++)
//    {
//        ICameraPtr cameraSptr = ICameraPtrVector[i];

//        printf("Camera[%d] Info :\n", i);
//        printf("    key           = [%s]\n", cameraSptr->getKey());
//        printf("    vendor name   = [%s]\n", cameraSptr->getVendorName());
//        printf("    model         = [%s]\n", cameraSptr->getModelName());
//        printf("    serial number = [%s]\n", cameraSptr->getSerialNumber());
//    }

    if (ICameraPtrVector.size() < 1)
    {
        printf("on camera.\n");
        return false;
        //	msgBoxWarn(tr("Device Disconnected."));
    }
    else
    {
        //默认设置列表中的第一个相机为当前相机，其他操作比如打开、关闭、修改曝光都是针对这个相机。
        IICameraPtr = ICameraPtrVector[0];
        cout << "find camera"<<endl;
    }
    return true;
}
bool camera::videoOpen()
{
    if (NULL == IICameraPtr)
    {
        //printf("connect camera fail. No camera.\n");
        cout << "no connect camera"<<endl;
        //exit(-1);
        return false;
    }

    if (true == IICameraPtr->isConnected())
    {
       // printf("camera is already connected.\n");
        cout << "camera is already connected"<<endl;
      //  exit(-1);
        return false;
    }

    if (false == IICameraPtr->connect())
    {
       // printf("connect camera fail.\n");
        cout << "摄像头连接失败"<<endl;
        videoStopStream(); //断流
        videoClose();	  //析构断开与相机的链接
      //  exit(-1);
        return false;
    }
    cout << "success connect" << endl;
    return true;
}

void camera::CameraChangeTrig(ETrigType trigType)
{
    if (NULL == IICameraPtr)
    {
        printf("Change Trig fail. No camera or camera is not connected.\n");
        exit(-1);
        return;
    }

    if (trigContinous == trigType)
    {
        //设置触发模式
        CEnumNode nodeTriggerMode(IICameraPtr, "TriggerMode");
        if (false == nodeTriggerMode.isValid())
        {
            printf("get TriggerMode node fail.\n");
            return;
        }
        if (false == nodeTriggerMode.setValueBySymbol("Off"))
        {
            printf("set TriggerMode value = Off fail.\n");
            return;
        }
    }
    else if (trigSoftware == trigType)
    {
        //设置触发源为软触发
        CEnumNode nodeTriggerSource(IICameraPtr, "TriggerSource");
        if (false == nodeTriggerSource.isValid())
        {
            printf("get TriggerSource node fail.\n");
            return;
        }
        if (false == nodeTriggerSource.setValueBySymbol("Software"))
        {
            printf("set TriggerSource value = Software fail.\n");
            return;
        }

        //设置触发器
        CEnumNode nodeTriggerSelector(IICameraPtr, "TriggerSelector");
        if (false == nodeTriggerSelector.isValid())
        {
            printf("get TriggerSelector node fail.\n");
            return;
        }
        if (false == nodeTriggerSelector.setValueBySymbol("FrameStart"))
        {
            printf("set TriggerSelector value = FrameStart fail.\n");
            return;
        }

        //设置触发模式
        CEnumNode nodeTriggerMode(IICameraPtr, "TriggerMode");
        if (false == nodeTriggerMode.isValid())
        {
            printf("get TriggerMode node fail.\n");
            return;
        }
        if (false == nodeTriggerMode.setValueBySymbol("On"))
        {
            printf("set TriggerMode value = On fail.\n");
            return;
        }
    }
    else if (trigLine == trigType)
    {
        //设置触发源为Line1触发
        CEnumNode nodeTriggerSource(IICameraPtr, "TriggerSource");
        if (false == nodeTriggerSource.isValid())
        {
            printf("get TriggerSource node fail.\n");
            return;
        }
        if (false == nodeTriggerSource.setValueBySymbol("Line1"))
        {
            printf("set TriggerSource value = Line1 fail.\n");
            return;
        }

        //设置触发器
        CEnumNode nodeTriggerSelector(IICameraPtr, "TriggerSelector");
        if (false == nodeTriggerSelector.isValid())
        {
            printf("get TriggerSelector node fail.\n");
            return;
        }
        if (false == nodeTriggerSelector.setValueBySymbol("FrameStart"))
        {
            printf("set TriggerSelector value = FrameStart fail.\n");
            return;
        }

        //设置触发模式
        CEnumNode nodeTriggerMode(IICameraPtr, "TriggerMode");
        if (false == nodeTriggerMode.isValid())
        {
            printf("get TriggerMode node fail.\n");
            return;
        }
        if (false == nodeTriggerMode.setValueBySymbol("On"))
        {
            printf("set TriggerMode value = On fail.\n");
            return;
        }

        // 设置外触发为上升沿（下降沿为FallingEdge）
        CEnumNode nodeTriggerActivation(IICameraPtr, "TriggerActivation");
        if (false == nodeTriggerActivation.isValid())
        {
            printf("get TriggerActivation node fail.\n");
            return;
        }
        if (false == nodeTriggerActivation.setValueBySymbol("RisingEdge"))
        {
            printf("set TriggerActivation value = RisingEdge fail.\n");
            return;
        }
    }
}

void camera::ExecuteSoftTrig()
{
    if (NULL == IICameraPtr)
    {
        printf("Set GainRaw fail. No camera or camera is not connected.\n");
        return;
    }

    CCmdNode nodeTriggerSoftware(IICameraPtr, "TriggerSoftware");
    if (false == nodeTriggerSoftware.isValid())
    {
        printf("get TriggerSoftware node fail.\n");
        return;
    }
    if (false == nodeTriggerSoftware.execute())
    {
        printf("set TriggerSoftware fail.\n");
        return;
    }
}

bool camera::videoStart()
{
    if (streamPtr != NULL)
    {
        cout << "数据流指针不为空"<<endl;
        return true;
    }

    if (NULL == IICameraPtr)
    {
       // printf("start camera fail. No camera.\n");
        cout << "start camera fail. No camera"<<endl;
        //exit(-1);
        return false;
    }

    streamPtr = CSystem::getInstance().createStreamSource(IICameraPtr); //创建流的对象
    if (NULL == streamPtr)
    {
        printf("Create stream source failed.");
        //exit(-1);
        return false;
    }
    bool  isStartGrabSetSuccess=streamPtr->startGrabbing();
    if(isStartGrabSetSuccess==false)
    {
        cout << "配置从数据流抓取方式失败"<<endl;
        videoStopStream(); //断流
        videoClose();	  //析构断开与相机的链接
        return false;
    }

    cout << "restart success"<<endl;
    return true;
}

void camera::videoStopStream()
{
    if (streamPtr == NULL)
    {
        printf("stopGrabbing succefully!\n");
        return;
    }

    if (!streamPtr->stopGrabbing())
    {
        printf("stopGrabbing  fail.\n");
    }
}
void camera::videoClose()
{
    if (NULL == IICameraPtr)
    {
        printf("disconnect camera fail. No camera.\n");
       // exit(-1);
        return;
    }

    if (false == IICameraPtr->isConnected())
    {
        printf("camera is already disconnected.\n");
       // exit(-1);
        return;
    }

    if (false == IICameraPtr->disConnect())
    {
        printf("disconnect camera fail.\n");
    }
}
void camera::startGrabbing()
{
    streamPtr->setBufferCount(1);
    streamPtr->startGrabbing();
}

void camera::SetExposeTime(double exp)
{
    bool bRet;
    IAcquisitionControlPtr sptrAcquisitionControl = CSystem::getInstance().createAcquisitionControl(IICameraPtr);
    if (NULL == sptrAcquisitionControl)
    {
        printf("create a IAcquisitionControlPtr failed!\n");
        return;
    }
    CEnumNode eNode = sptrAcquisitionControl->exposureAuto();
    uint64 getValue;
    if (!eNode.getValue(getValue))
    {
        printf("get value of type is failed!\n");
        return;
    }
    if (getValue)//如果开启了自动曝光模式，则关闭
    {
        bRet = eNode.setValueBySymbol("Off");
        if (!bRet)
        {
            printf("close autoExposure failed!\n");
            return;
        }
    }

    CDoubleNode dNode = sptrAcquisitionControl->exposureTime();
    bRet = dNode.setValue(exp);
    if (!bRet)
    {
        printf("set exposure failed!\n");
        return;
    }
    cout << "exposure success"<<endl;

}

void camera::SetAdjustPlus(double adj)
{
    if (NULL == IICameraPtr)
    {
        printf("Set GainRaw fail. No camera or camera is not connected.\n");
        return;
    }

    CDoubleNode nodeGainRaw(IICameraPtr, "GainRaw");

    if (false == nodeGainRaw.isValid())
    {
        printf("get GainRaw node fail.\n");
        return;
    }

    if (false == nodeGainRaw.isAvailable())
    {
        printf("GainRaw is not available.\n");
        return;
    }

    if (false == nodeGainRaw.setValue(adj))
    {
        printf("set GainRaw value = %f fail.\n", adj);
        return;
    }
}

void camera::setBufferSize(int nSize)
{
    streamPtr = CSystem::getInstance().createStreamSource(IICameraPtr);
    if (NULL == streamPtr)
    {
        printf("create a SourceStream failed!\n");
        return;
    }
    streamPtr->setBufferCount(nSize);
}
void camera::setBalanceRatio(double dRedBalanceRatio, double dGreenBalanceRatio, double dBlueBalanceRatio)
{
    bool bRet;
    IAnalogControlPtr sptrAnalogControl = CSystem::getInstance().createAnalogControl(IICameraPtr);
    if (NULL == sptrAnalogControl)
    {
        return ;
    }

    /* 关闭自动白平衡 */
    CEnumNode enumNode = sptrAnalogControl->balanceWhiteAuto();
    if (false == enumNode.isReadable())
    {
        printf("balanceRatio not support.\n");
        return ;
    }

    bRet = enumNode.setValueBySymbol("Off");
    if (false == bRet)
    {
        printf("set balanceWhiteAuto Off fail.\n");
        return ;
    }

    enumNode = sptrAnalogControl->balanceRatioSelector();
    bRet = enumNode.setValueBySymbol("Red");
    if (false == bRet)
    {
        printf("set red balanceRatioSelector fail.\n");
        return ;
    }

    CDoubleNode doubleNode = sptrAnalogControl->balanceRatio();
    bRet = doubleNode.setValue(dRedBalanceRatio);
    if (false == bRet)
    {
        printf("set red balanceRatio fail.\n");
        return ;
    }

    enumNode = sptrAnalogControl->balanceRatioSelector();
    bRet = enumNode.setValueBySymbol("Green");
    if (false == bRet)
    {
        printf("set green balanceRatioSelector fail.\n");
        return ;
    }

    doubleNode = sptrAnalogControl->balanceRatio();
    bRet = doubleNode.setValue(dGreenBalanceRatio);
    if (false == bRet)
    {
        printf("set green balanceRatio fail.\n");
        return ;
    }

    enumNode = sptrAnalogControl->balanceRatioSelector();
    bRet = enumNode.setValueBySymbol("Blue");
    if (false == bRet)
    {
        printf("set blue balanceRatioSelector fail.\n");
        return ;
    }

    doubleNode = sptrAnalogControl->balanceRatio();
    bRet = doubleNode.setValue(dBlueBalanceRatio);
    if (false == bRet)
    {
        printf("set blue balanceRatio fail.\n");
        return ;
    }
}

void camera::setResolution(int height, int width)
{
    bool bRet;
    IImageFormatControlPtr sptrImageFormatControl = CSystem::getInstance().createImageFormatControl(IICameraPtr);
    if (NULL == sptrImageFormatControl)
    {
        printf("create a IImageFormatControlPtr failed!\n");
        return;
    }

    CIntNode intNode = sptrImageFormatControl->height();
    bRet = intNode.setValue(width);
    if (false == bRet)
    {
        printf("set width fail.\n");
        return;
    }

    intNode = sptrImageFormatControl->height();
    bRet = intNode.setValue(height);
    if (false == bRet)
    {
        printf("set height fail.\n");
        return;
    }
}
void camera::setROI(int64_t nX, int64_t nY, int64_t nWidth, int64_t nHeight)
{
    bool bRet;
    CIntNode nodeWidth(IICameraPtr, "Width");
    bRet = nodeWidth.setValue(nWidth);
    if (!bRet)
    {
        printf("set width fail.\n");
    return;
    }
    CIntNode nodeHeight(IICameraPtr, "Height");
    bRet = nodeHeight.setValue(nHeight);
    if (!bRet)
    {
        printf("set Height fail.\n");
    return;
    }
    CIntNode OffsetX(IICameraPtr, "OffsetX");
    bRet = OffsetX.setValue(nX);
    if (!bRet)
    {
        printf("set OffsetX fail.\n");
    return;
    }
    CIntNode OffsetY(IICameraPtr, "OffsetY");
    bRet = OffsetY.setValue(nY);
    if (!bRet)
    {
        printf("set OffsetY fail.\n");
    return;
    }
}

void camera::setBinning()
{
    CEnumNodePtr ptrParam(new CEnumNode(IICameraPtr, "Binning"));
    if (ptrParam)
    {
        if (false == ptrParam->isReadable())
        {
            printf("binning not support.\n");
            return;
        }

        if (false == ptrParam->setValueBySymbol("XY"))
        {
            printf("set Binning XY fail.\n");
            return;
        }

//        if (false == ptrParam->setValueBySymbol("Off"))
//        {
//            printf("set Binning Off fail.\n");
//            return;
//        }
    }
    return;
}

bool camera::loadSetting(int mode)
{
    CSystem &sysobj = CSystem::getInstance();
    IUserSetControlPtr iSetPtr;
    iSetPtr = sysobj.createUserSetControl(IICameraPtr);
    CEnumNode nodeUserSelect(IICameraPtr, "UserSetSelector");
    if (mode == 0)
    {
        if (!nodeUserSelect.setValueBySymbol("UserSet1")){
            cout << "set UserSetSelector failed!" << endl;
            return false;
        }

    }
    else if (mode == 1){
        if (!nodeUserSelect.setValueBySymbol("UserSet2")){
            cout << "set UserSetSelector failed!" << endl;
            return false;
        }

    }

    CCmdNode nodeUserSetLoad(IICameraPtr, "UserSetLoad");
    if (!nodeUserSetLoad.execute()){
        cout << "set UserSetLoad failed!" << endl;
        return false;
    }
    return true;

}
void camera::setFrameRate(double rate)
{
    bool bRet;
    IAcquisitionControlPtr sptAcquisitionControl = CSystem::getInstance().createAcquisitionControl(IICameraPtr);
    if (NULL == sptAcquisitionControl)
    {
        return;
    }

    CBoolNode booleanNode = sptAcquisitionControl->acquisitionFrameRateEnable();
    bRet = booleanNode.setValue(true);
    if (false == bRet)
    {
        printf("set acquisitionFrameRateEnable fail.\n");
        return;
    }

    CDoubleNode doubleNode = sptAcquisitionControl->acquisitionFrameRate();
    bRet = doubleNode.setValue(rate);
    if (false == bRet)
    {
        printf("set acquisitionFrameRate fail.\n");
        return;
    }
}


void camera::restartConnect()
{
    bool temp=true;
    streamPtr.reset();
    temp=videoCheck();
    if(temp==true)
    {
        temp=videoOpen();
        CameraChangeTrig();
    }
    if(temp==true)
    {

        temp=videoStart();
    }
    CameraState=temp;
}
