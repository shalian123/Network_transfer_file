#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "PassiveServer.h"



#define LISTEN_SIZE 20

/**	@fn	start_server(int port, int type)
 *	@brief	����������
 *	@port   �������˿�.
 *	@param	type	    TCP/UDP���ӷ�ʽѡ�� 
 *	@return	-1��ʾд��ʧ�ܡ�sock_fd��ʾд��ɹ��� 
 */

int start_server(int port, int type)
{
    //�����������׽���
    int sock_fd = socket(AF_INET, type, 0);
    if(sock_fd < 0)
	{
        printf("create socket error\n");
        return -1;
    }

    int reuse = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &reuse, sizeof(int));  //SO_REUSEADDR�������ñ��ص�ַ�Ͷ˿ڣ�SO_BROADCAST�������͹㲥
    struct sockaddr_in server_addr;	
    bzero(&server_addr, sizeof(struct sockaddr_in)); 
    server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(port); 
    if(bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
        perror("bind error\n");
        return -1;
    }
    //TCP
    if(SOCK_STREAM == type)
	{
      
        if(listen(sock_fd, LISTEN_SIZE) < 0)
		{
            printf("listen error\n");
            return -1;
        }
        printf("tcp server start\n");
    }
    else
        printf("udp server start\n");
    return sock_fd;
}

int create_tcp_server(int port)
{
    start_server(port, SOCK_STREAM);
}

int create_udp_server(int port)
{
    start_server(port, SOCK_DGRAM);
}
