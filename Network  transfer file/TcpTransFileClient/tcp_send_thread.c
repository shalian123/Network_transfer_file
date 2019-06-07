#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tcp_client.h"
#include "Socket.h"
#include "protocol.h"


/**	@fn	void *tcp_send_thread(void * arg) 
 *	@brief	�ͻ���TCP�ϴ��ļ��߳� 
 *	@param  arg  ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void* tcp_send_thread(void * arg)
{
	pthread_detach(pthread_self());	//���÷�������	
	int tcp_fd = *((int *)(arg));  //�Ѳ����������������̵߳�ʱ��

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
	send(tcp_fd, &path, sizeof(path), 0); //send file path to server
	
	/* �����ļ� */
	char buf[BUFF_SIZE];
	int buf_len;
	while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
	{
		if(send(tcp_fd, buf, buf_len, 0)<0)
		{
			printf("send failed ! \n");
			break;
		}
		
	}
	printf("client_send finished!\n");
	/* �ر��ļ� */
	close(fd);	
	return NULL;
}










