/**	@main_thread.c
 *	@brief  多线程实现接收多个客户端的上传/下载文件
 *
 *	@author		
 *	@date		2019/05/18
 *
 *	@note 实现TCP文件上传，下载。
    @note 实现UDP文件上传，下载。
 *	@note  
 */
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"
#include "PassiveServer.h"
#include "sha256.h"

char filepath[MAX_DATA_SIZE]; //文件路径
char filename[MAX_DATA_SIZE]; //文件名
char buffer[BUFF_SIZE];       //file buffer
int recv_len;                 //接收数据长度
int write_len;                //写的数据长度
int file_path_len;            //文件路径的长度

void* process_client_send()  ;               //处理tcp客户端上传文件请求
void* process_client_recv();                 //处理tcp客户端下载文件请求
void* udp_process_client_send();             //处理udp客户端上传文件请求
void* udp_process_client_recv();             //处理udp客户端下载文件请求


/**	@fn	main(int argc,char **argv)
 *	@brief	主函数.
 *	@param	argv[1]	    服务端TCP/UDP连接方式选择 
 *	@return	-1表示写入失败。0表示写入成功。 
 */
int main(int argc,char **argv)
{
   
    int sockfd;
	int client_sock; //tcp连接的客户端id
	Net_packet packet;
	if (2 != argc)
	{
		printf("Usage: %s tcp or udp.\n", argv[0]);
		exit(1);
	}
    
    if(0 == (strcmp("tcp", argv[1]) ))
    {
    	sockfd = create_tcp_server(PORT);
    	if(-1 == sockfd)
        	exit(-1);
    	while(1)
   		{
        	//接受客户端连接
        	socklen_t addrlen = sizeof(struct sockaddr);
        	struct sockaddr_in client_addr;	//客户端地址结构
        	client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
	  
       		if(client_sock < 0)
	   		{
           		printf("accept error\n");
       		}
	   		else
	   		{
           		printf("accept success\n");			
	   		}
			recv(client_sock,&packet, sizeof(packet),0);          //接收数据包，根据packet.data_type判断是上传/下载
			memset(filepath,'\0',sizeof(filepath));
			file_path_len = recv(client_sock,filepath,256,0);//
			printf("filepath :%s\n",filepath);	

	   		if(file_path_len<0)
	   		{
		   		printf("recv filepath error!\n");
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
				strcpy(filename,filepath+(strlen(filepath)-k)+1);  
			    printf("filename :%s\n",filename);
	   		}
	        if(packet.data_type == 'u'| packet.data_type == 'U')
       		{	pthread_t pid;
       			if(pthread_create(&pid, NULL, process_client_send, (void *)&client_sock) < 0) //创建线程处理client的上传文件
	   			{
           			printf("pthread_create error\n");
       			}
	   			sleep(100);  //必须有	 
	   			close(client_sock);
			}
			
			if(packet.data_type == 'd'| packet.data_type == 'D')
			{
				pthread_t tcp_send_tid;
				if (pthread_create(&tcp_send_tid, NULL,process_client_recv, (void *)&client_sock) != 0)
				{
					return 1;		
				}
    			sleep(100);
				close(client_sock);
			
			}
   		 }
    	 close(sockfd);
    }


	if(0 == (strcmp("udp", argv[1]) ))
	{
		sockfd = create_udp_server(PORT);
		if(-1 == sockfd)
			exit(-1);
		
		//接受客户端连接
		//socklen_t addrlen = sizeof(struct sockaddr);
		struct sockaddr_in client_addr; //客户端地址结构
		socklen_t addrlen = sizeof(client_addr);
		while(1)
		{
			recvfrom(sockfd,&packet, sizeof(packet),0,(struct sockaddr*)&client_addr, &addrlen);		  //接收数据包，根据packet.data_type判断是上传/下载

			memset(filepath,'\0',sizeof(filepath));
			file_path_len = recvfrom(sockfd,filepath,256,0,(struct sockaddr*)&client_addr, &addrlen);//
			printf("filepath :%s\n",filepath);	
			
			if(file_path_len<0)
			{
				printf("recv filepath error!\n");
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
				strcpy(filename,filepath+(strlen(filepath)-k)+1);  
				printf("filename :%s\n",filename);
			}
			if(packet.data_type == 'u'| packet.data_type == 'U')
			{	
				pthread_t pid;
				if(pthread_create(&pid, NULL, udp_process_client_send, (void *)&sockfd) < 0) //创建线程处理client的上传文件
				{
					printf("pthread_create error\n");
				}
				sleep(100);  //必须有	 
				
			}
					
			if(packet.data_type == 'd'| packet.data_type == 'D')
			{
				#if 0
				pthread_t tcp_send_tid;
				if (pthread_create(&tcp_send_tid, NULL,udp_process_client_recv, (void *)&sockfd) < 0)
				{
					printf("pthread_create error\n");		
				}
				sleep(100);
				//close(client_sock);
				#else
				int send_len = 0;
				int fd = open(filepath, O_RDONLY,0666); //只读打开文件 
				if (fd < 0)
				{
					perror("file_send_thread");
					return 0;
				}

				/**********发送文件************/
				char buf[BUFF_SIZE];
				int buf_len;
				while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
				{
					if((send_len=sendto(sockfd, buf, buf_len, 0,(struct sockaddr*)&client_addr, addrlen))<0)
					{
						printf("send failed ! \n");
						break;
					}
					printf("send_len: %d \n",send_len);
					bzero(buf,BUFF_SIZE);
					//break;
				}	
				printf("udpsend finished!\n");
				
				char digest[10000];  ////用来存储sha256签名
				printf("SHA-256---Server :Secure Hash Algorithm \n");
				read(fd, buf, sizeof(buf));
				sha_256(digest, (char*)buf);  //sha256 验证
				puts(digest); //打印到窗体
				
				close(fd);	//关闭文件 
				
				#endif
			}
		}
		close(sockfd);
	}

	
    return 0;
}

