#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <signal.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h> 
#include <pthread.h>
#include "mysql.h"
#include "data.h"

char write_buf[MAX_BUF];
int client_sockfd = -1;

int check_flag = 0;     //功能一控制，值为1打开功能1，否则关闭
int cc_flag[7];         //传感器数据显示控制，若值为1则显示对应传感器数据，否则不显示
int exi = 0;            //程序退出控制，若值为1则退出程序

MYSQL mysql;

//此函数实现菜单上的功能1
void check_sendata()
{
    MYSQL_RES * result;
    MYSQL_ROW row; 
    int end;
    int res;
    char sel;
    memset(cc_flag, 0, sizeof(cc_flag));

    //查询zigbee节点的传感器类型
    mysql_query(&mysql, "select sensor_id from end_device");
    result = mysql_store_result(&mysql);//将查询的全部结果读取到客户端  
    
    //向用户询问要查看哪些传感器
    while((row = mysql_fetch_row(result)) != NULL)
    {
        end = atoi(row[0]);

        switch(end)
        {
            case 1:
                printf("是否开启温度？（Y/N）");
                scanf("%c", &sel);
                getchar();
                if(sel == 'Y')
                    cc_flag[1] = 1;
                break;
            case 2:
                printf("是否开启湿度? (Y/N)");
                scanf("%c", &sel);
                getchar();
                if(sel == 'Y')
                    cc_flag[2] = 1;
                break;
            case 3:
                printf("是否开启光照？（Y/N）");
                scanf("%c", &sel);
                getchar();
                if(sel == 'Y')
                    cc_flag[3] = 1;
                break;
            case 4:
                printf("是否开启烟雾？（Y/N）");
                scanf("%c", &sel);
                getchar();
                if(sel == 'Y')
                    cc_flag[4] = 1;
                break;
            case 5:
                printf("是否开启磁感应？（Y/N）");
                scanf("%c", &sel);
                getchar();
                if(sel == 'Y')
                    cc_flag[5] = 1;
                break;
            default:
                break;

        } 
    }

    check_flag = 1;

    while(1)
    {
        //当用户输入0时返回上层菜单
        scanf("%d", &check_flag);
        getchar();
        if(check_flag == 0)
        {
            printf("查询结束，返回上层菜单\n");
            break;
        }
    }

}

//实现菜单上的功能2
void sel_relay()
{
    char ch[3];
    MYSQL_RES * result;
    MYSQL_ROW row; 
    char sel_buf[MAX_BUF];
    unsigned short addr = 0;
    socket_type sockety;

    //查询继电器对应的zigbee节点的短地址
    sprintf(sel_buf, "select end_addr from end_device where sensor_id=%d", 6);
    mysql_query(&mysql, sel_buf);
    result = mysql_store_result(&mysql);//将查询的全部结果读取到客户端  
    if((row = mysql_fetch_row(result)) != NULL)
        addr = atoi(row[0]);

    //如果存在继电器节点，则循环等待用户输入控制指令
    if(addr)
    {
        cc_flag[6] = 1;

        while(1)
        {
            printf("继电器已连接:\n");
            printf("请输入指令：");
            scanf("%s", ch);
            getchar();
            //若用户输入的是00则返回上层菜单
            if(ch[0] == '0' && ch[1] == '0')
            {
                printf("返回上层菜单:\n");
                cc_flag[6] = 0;
                break;
            }
            else
            {
                //将控制指令发送到网关
                sockety.End_ID = addr;
                sockety.sensor_type = 6;
                sockety.data[0] = ch[0] ;
                sockety.data[1] = ch[1] ;
                write(client_sockfd, &sockety, sizeof(sockety));
                //延时1秒
                sleep(1);
            }
        }
    }
    else
        printf("没有连接继电器，返回上层菜单\n");

}

//此线程显示菜单，并等待用户选择
void *mythread1(void *threadid)
{
    char ch[3];

    int sel = 0; 
    while(1)
    {
        //列出功能菜单
        printf("1、查看传感器信息。\n");
        printf("2、控制继电器状态。\n");
        printf("3、退出程序。\n");
        printf("请选择您需要的功能，键入对应的数字:");
        //用户输入需要的功能
        scanf("%d", &sel);
        getchar();
        if(sel == 1)
            //实现功能1
            check_sendata();
        else if(sel == 2)
            //实现功能2
            sel_relay();
            //退出程序
        else
        {
            exi = 1;
            break;
        }
    }

    //结束线程
    pthread_exit(0);
}
  
