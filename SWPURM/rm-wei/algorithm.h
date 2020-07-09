#ifndef ALGORITHM_H
#define ALGORITHM_H
#include<opencv2/opencv.hpp>
#include<serial.h>
#include <mutex>

using namespace cv;
class pidcontral{
public:
    void PID_init_YAW();
    void PID_init_PICTH();
//    pidcontral& operator=(const pidcontral& another);
    float PID_realize(float Points_xy,int a);
    float PID_imitate(int a);
private:
    float x_err_next;
    float x_err;
    float x_err_last;
    float x_Kp, x_Ki, x_Kd;
    float y_err;
    float y_err_next;
    float y_err_last;
    float y_Kp, y_Ki, y_Kd;
    int index;
};

class algorithm{
public:
    algorithm();
//    algorithm(const serial& b,const pidcontral& c,const pidcontral& d);
    algorithm& operator=(const algorithm& another);

<<<<<<< HEAD
    void get_Point(cv::Point2f Points_xy,int high);
=======
    void get_Point(Point2f p1,Point2f p2,Point2f p3,Point2f p4,float high);
>>>>>>> 518ce1677f3c86d870d47a4c976366badf86d8db

    void ranging(float high);//测距

    float gravity();//重力补偿

    bool colorjudge();//判断自瞄颜色

    void serial_read();//串口接收

    void dataprocessing();//数据处理和串口发送

    void serial_translate();//翻译串口数据

    void serial_send();//发送数据



private:
    cv::Point2f my_arrmorPoint;
    serial usbtty;
    unsigned char reversebff[9];
    float distance,light_high;
    pidcontral xpid;
    pidcontral ypid;
    float xangle,yangle;
    unsigned short int Big_speed,Lit_speed;
    unsigned char GONEID,COLOR;
<<<<<<< HEAD
    bool SYMBOL=false;
=======
    int SYMBOL=10;
>>>>>>> 518ce1677f3c86d870d47a4c976366badf86d8db
    mutex lock_2,lock_1;
};


#endif // ALGORITHM_H
