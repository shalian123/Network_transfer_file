#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>
#include "tcp_client.h"
#include "Socket.h"
#include "protocol.h"


/**	@fn	void *udp_send_thread(void * arg) 
 *	@brief	�ͻ���udp�ϴ��ļ��߳� 
 *	@param  arg  ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void* udp_send_thread(void * arg)
{
	pthread_detach(pthread_self());	//���÷�������	
	int udp_fd = *((int *)(arg));  //�Ѳ����������������̵߳�ʱ��

	struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;                  //Address Family
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_LOOPBACK;0.0.0.0
    server_addr.sin_port = htons(PORT);         //Port number
    inet_pton(AF_INET, SERVER, &server_addr.sin_addr);

	printf("%s:%d\n",__func__,__LINE__);

	/* ��ȡҪ�ϴ����ļ�·�� */
	char path[MAX_DATA_SIZE];
	printf("please input upload file path\n");
	scanf("%s", path);

	/* ֻ�����ļ� */
	int fd = open(path, O_RDONLY,0666);
	if (fd < 0)
	{
		perror("file_send_thread");
		return NULL;
	}

	/* �����ļ�·����(����·��) */
	sendto(udp_fd, &path, sizeof(path), 0,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)); //send file path to server
	
	/* �����ļ� */
	char buf[BUFF_SIZE];
	int buf_len;
	while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
	{
		if(sendto(udp_fd, buf, buf_len, 0,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in))<0)
		{
			printf("send failed ! \n");
			break;
		}
		
	}
	printf("udpclient_send finished!\n");
	/* �ر��ļ� */
	close(fd);	
	return NULL;
}

