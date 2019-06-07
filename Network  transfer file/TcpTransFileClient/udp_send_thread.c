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
 *	@brief	客户端udp上传文件线程 
 *	@param  arg  参数同步，线程创建的时候.
 *	@return	无. 
 */
void* udp_send_thread(void * arg)
{
	pthread_detach(pthread_self());	//设置分离属性	
	int udp_fd = *((int *)(arg));  //把参数传进来，创建线程的时候

	struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;                  //Address Family
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_LOOPBACK;0.0.0.0
    server_addr.sin_port = htons(PORT);         //Port number
    inet_pton(AF_INET, SERVER, &server_addr.sin_addr);

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
	sendto(udp_fd, &path, sizeof(path), 0,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)); //send file path to server
	
	/* 发送文件 */
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
	/* 关闭文件 */
	close(fd);	
	return NULL;
}

