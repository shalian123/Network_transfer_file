#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define SERVER "10.1.74.178"  //连接服务器的IP
#define PORT 6464             //连接服务器的端口
#define BUFF_SIZE 1024        //读写数据缓冲区长度
#define MAX_DATA_SIZE 256     //最大文件路径长度 


/* 网络协议包 */
typedef struct NET_PACKET
{
	/* 发送方地址 */
	int src_ip;
	int src_port;
	
	/* 接收方地址 */
	int dst_ip;
	int dst_port;
	
	/* 数据类型 */
	char data_type;           //U---上传；D---下载      
	
	
} Net_packet;






#endif

