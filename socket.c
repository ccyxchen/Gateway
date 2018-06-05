#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include "socket.h"

/************************************************
客户端的套接字初始化函数
 ************************************************/

int socket_init_us()
{
	int sockfd = -1;
	socklen_t len = 0;  
    struct sockaddr_in address;  
    int result;  

    //创建流套接字  
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 

    //设置要连接的服务器的信息  
    address.sin_family = AF_INET;//使用网络套接字  
    address.sin_addr.s_addr = inet_addr("192.168.8.104");//服务器地址  
    address.sin_port = htons(20001);//服务器所监听的端口 

    len = sizeof(address);  
    //连接到服务器  
    result = connect(sockfd, (struct sockaddr*)&address, len);  
    if(result == -1)  
    {  
        perror("ops:client\n");  
        exit(1);  
    }
    else{
        printf("Connected success!\n");
    }

    return sockfd;
}