int main()  
{  
    char read_buf[MAX_BUF];     //字符串缓冲区
    int server_sockfd = -1;     //服务器套接字
     
    socklen_t client_len = 0;  
    struct sockaddr_in server_addr;  
    struct sockaddr_in client_addr;      //服务器和客户端的套接字地址

    pthread_t pid1;                     //线程号
    int res;

    socket_type sockty;     //客户端的套接字
  
    MYSQL_RES * result;     //Mysql返回的查询结果
    MYSQL_ROW row;          //MySQL查询结果中的一行数据
    unsigned int end_id;
    int num;

    //初始化MySQL
    mysql_init(&mysql);  
    //连接MySQL上的数据库
    res = mysql_real_connect(&mysql, "127.0.0.1", "root", "xxxxxxxx", NULL, 3306, NULL, 0);
    if(!res)
        printf("connect error!\n");

    //设置MySQL上的gateway数据库
    mysql_query(&mysql, "set names 'utf8'");  
    mysql_query(&mysql, "use gateway");
    //清空下面的2个表
    mysql_query(&mysql, "delete from sensor_data");
    mysql_query(&mysql, "delete from end_device");

    //创建流套接字  
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    //设置服务器接收的连接地址和监听的端口  
    server_addr.sin_family = AF_INET;//指定网络套接字  
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//接受所有IP地址的连接  
    server_addr.sin_port = htons(20001);//绑定到9736端口  
    //绑定（命名）套接字  
    bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));  
    //创建套接字队列，监听套接字  
    listen(server_sockfd, 5);  

    client_len = sizeof(client_addr);  
    //接受客户端连接，创建新的套接字  
    printf("等待网关连接...\n");
    client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
    printf("网关连接成功，进入菜单:\n");

    //创建线程，此线程用于显示菜单
    res = pthread_create(&pid1, NULL, (void *)mythread1, NULL);
    if(res)
    {
        printf("Create pthread error!\n");
        return 1;
    }

    //循环接收网关发送来的数据
    while(1)  
    {  
        if(exi)         //退出循环，结束服务器
            break; 

        //读取网关发来的数据
        memset(read_buf, 0, sizeof(read_buf));
        if(read(client_sockfd, &sockty, sizeof(sockty)) == 0)
            break;
        //将2个8位数据组合成16位数据
        num = ((sockty.data[0]<<8) & 0xff00) + sockty.data[1];
    
        //判断数据，当data[0] = 0xff并且data[1] = 0xff表示发送来的是zigbee节点的地址信息，否则发来的是数据信息
        if(sockty.data[0] == 0xff && sockty.data[1] == 0xff)
        {
            //zigbee节点发来地址信息后，存储到设备表(end_device)
            sprintf(read_buf, "insert into end_device values(null, %d, %d)", sockty.End_ID, sockty.sensor_type);
            mysql_query(&mysql, read_buf);
        }
        else        //接收到的是数据信息，将数据保存到传感器信息表(sensor_data)
        {
            //查询接收到的数据对应的zigbee节点id,节点ID存储在节点表(end_device)
            sprintf(read_buf, "select end_id from end_device where end_addr=%d and sensor_id=%d", sockty.End_ID, sockty.sensor_type);
            mysql_query(&mysql, read_buf);

            //获取查询结果
            result = mysql_store_result(&mysql);//将查询的全部结果读取到客户端  
            row = mysql_fetch_row(result);
             if(row == NULL)
                continue;
            end_id = atoi(row[0]);
            //插入传感器数据到传感器信息表(sensor_data)
            sprintf(read_buf, "insert into sensor_data values(%d, %d, %d, now())", sockty.sensor_type, end_id, num);
            mysql_query(&mysql, read_buf);

            //判断是否向用户显示某节点的信息
            if(check_flag == 1 || sockty.sensor_type == 6)
            {
                //如果用户选择查看对应节点信息，则将从网关发来的传感器数据打印出来
                if(cc_flag[sockty.sensor_type] == 1)
                {
                    //查询传感器对应的中文名
                    sprintf(read_buf, "select sensor_name from sensor where sensor_id=%d", sockty.sensor_type);
                    mysql_query(&mysql, read_buf);
                    result = mysql_store_result(&mysql);//将查询的全部结果读取到客户端  
                    row = mysql_fetch_row(result);

                    //继电器传感器显示的数据格式
                    if( sockty.sensor_type == 6)
                        sprintf(read_buf, "传感器ID: %d, 传感器类型: %s, 继电器%d: %d\n",end_id, row[0], sockty.data[0], sockty.data[1]);
                    //其他传感器显示的数据格式
                    else
                        sprintf(read_buf, "传感器ID: %d, 传感器类型: %s, 数据: %d.%d\n", end_id, row[0], num/10, num % 10);
                    printf("%s", read_buf);
                }
            }
        } 
    }  

    close(client_sockfd);  
    exit(0); 
}  
