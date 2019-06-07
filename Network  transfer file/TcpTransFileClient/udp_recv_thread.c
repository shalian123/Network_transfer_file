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
#include "Socket.h"
#include "protocol.h"
#include "sha256.h"
#include <stdlib.h>

/**	@fn	void *tcp_recv_thread(void * arg) 
 *	@brief	�ͻ���UDP�����ļ��߳� 
 *	@param  arg  ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void* udp_recv_thread(void *arg)
{	
	pthread_detach(pthread_self());	//���÷�������
	int udp_fd = *(int*)arg;

	struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;                  //Address Family
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_LOOPBACK;0.0.0.0
    server_addr.sin_port = htons(PORT);         //Port number
    //inet_pton(AF_INET, SERVER, &server_addr.sin_addr);
	socklen_t addrlen = sizeof(server_addr);
	
	char filepath[MAX_DATA_SIZE];
	char filename[MAX_DATA_SIZE];
	int recv_len=0;
	printf("please input download file path\n");
	scanf("%s", filepath);
	int file_path_len = sendto(udp_fd, &filepath, sizeof(filepath), 0,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)); //send download file path to server
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
		strcpy(filename,filepath+(strlen(filepath)-k)+1);            //���ļ�·���л�ȡ�ļ���
		//printf("download file name :%s\n",filename);
	}

	int fd = open(filename, O_RDWR|O_CREAT|O_TRUNC,0666);
	if (fd < 0)
	{   
		perror("file_recv_thread: ");
		return NULL;
	}

	/*�����ļ�*/
	char buf[BUFF_SIZE];
	char digest[10000];  ////�����洢sha256ǩ��
	//recv_len = recvfrom(udp_fd, buf, sizeof(buf), 0,NULL, NULL);

	while ((recv_len = recvfrom(udp_fd, buf, sizeof(buf), 0,(struct sockaddr*)&server_addr, &addrlen)) > 0)
	{ 
	  
		write(fd, buf, recv_len);
		
		bzero(buf,BUFF_SIZE); 
		break;//���ļ�������
	}
	
	printf("client_recv finished!\n");

	printf("SHA-256---Client: Secure Hash Algorithm \n");
	read(fd, buf, sizeof(buf));
	sha_256(digest, (char*)buf);  //sha256 ��֤
	puts(digest); //��ӡ������

	close(fd);
	return NULL;
}


