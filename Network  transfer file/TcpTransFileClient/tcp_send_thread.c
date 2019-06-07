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
 *	@brief	客户端TCP上传文件线程 
 *	@param  arg  参数同步，线程创建的时候.
 *	@return	无. 
 */
void* tcp_send_thread(void * arg)
{
	pthread_detach(pthread_self());	//设置分离属性	
	int tcp_fd = *((int *)(arg));  //把参数传进来，创建线程的时候

	printf("%s:%d\n",__func__,__LINE__);

	/* 读取要上传的文件路径 */
	char path[MAX_DATA_SIZE];
	printf("please input upload file path\n");
	scanf("%s", path);

	/* 只读打开文件 */
	int fd = open(path, O_RDONLY,0666);
	if (fd < 0)
	{
		perror("file_send_thread");
		return NULL;
	}

	/* 发送文件路径名(包含路径) */
	send(tcp_fd, &path, sizeof(path), 0); //send file path to server
	
	/* 发送文件 */
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
	/* 关闭文件 */
	close(fd);	
	return NULL;
}










