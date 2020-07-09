#include "serial.h"

serial::serial()
{

}

//serial::serial(char *name,unsigned int Speeds,unsigned int Data,unsigned int Stop,char parity)
//{
//    serialDevName= name;
//    this->serialSpeeds=Speeds;
//    this->serialData=    Data;
//    this->serialStop=    Stop;
//    this->serialParity=parity;
//}


bool serial::serial_open()
{
    fd=open(serialDevName,O_RDWR|O_NOCTTY|O_NONBLOCK);
    if(fd<0) return false;
    else     return  true;

}

void serial::serial_speed(int speed,struct termios &opt)
{
//    int i;
//    int num=sizeof(speed_a)/sizeof(int);
//    for(i=0;i<num;i++)
//    {
//        if(name_arr[i]==speed)
//        {
//            cfsetispeed(&opt,speed_a[i]);
//            cfsetospeed(&opt,speed_a[i]);
//            break;
//        }
//    }
//    if(i==num)
//    {
//        cout << "波特率错误"<<endl;
//    }
    cfsetispeed(&opt,B115200);
    cfsetospeed(&opt,B115200);
}
void serial::set_data(unsigned int dataBit,struct termios &opt)
{
    opt.c_cflag &=             ~CSIZE;

        switch (dataBit)
        {
        case 5:
        {
            opt.c_cflag|=CS5;
            break;
        }
        case 6:
        {
            opt.c_cflag|=CS6;
            break;
        }
        case 7:
        {
            opt.c_cflag|=CS7;
            break;
        }
        case 8:
        {
            opt.c_cflag|=CS8;
            break;
        }
        default:
        {
            opt.c_cflag|=CS8;
            cout <<"数据位设置失败，默认8位数据位"<<endl;
            break;
        }
        }
}
void serial::set_stop(unsigned int stop,struct termios &opt)
{
    switch(stop)
        {
        case 2:
        {
            opt.c_cflag|= CSTOPB;
            break;
        }
        case 1:
        {
            opt.c_cflag&= ~CSTOPB;
            break;
        }
        default:
        {
            opt.c_cflag&= ~CSTOPB;
            cout << "停止位设置错误，默认设置为一位"<<endl;
            break;
        }
        }
}
void serial::set_parity(char parity, termios &opt)
{
    switch(parity)
        {
        case 'n':                  // 无校验
        case 'N':
        {
            opt.c_cflag &= ~PARENB;
            opt.c_iflag &= ~INPCK;
            break;
        }
        case 'e':                  //奇校验
        case 'E':
        {
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            opt.c_iflag |= INPCK;
            break;
        }
        case 'o':                  //偶校验
        case 'O':
        {
            opt.c_cflag |= PARENB;
            opt.c_cflag |= PARODD;
            opt.c_iflag |= INPCK;
            break;
        }
        default:
        {
            cout << "奇偶校验失败，默认设置为无校验"<<endl;
            opt.c_cflag &= ~PARENB;
            opt.c_iflag &= ~INPCK;
            break;
        }
        }
}

bool serial::serialParameter()
{
    termios    opt;
    termios oldOpt;
    tcgetattr(fd, &oldOpt);
    tcflush(fd, TCIOFLUSH);
   //激活本地连接和接受数据始能
    opt.c_cflag |=     (CLOCAL|CREAD);
   //波特率
    serial_speed(serialSpeeds,opt);


   //恢复串口未阻塞状态
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    {
        printf("fcntl failed!\n");
        exit(0);
    }

   //设置数据位
    set_data(serialData,      opt);
   //设置停止位
    set_stop(serialStop,      opt);
   //设置奇偶校验
    set_parity(serialParity,  opt);

   //其他设置

    if(serialParity != 'n')
        opt.c_iflag |= INPCK;

    opt.c_iflag |=        IGNPAR|ICRNL;
    opt.c_oflag |=               OPOST;
    opt.c_iflag &= ~(IXON|IXOFF|IXANY);
    opt.c_cc[VTIME]  =               0;//等待时间
    opt.c_cc[VMIN]   =               0;//最小接收时间

    tcflush(fd,TCIOFLUSH);
    int temp=tcsetattr(fd,TCSANOW,&opt);

    if(temp!=0) return false;
    else        return  true;
}

bool serial::serialInit(char *name,unsigned int Speeds,unsigned int Data,unsigned int Stop,char parity)
{
    serialDevName= name;
    this->serialSpeeds=Speeds;
    this->serialData=    Data;
    this->serialStop=    Stop;
    this->serialParity=parity;
    //打开串口
    if(!serial_open())
       {
           cout << "open serial fail"  <<endl;
           return false;
       }

       //2.配置串口参数
       if(!serialParameter())
       {
           cout << "parameter set fail"<<endl;
           return false;
       }

       cout <<"successful"<<endl;
       return true;
}
