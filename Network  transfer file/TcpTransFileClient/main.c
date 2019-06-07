#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "protocol.h"
#include "Socket.h"
#include "tcp_client.h"


/**	@fn	main(int argc,char **argv)
 *	@brief	主函数.
 *	@param	argv[1]	  客户端TCP/UDP连接方式选择 
 *	@return	-1表示写入失败。0表示写入成功。 
 */
int main(int argc,char **argv)
{
	Net_packet packet; //packet.data_type  U--上传；D--下载；
	if (2 != argc)
	{
		printf("Usage: %s tcp or udp.\n", argv[0]);
		exit(1);
	}


	
#if 1
	/************************ tcp *******************************/
    if(0 == (strcmp("tcp", argv[1]) ))
    {
    	int tcp_fd = connect_tcp(SERVER, PORT);  //创建连接
    	if(-1 == tcp_fd)
        {
        	return -1;
		}
		else
		{  
		    printf("\033[33m/*****************************************************************************************/\n");
			printf("\033[33m/********************\033[35mCongratulation! You have connected to the TCP server!\033[33m****************/\n");
            printf("/*******************  \033[35m Please input cmd：\033[37mU ---- Upload;  D ---- Download   \033[33m***************/\n");
			printf("/*****************************************************************************************/\n");

			scanf("%c",&packet.data_type);            //输入指令，上传或下载
			send(tcp_fd, &packet, sizeof(packet), 0); //发送指令
			if(packet.data_type == 'U'|packet.data_type == 'u')
			{
				// tcp文件发送线程 	
				pthread_t tcp_send_tid;
				if (pthread_create(&tcp_send_tid, NULL,tcp_send_thread, (void *)&tcp_fd) != 0)
				{
					return 1;		
				}
    			sleep(100);
			}
	    	if(packet.data_type == 'D'|packet.data_type == 'd')
			{
				// tcp文件下载线程	
				pthread_t tcp_recv_tid;
				if (pthread_create(&tcp_recv_tid, NULL, tcp_recv_thread, (void *)&tcp_fd) != 0)
				{
					return 1;		
				}
				sleep(100);
			}
			
        }
		close(tcp_fd);
    }
	/************************ end tcp *******************************/
#endif

	/************************ udp *******************************/
	if(0 == (strcmp("udp", argv[1]) ))
	{
		int udp_fd = connect_udp(SERVER, PORT);  //创建连接
    	if(-1 == udp_fd)
        {
        	return -1;
		}

		else
		{  
		    printf("\033[33m/*****************************************************************************************/\n");
			printf("\033[33m/********************\033[35mCongratulation! You have connected to the UDP server!\033[33m****************/\n");
            printf("/*******************  \033[35m Please input cmd：\033[37mU ---- Upload;  D ---- Download   \033[33m***************/\n");
			printf("/*****************************************************************************************/\n");

			scanf("%c",&packet.data_type);            //输入指令，上传或下载
			send(udp_fd, &packet, sizeof(packet), 0); //发送指令
			if(packet.data_type == 'U'|packet.data_type == 'u')
			{
				// tcp文件发送线程 	
				pthread_t udp_send_tid;
				if (pthread_create(&udp_send_tid, NULL,udp_send_thread, (void *)&udp_fd) != 0)
				{
					return 1;		
				}
    			sleep(100);
			}
	    	if(packet.data_type == 'D'|packet.data_type == 'd')
			{
				// udp文件下载线程	
				pthread_t udp_recv_tid;
				if (pthread_create(&udp_recv_tid, NULL, udp_recv_thread, (void *)&udp_fd) != 0)
				{
					return 1;		
				}
				sleep(100);
			}
			
        }
		close(udp_fd);
	}
	return 0;
}


