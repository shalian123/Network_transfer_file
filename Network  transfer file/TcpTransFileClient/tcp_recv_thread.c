#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tcp_client.h"
//#include "Socket.h"
#include "protocol.h"
#include "sha256.h"

/**	@fn	void *tcp_recv_thread(void * arg) 
 *	@brief	客户端TCP下载文件线程 
 *	@param  arg  参数同步，线程创建的时候.
 *	@return	无. 
 */
void* tcp_recv_thread(void *arg)
{	
	pthread_detach(pthread_self());	//设置分离属性
	int tcp_fd = *(int*)arg;
	char filepath[MAX_DATA_SIZE];
	char filename[MAX_DATA_SIZE];
	int recv_len=0;
	printf("please input download file path\n");
	scanf("%s", filepath);
	int file_path_len = send(tcp_fd, &filepath, sizeof(filepath), 0); //send download file path to server
	if(file_path_len<0)
	{
		printf("download file path error!\n");
	}
	else
	{
		int i=0,k=0;  
		for(i=strlen(filepath);i>=0;i--)  
		{  
			if(filepath[i]!='/') 	 
			{  
				k++;  
			}  
			else   
				break;	 
		}  
		strcpy(filename,filepath+(strlen(filepath)-k)+1);            //从文件路径中获取文件名
		//printf("download file name :%s\n",filename);
	}
	#if 0
    time_t t;
    struct tm *pt;
    time(&t);
    pt = gmtime(&t);
	printf("[getfile time: %d:%d:%d]\n",8 + pt->tm_hour,pt->tm_min, pt->tm_sec);
    printf("[filename:  %s]\n",filename);
    #endif
	
	int fd = open(filename, O_RDWR|O_CREAT|O_TRUNC,0666);
	if (fd < 0)
	{   
		perror("file_recv_thread: ");
		return NULL;
	}
	/*接收文件*/
	char buf[BUFF_SIZE];
	char digest[10000];  ////用来存储sha256签名
	
	while ((recv_len = recv(tcp_fd, buf, sizeof(buf), 0)) > 0)
	{ 
		write(fd, buf, recv_len);
		bzero(buf,BUFF_SIZE); 
		break;//不能有，否则大包会只能接收到1024
	}
    printf("client_recv finished!\n");
#if 0
	printf("SHA-256---Secure Hash Algorithm \n");
	read(fd, buf, sizeof(buf));
	sha_256(digest, (char*)buf);  //sha256 验证
	puts(digest); //打印到窗体
#endif	
	close(fd);
	return NULL;
}

