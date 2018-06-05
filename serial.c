#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>
#include "serial.h"

/********************************************************
串口初始化函数
 ********************************************************/

int set_com_config(int fd,int baud_rate, int data_bits,char parity,int stop_bits)
{
       struct termios new_cfg,old_cfg;
       int speed;

       /*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关出错信息*/
       if(tcgetattr(fd,&old_cfg) != 0)        /*该函数得到fd指向的终端配置参数，并将它们保存到old_cfg变量中，成功返回0，否则-1*/
       {
              perror("tcgetttr");
              return -1;
       }
       /*设置字符大小*/
       new_cfg=old_cfg;
       cfmakeraw(&new_cfg);    /*配置为原始模式*/
       new_cfg.c_cflag &= ~CSIZE;/*用位掩码清空数据位的设置*/
       /*设置波特率*/
       switch(baud_rate)
       {
            case 2400:
                        speed=B2400;
                        break;
			case 4800:
                        speed=B4800;
                        break;
            case 9600:
                        speed=B9600;
                        break;   
            case 19200:
                        speed=B19200;
                        break;                        
            case 38400:
                        speed=B38400;
                        break;
            
              case 115200:
                        speed=B115200;
                        break;
			default:
						speed=B115200;
       }
       cfsetispeed(&new_cfg,speed);
       cfsetospeed(&new_cfg,speed);

      

       /*设置数据位*/

       switch(data_bits)

       {

			case 7:
					new_cfg.c_cflag |= CS7;
					break;
			case 8:
					new_cfg.c_cflag |= CS8;
					break;
			default:
					new_cfg.c_cflag |= CS7;
       }

      

       /*设置奇偶校验位*/

       switch(parity)

       {
		  
		  case 'n':
		  case 'N':
		  {
				 new_cfg.c_cflag &= ~PARENB;
				 new_cfg.c_iflag &= ~INPCK;
		  }
		  break;
		  case 'o':
		  case 'O':
		  {
				 new_cfg.c_cflag |= (PARODD | PARENB);
				 new_cfg.c_iflag |= INPCK;
		  }
		  break;
		  case 'e':
		  case 'E':
		  {
				 new_cfg.c_cflag |=  PARENB;
				 new_cfg.c_cflag &= ~PARODD;
				 new_cfg.c_iflag |= INPCK;
		  }
		  break;
		  default:
		  {
				 new_cfg.c_cflag &= ~PARENB;
				 new_cfg.c_iflag &= ~INPCK;
		  }
             

       }

      

       /*设置停止位*/

       switch(stop_bits)

       {

              
              case 1:
					new_cfg.c_cflag &= ~CSTOPB;
					break;

              case 2:
					new_cfg.c_cflag |= CSTOPB;
					break;
			  default:
					new_cfg.c_cflag &= ~CSTOPB;

       }
       /*设置等待时间和最小接收字符*/
       new_cfg.c_cc[VTIME]=0;
       new_cfg.c_cc[VMIN]=1;
       /*处理未接收字符*/
       tcflush(fd,TCIFLUSH);
       /*激活新配置*/
       if((tcsetattr(fd,TCSANOW,&new_cfg))!=0)
       {
              perror("tcsetattr");
              return -1;
       }
       return 0;

}

//初始化NANO-T3上的串口2
int serial_init_us(void)
{
	int serial_fd = -1;

  //打开串口2的设备文件
	if( (serial_fd = open("/dev/ttySAC2", O_RDWR|O_NOCTTY|O_NDELAY)) == -1)
        printf("open tty dev feiled!\n");
    else
        printf("open files success!\n");

    //初始化串口的参数，设置波特率，数据长度，停止位等
    if(set_com_config(serial_fd, 115200, 8, 'N', 1) < 0) /* 配置串口 */
    {
        perror("set_com_config error!\n");
        return 1;
    }   
  
    if (fcntl(serial_fd, F_SETFL, 0) < 0) /* 恢复串口为阻塞状态 */
    {
        perror("fcntl F_SETFL\n");
    }

    if (isatty(serial_fd) == 0) /* 测试打开的文件是否为终端设备 */
    {
        perror("This is not a terminal device");
    }   

    return serial_fd;
}
