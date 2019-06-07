#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "Socket.h"


/**	@fn	connectsock(char* server_ip, int server_port, int type)
 *	@brief	连接服务器
 *	@param	server_ip	服务器IP 
 *	@param	server_port 服务器端口.
 *	@param	type	    TCP/UDP连接方式选择 
 *	@return	-1表示写入失败。sock_fd表示写入成功。 
 */
int connectsock(char* server_ip, int server_port, int type)
{
    int sock_fd = socket(AF_INET, type, 0);
    if(-1 == sock_fd)
	{
        printf("create socket error\n");
        return -1;
    }

    struct sockaddr_in server_addr;
  
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                  //Address Family
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_LOOPBACK;0.0.0.0
    server_addr.sin_port = htons(server_port);         //Port number
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if(-1 == connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)))
	{
        printf("connect server error\n");
        return -1;
    }

    printf("connect server success\n");
    return sock_fd;
}

int connect_tcp(char* server_ip, int server_port)
{
    return connectsock(server_ip, server_port, SOCK_STREAM);
}

/***********备注*************
 UDP可多次调用connect有两种用途:
 1,指定一个新的ip&port连结. 
 2,断开和之前的ip&port的连结.
 TCP只能调用一次
*****************************/
int connect_udp(char* server_ip, int server_port)
{
    return connectsock(server_ip, server_port, SOCK_DGRAM); 
}