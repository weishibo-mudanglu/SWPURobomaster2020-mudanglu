#include"algorithm.h"
#define row 640.0//宽度
#define col 512.0//高度
#define GRAVITY 9.78//重力加速度
using namespace std;
union CI{
    unsigned char st[4];
    float num;
};//联合体
union CS{
    unsigned char st[2];
    unsigned short int num;
};
//画面像素差计算
float maghrib_pixel(float p1,float p2,float p3,float p4)
{
    float coordinates =(p1+p2+p3+p4)/4;

    float erro = fabs(coordinates-row/2);

    return erro;

}


//像素差值转化为角度值
float maghrib_angle(float erro,int a)
{
    float angle_erro;
    //x方向角度计算
    if(a == 1)
    {
         angle_erro = erro/row*60;
    }
    //y方向角度计算
    if(a==2)
    {
         angle_erro = erro/col*60;
    }
    return angle_erro;
}

float BulletModel(float x, float v, float angle)
{
    //x:m,v:m/s,angle:rad
  float t, y;
  t = (float)(x/(v * cos(angle)));//计算水平方向弹丸击中目标的时间
  y = (float)(v * sin(angle) * t - GRAVITY * t * t / 2);//加速度速度公式计算了弹丸经过时间t后实际的高度
  return y;//返回计算出的实际高度
}

float GetPitch(float x,  float v) //
{
  float y_temp = 0.0;
  float y_actual, dy;
  float a;
  // by iteration
  for (int i = 0; i < 20; i++)
  {
    a = (float) atan2(y_temp, x);//返回夹角
    y_actual = BulletModel(x, v, a);//得到能打击的实际高度
    dy = y - y_actual;
    y_temp = y_temp + dy;
    if (fabs(dy) < 0.001)
    {
      break;
    }
    //printf("iteration num %d: angle %f,temp target y:%f,err of y:%f\n",i+1,a*180/3.1415926535,yTemp,dy);
  }
  return a;

}

float pidcontral::PID_realize(float p1,float p2,float p3,float p4,int a)
{
    if(a==1)
    {
        float pixel_erro = maghrib_pixel(p1,p2,p3,p4);
        float angle_erro = maghrib_angle(pixel_erro,a);
        x_err = angle_erro;
        float incrementSpeed = x_Kp*(x_err - x_err_next) + x_Ki*x_err + x_Kd*(x_err - 2 * x_err_next + x_err_last);
        x_err_last = x_err_next;
        x_err_next = x_err;
        //cout<<"进入x_pid算法:"<<incrementSpeed<<endl;
        return incrementSpeed;
    }
    else
    {
        float pixel_erro = maghrib_pixel(p1,p2,p3,p4);
        float angle_erro = maghrib_angle(pixel_erro,a);
        y_err = angle_erro;
        float incrementSpeed = y_Kp*(y_err - y_err_next) + y_Ki*x_err + y_Kd*(y_err - 2 * y_err_next + y_err_last);
        y_err_last = y_err_next;
        y_err_next = y_err;
        //cout<<"进入y_pid算法:"<<incrementSpeed<<endl;
        return incrementSpeed;
    }


}
//pidcontral& pidcontral::operator =(const pidcontral& another)
//{
//    this->
//}
void pidcontral::PID_init()
{
    x_err = 0.0;
    x_err_last = 0.0;
    x_err_next = 0.0;
    x_Kp = 0.2;
    x_Ki = 0.015;
    x_Kd = 0.2;
    y_err = 0.0;
    y_err_last = 0.0;
    y_err_next = 0.0;
    y_Kp = 0.2;
    y_Ki = 0.015;
    y_Kd = 0.2;
}
algorithm::algorithm()
{

}

algorithm::algorithm(const serial& b,const pidcontral &c,const pidcontral &d)
{
    this->usbtty=b;
    this->xpid = c;
    this->ypid = d;
}
algorithm& algorithm::operator =(const algorithm& another)
{
    this->usbtty = another.usbtty;
    this->xpid = another.xpid;
    this->ypid = another.ypid;
    return *this;
}