/**	@fn	void *process_client_send(void * arg) 
 *	@brief	服务器处理客户端TCP上传文件线程 
 *	@param  arg  参数同步，线程创建的时候.
 *	@return	无. 
 */
void *process_client_send(void * arg)
{
	pthread_detach(pthread_self()); //设置分离属性
    int client_sock = *((int *)(arg)); //参数传进来，同步client_sock
	//printf("%d \n",client_sock);
    int fp = open(filename, O_RDWR|O_CREAT|O_TRUNC,0666);
    if(fp!=-1)
    {
        while(recv_len = recv(client_sock,buffer,BUFF_SIZE,0))
        {
           
           if(recv_len<0)
           {
              printf("recv error!\n");
              break;
           }
           printf("recv_len: %d \n",recv_len);
           write_len = write(fp,buffer,recv_len);
           if(write_len < recv_len)
           {
              printf("write error!\n");
              break;
           }
           bzero(buffer,BUFF_SIZE); 
           //memset(buffer,0,sizeof(buffer));
           break; //大文件(超过1024时不要break，这里是为了打印接收完成)
        }
        printf("recv finished!\n");
        close(fp);
    }
   else
   {
    printf("filename is null!\n");
   }
   
}



/**	@fn	void *process_client_recv(void * arg) 
 *	@brief	服务器处理客户端TCP下载文件线程 
 *	@param  arg 参数同步，线程创建的时候.
 *	@return	无. 
 */
void* process_client_recv(void * arg)
{
	pthread_detach(pthread_self());	//设置分离属性	
	int client_sock = *((int *)(arg));  //把参数传进来，创建线程的时候

	//printf("%s:%d\n",__func__,__LINE__);
    int send_len;
	int fd = open(filepath, O_RDONLY,0666); //只读打开文件 
	if (fd < 0)
	{
		perror("file_send_thread");
		return NULL;
	}

	/* 发送文件名(包含路径) */
	//send(client_sock, &filepath, sizeof(filepath), 0); //send file path to client
	
	/**********发送文件************/
	char buf[BUFF_SIZE];
	int buf_len;
	while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
	{
		if((send_len=send(client_sock, buf, buf_len, 0))<0)
		{
			printf("send failed ! \n");
			break;
		}
		printf("send_len: %d \n",send_len);
		bzero(buf,BUFF_SIZE);
		//break;
	}	
	printf("send finished!\n");
	close(fd);	//关闭文件 
	//return NULL;
}



/**	@fn	void *udp_process_client_send(void * arg) 
 *	@brief	服务器处理客户端UDP上传文件线程 
 *	@param  arg  参数同步，线程创建的时候.
 *	@return	无. 
 */
void *udp_process_client_send(void * arg)
{
	pthread_detach(pthread_self()); //设置分离属性
    int udp_client_sock = *((int *)(arg)); //参数传进来，同步client_sock

	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;				   //Address Family
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_LOOPBACK;0.0.0.0
	server_addr.sin_port = htons(PORT); 		//Port number
	//inet_pton(AF_INET, SERVER, &server_addr.sin_addr);
	socklen_t addrlen = sizeof(server_addr);
	//printf("%d \n",udp_client_sock);
    int fp = open(filename, O_RDWR|O_CREAT|O_TRUNC,0666);
    if(fp!=-1)
    {
        while(recv_len = recvfrom(udp_client_sock,buffer,BUFF_SIZE,0,(struct sockaddr*)&server_addr, &addrlen))
        {
           
           if(recv_len<0)
           {
              printf("recv error!\n");
              break;
           }
           printf("recv_len: %d \n",recv_len);
           write_len = write(fp,buffer,recv_len);
           if(write_len < recv_len)
           {
              printf("write error!\n");
              break;
           }
           bzero(buffer,BUFF_SIZE); 
           //memset(buffer,0,sizeof(buffer));
           break;//为了打印接收完成，大文件传输时可不加
        }
        printf("udprecv finished!\n");
        close(fp);
    }
   else
   {
    printf("filename is null!\n");
   }
   
}




/**	@fn	void *udp_process_client_send(void * arg) 
 *	@brief	服务器处理客户端UDP下载文件线程 
 *	@param  arg  参数同步，线程创建的时候.
 *	@return	无. 
 */
void *udp_process_client_recv(void * arg)
{
	pthread_detach(pthread_self()); //设置分离属性
    int udp_client_sock = *((int *)(arg)); //参数传进来，同步client_sock

	int send_len;
	int fd = open(filepath, O_RDONLY,0666); //只读打开文件 
	if (fd < 0)
	{
		perror("file_send_thread");
		return NULL;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;				   //Address Family
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_LOOPBACK;0.0.0.0
	server_addr.sin_port = htons(PORT); 		//Port number
	socklen_t addrlen = sizeof(server_addr);
	
	/**********发送文件************/
	char buf[BUFF_SIZE];
	int buf_len;
	while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
	{
		if((send_len=sendto(udp_client_sock, buf, buf_len, 0,(struct sockaddr*)&server_addr, addrlen))<0)
		{
			printf("send failed ! \n");
			break;
		}
		printf("send_len: %d \n",send_len);
		bzero(buf,BUFF_SIZE);
		//break;
	}	
	printf("udpsend finished!\n");
	close(fd);	//关闭文件 
   
}







