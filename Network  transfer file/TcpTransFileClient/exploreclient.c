#include <sys/types.h>   
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include "protocol2.h"


#define  MAXNUM_SER	 30   							//the max num of server
static struct sockaddr_in  ser_addr[MAXNUM_SER]; 	
static int server_total_num;		
static struct sockaddr_in  ser_connct;				//the curret connect server

/**	@fn	int server_discovery(void)
 *	@brief	查找服务器
 *	
 *	@param	无
 *	@return	0表示写入失败。1表示写入成功。 
 */
int server_discovery(void)
{
	server_total_num = 0;
	int sockfd = -1;
	int ret = -1;
	const int optval = 1;
	struct sockaddr_in	data_from_addr;
	socklen_t data_from_addrlen;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sockfd)
    {
        perror("sockfd");
        exit(-1);
    }

	//set the mode of sockfd is broadcast
	ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
    if (-1 == ret)
    {
        perror("setsockopt");
		close(sockfd);
        exit(-1);
    }
	cmd_pack discovery_pack = 	
	{
		.head		= 	CMD_HEAD,
		.type 		= 	DISCOVERY,
		.pack_size 	= 	sizeof(cmd_pack),
		.retry		=  	0,
	};
	
	cmd_pack ack_pack;
	//bzero(&ack_pack, sizeof(ack_pack));

	//set broadcast destination ip address
	struct sockaddr_in sockaddr_bro;
	bzero(&sockaddr_bro, sizeof(sockaddr_bro));
    sockaddr_bro.sin_family         = AF_INET;
    sockaddr_bro.sin_port           = htons(SER_PORT);
    sockaddr_bro.sin_addr.s_addr    = inet_addr(BRO_ADDR);

	//set local ip address
	struct sockaddr_in sockaddr_local;
	bzero(&sockaddr_local, sizeof(sockaddr_local));
    sockaddr_local.sin_family = AF_INET;
    sockaddr_local.sin_port   = 0;
    sockaddr_local.sin_addr.s_addr = inet_addr("0.0.0.0");

	if (-1 == bind(sockfd, (const struct sockaddr*)&sockaddr_local, sizeof(sockaddr_local)))
	{
		perror("bind");
		close(sockfd);
		exit(-1);
	}

	fd_set rset;
	FD_ZERO(&rset);
	struct timeval timeout =
	{
		.tv_sec = 0,
		.tv_usec = RESEND_TIME_US,
	};
	for (discovery_pack.retry = 0; discovery_pack.retry < MAX_RETRY; discovery_pack.retry++)
	{
		ret = sendto(sockfd, &discovery_pack, sizeof(discovery_pack), 0, 
		(const struct sockaddr*)&sockaddr_bro, sizeof(sockaddr_bro));
		if (-1 == ret)
        {
            perror("sendto");
			close(sockfd);
            exit(-1);
        }
		//printf("the num bytes send is %d.\n", ret);
		FD_SET(sockfd, &rset);
		select(sockfd + 1, &rset, NULL, NULL, &timeout);
		timeout.tv_sec = 0;
		timeout.tv_usec = RESEND_TIME_US;
		int i = 0;
		for(i = 0; i < MAXNUM_SER; i++)
			{ 	
				if (FD_ISSET(sockfd, &rset))
				{	
					
					bzero(&ack_pack, sizeof(ack_pack));
					recvfrom(sockfd, &ack_pack, sizeof(ack_pack), 0, 
						(struct sockaddr *)&(ser_addr[server_total_num]), &data_from_addrlen);
					if (ack_pack.head == CMD_HEAD && ack_pack.type == ACK && ack_pack.pack_size == sizeof(cmd_pack))
					{	
						server_total_num++;
						discovery_pack.retry = 0;
					}
					FD_SET(sockfd, &rset);
					select(sockfd + 1, &rset, NULL, NULL, &timeout);
					timeout.tv_sec = 0;
					timeout.tv_usec = RESEND_TIME_US;
					
				}	
			}
		if (server_total_num > 0)
		{
			return 1;
		}
		
		printf("retry to send discovery_pack\n");
	}
	if (discovery_pack.retry >= MAX_RETRY)
	{
		return 0;
	}
}


int main(int argc, char** argv)
{
	int ret = -1;
	int i;
	ret = server_discovery();
	if (ret == 0)
	{
		printf("No Server Available.\n");
		while (!server_discovery());  //wait Until there is a server available 
	}
	if (ret == 1)
	{
		for (i = 0; i < server_total_num; i++)
		{
			printf("Available server number and ip address: \n");
			printf("#%d, ip:[%s]\n", i, inet_ntoa(ser_addr[i].sin_addr));
		}
	}
	
	while (1)
	{
		printf("Please enter a digital number to select the server or enter any single non-digital character refresh server:");
		setbuf(stdin, NULL);
		ret  = scanf("%d", &i);
		if (ret == 0)
		{
			while (!server_discovery()); 
			for (i = 0; i < server_total_num; i++)
			{
				printf("Available server number and ip address: \n");
				printf("#%d, ip:[%s]\n", i, inet_ntoa(ser_addr[i].sin_addr));
			}
			continue;
		}
		if (i < 0 || i >= server_total_num)
		{
			printf("Input error! Available server number range is [0, %d)\n", server_total_num);
			continue;
		}
		else
		{
			memcpy (&ser_connct, &ser_addr[i], sizeof(struct sockaddr_in));
			printf("Server of your choice,IP address: %s, Port number: %d.\n", 
			inet_ntoa(ser_connct.sin_addr),ntohs(ser_connct.sin_port));
			break;
		}
	}
	printf("input ");
	return 0;
}






