void algorithm::get_Point(Point2f p1, Point2f p2, Point2f p3, Point2f p4,float high)
{
    if(lock_2.try_lock())
    {
        my_arrmorPoint[0]=p1;
        my_arrmorPoint[1]=p2;
        my_arrmorPoint[2]=p3;
        my_arrmorPoint[3]=p4;
        light_high = high;
        SYMBOL = true;
    }
    //std::cout<<"get point succsee"<<std::endl;
    lock_2.unlock();
}
//让运算可能用到的值一次性全部加载，防止线程中断占据大量时间
void algorithm::load_Point(Point2f p1, Point2f p2, Point2f p3, Point2f p4,float high)
{
    lock_2.lock();
    p1 = my_arrmorPoint[0];
    p2 = my_arrmorPoint[1];
    p3 = my_arrmorPoint[2];
    p4 = my_arrmorPoint[3];
    high = light_high;
    SYMBOL = false;
    lock_2.unlock();
}

bool algorithm::colorjudge()
{
    if(COLOR==0x01)
        return true;
    else
        return false;
}


void algorithm::serial_read()
{

    while(1)
    {   string words="wawooooo";
        int lenth=words.length();
        char *send=(char *)words.data();
        int writeLength=write(usbtty.fd,send,lenth);
        cout<<"串口发送"<<writeLength<<endl;
        lock_1.lock();
        int readLength = read(usbtty.fd,reversebff,9);
        cout<<"运行串口接收线程"<<endl;
        if(readLength>1)
        {
            cout<<"接收数据长度"<<readLength<<endl;
            cout<<"接收数据"<<reversebff<<endl;
            cout<<"****************************************"<<endl;
        }
        lock_1.unlock();
    }

}

void algorithm::serial_translate()
{
    CS speed_big,speed_lit;
    lock_1.lock();
    if(reversebff[0]==0xAA)
    {
        if(reversebff[1]==0xAA)
        {
            switch (reversebff[2])
            {
                case 0x01:
                    GONEID=reversebff[3];
                    speed_big.st[0] = reversebff[4];
                    speed_big.st[1] = reversebff[5];
                    speed_lit.st[0] = reversebff[6];
                    speed_lit.st[1] = reversebff[7];
                    Big_speed = speed_big.num;
                    Lit_speed = speed_lit.num;
                    break;
                case 0x02:
                    GONEID=0x02;
                    break;
                case 0x03:
                    GONEID=reversebff[3];
                    speed_big.st[0] = reversebff[4];
                    speed_big.st[1] = reversebff[5];
                    speed_lit.st[0] = reversebff[6];
                    speed_lit.st[1] = reversebff[7];
                    break;
                case 0x05:
                    COLOR = reversebff[3];
                    break;
            }
        }
    }
    lock_1.unlock();
}

void algorithm::serial_send()
{

}
void algorithm::PID()
{
    xangle=xpid.PID_realize(my_arrmorPoint[0].x,my_arrmorPoint[1].x,my_arrmorPoint[2].x,my_arrmorPoint[3].x,1);
    yangle=ypid.PID_realize(my_arrmorPoint[0].y,my_arrmorPoint[1].y,my_arrmorPoint[2].y,my_arrmorPoint[3].y,2);
}

void algorithm::gravity()
{
    switch(GONEID)
    {
        case 0x00:
            return GetPitch(distance,Big_speed);
            break;
        case 0x01:
            return GetPitch(distance,Lit_speed);
            break;
    }
}
void algorithm::ranging(float high)
{
//    std::cout<<"进入了测距算法"<<std::endl;
    distance = high*100;
}

void algorithm::dataprocessing()
{
    while(1)
    {
        Point2f p1,p2,p3,p4;
        float high;
        serial_translate();//翻译串口数据
        if(SYMBOL==true)
        {
            load_Point(p1,p2,p3,p4,high);
        }
        else
        {

        }
        ranging(high);
        gravity();
    }
}