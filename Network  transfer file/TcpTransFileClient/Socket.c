#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "Socket.h"


/**	@fn	connectsock(char* server_ip, int server_port, int type)
 *	@brief	���ӷ�����
 *	@param	server_ip	������IP 
 *	@param	server_port �������˿�.
 *	@param	type	    TCP/UDP���ӷ�ʽѡ�� 
 *	@return	-1��ʾд��ʧ�ܡ�sock_fd��ʾд��ɹ��� 
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

/***********��ע*************
 UDP�ɶ�ε���connect��������;:
 1,ָ��һ���µ�ip&port����. 
 2,�Ͽ���֮ǰ��ip&port������.
 TCPֻ�ܵ���һ��
*****************************/
int connect_udp(char* server_ip, int server_port)
{
    return connectsock(server_ip, server_port, SOCK_DGRAM); 
}