#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "socket.h"
#include "serial.h"
#include "data.h"


/*******************************************************
 运行在NANO-T3上的网关程序
 功能：1、将zigbee协调器从串口发来的数据转发给服务器
 2、将服务器发来的控制指令转发给zigbee协调器
 3、检测数据帧错误
 *******************************************************/
int serial_fd = -1;
int sockfd = -1;  

/*******************************************************
线程1监测从服务器发来的数据，并转发给协调器
 *******************************************************/
void *mythread1(void *threadid)
{
	socket_type sockty;
	char ch[3]; 
	while(1)
	{
        //读取从服务器发来的数据
   		if(read(sockfd, &sockty, sizeof(sockty)) == 0)
            break;
   		printf("success to rei\n");
        //将服务器的数据发给协调器
   		write(serial_fd, &sockty, sizeof(sockty));
   	}

    //退出套接字
    close(sockfd);
    //退出线程
    pthread_exit(0);
}

int main()  
{  
    
    socket_type sockty;
    int num;
    char buf[BUFFER_SIZE], rei_buf[REI_SIZE]; 
    pthread_t pid1;
    int res;

    //创建流套接字,用于与服务器连接  
    sockfd = socket_init_us();

    //初始化串口，此串口接收zigbee协调器信息
    serial_fd = serial_init_us();

    //创建线程1，此线程接收服务器传来的控制指令
    res = pthread_create(&pid1, NULL, (void *)mythread1, NULL);
    if(res)
    {
        printf("Create pthread error!\n");
        return 1;
    }

    //进入循环，等待串口消息
    while(1)
    {
        memset(buf, 0, BUFFER_SIZE);
        //读取协调器从串口发来的数据
        if(read(serial_fd, &sockty, sizeof(sockty)) > 0 )
        {	
            //读取成功，将数据转发给服务器
            if(write(sockfd, &sockty, sizeof(sockty)) > 0){
            	printf("write success.\n");
            }
            else
            {
                printf("send to server feiled.\n");
                break;
            }
        }
        else
            printf("rei from cc2530 feiled.\n");
    }
    //发送到服务器
    
    printf("colse the fork!\n");  
    //关闭套接字
    close(sockfd);  
    exit(0);  
}  
