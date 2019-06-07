/**	@main_thread.c
 *	@brief  ���߳�ʵ�ֽ��ն���ͻ��˵��ϴ�/�����ļ�
 *
 *	@author		
 *	@date		2019/05/18
 *
 *	@note ʵ��TCP�ļ��ϴ������ء�
    @note ʵ��UDP�ļ��ϴ������ء�
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

char filepath[MAX_DATA_SIZE]; //�ļ�·��
char filename[MAX_DATA_SIZE]; //�ļ���
char buffer[BUFF_SIZE];       //file buffer
int recv_len;                 //�������ݳ���
int write_len;                //д�����ݳ���
int file_path_len;            //�ļ�·���ĳ���

void* process_client_send()  ;               //����tcp�ͻ����ϴ��ļ�����
void* process_client_recv();                 //����tcp�ͻ��������ļ�����
void* udp_process_client_send();             //����udp�ͻ����ϴ��ļ�����
void* udp_process_client_recv();             //����udp�ͻ��������ļ�����


/**	@fn	main(int argc,char **argv)
 *	@brief	������.
 *	@param	argv[1]	    �����TCP/UDP���ӷ�ʽѡ�� 
 *	@return	-1��ʾд��ʧ�ܡ�0��ʾд��ɹ��� 
 */
int main(int argc,char **argv)
{
   
    int sockfd;
	int client_sock; //tcp���ӵĿͻ���id
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
        	//���ܿͻ�������
        	socklen_t addrlen = sizeof(struct sockaddr);
        	struct sockaddr_in client_addr;	//�ͻ��˵�ַ�ṹ
        	client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
	  
       		if(client_sock < 0)
	   		{
           		printf("accept error\n");
       		}
	   		else
	   		{
           		printf("accept success\n");			
	   		}
			recv(client_sock,&packet, sizeof(packet),0);          //�������ݰ�������packet.data_type�ж����ϴ�/����
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
       			if(pthread_create(&pid, NULL, process_client_send, (void *)&client_sock) < 0) //�����̴߳���client���ϴ��ļ�
	   			{
           			printf("pthread_create error\n");
       			}
	   			sleep(100);  //������	 
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
		
		//���ܿͻ�������
		//socklen_t addrlen = sizeof(struct sockaddr);
		struct sockaddr_in client_addr; //�ͻ��˵�ַ�ṹ
		socklen_t addrlen = sizeof(client_addr);
		while(1)
		{
			recvfrom(sockfd,&packet, sizeof(packet),0,(struct sockaddr*)&client_addr, &addrlen);		  //�������ݰ�������packet.data_type�ж����ϴ�/����

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
				if(pthread_create(&pid, NULL, udp_process_client_send, (void *)&sockfd) < 0) //�����̴߳���client���ϴ��ļ�
				{
					printf("pthread_create error\n");
				}
				sleep(100);  //������	 
				
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
				int fd = open(filepath, O_RDONLY,0666); //ֻ�����ļ� 
				if (fd < 0)
				{
					perror("file_send_thread");
					return 0;
				}

				/**********�����ļ�************/
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
				
				char digest[10000];  ////�����洢sha256ǩ��
				printf("SHA-256---Server :Secure Hash Algorithm \n");
				read(fd, buf, sizeof(buf));
				sha_256(digest, (char*)buf);  //sha256 ��֤
				puts(digest); //��ӡ������
				
				close(fd);	//�ر��ļ� 
				
				#endif
			}
		}
		close(sockfd);
	}

	
    return 0;
}

/**	@fn	void *process_client_send(void * arg) 
 *	@brief	����������ͻ���TCP�ϴ��ļ��߳� 
 *	@param  arg  ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void *process_client_send(void * arg)
{
	pthread_detach(pthread_self()); //���÷�������
    int client_sock = *((int *)(arg)); //������������ͬ��client_sock
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
           break; //���ļ�(����1024ʱ��Ҫbreak��������Ϊ�˴�ӡ�������)
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
 *	@brief	����������ͻ���TCP�����ļ��߳� 
 *	@param  arg ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void* process_client_recv(void * arg)
{
	pthread_detach(pthread_self());	//���÷�������	
	int client_sock = *((int *)(arg));  //�Ѳ����������������̵߳�ʱ��

	//printf("%s:%d\n",__func__,__LINE__);
    int send_len;
	int fd = open(filepath, O_RDONLY,0666); //ֻ�����ļ� 
	if (fd < 0)
	{
		perror("file_send_thread");
		return NULL;
	}

	/* �����ļ���(����·��) */
	//send(client_sock, &filepath, sizeof(filepath), 0); //send file path to client
	
	/**********�����ļ�************/
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
	close(fd);	//�ر��ļ� 
	//return NULL;
}



/**	@fn	void *udp_process_client_send(void * arg) 
 *	@brief	����������ͻ���UDP�ϴ��ļ��߳� 
 *	@param  arg  ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void *udp_process_client_send(void * arg)
{
	pthread_detach(pthread_self()); //���÷�������
    int udp_client_sock = *((int *)(arg)); //������������ͬ��client_sock

	
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
           break;//Ϊ�˴�ӡ������ɣ����ļ�����ʱ�ɲ���
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
 *	@brief	����������ͻ���UDP�����ļ��߳� 
 *	@param  arg  ����ͬ�����̴߳�����ʱ��.
 *	@return	��. 
 */
void *udp_process_client_recv(void * arg)
{
	pthread_detach(pthread_self()); //���÷�������
    int udp_client_sock = *((int *)(arg)); //������������ͬ��client_sock

	int send_len;
	int fd = open(filepath, O_RDONLY,0666); //ֻ�����ļ� 
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
	
	/**********�����ļ�************/
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
	close(fd);	//�ر��ļ� 
   
}







