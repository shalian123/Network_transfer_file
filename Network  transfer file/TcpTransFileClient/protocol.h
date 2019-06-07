#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define SERVER "10.1.74.178"  //���ӷ�������IP
#define PORT 6464             //���ӷ������Ķ˿�
#define BUFF_SIZE 1024        //��д���ݻ���������
#define MAX_DATA_SIZE 256     //����ļ�·������ 


/* ����Э��� */
typedef struct NET_PACKET
{
	/* ���ͷ���ַ */
	int src_ip;
	int src_port;
	
	/* ���շ���ַ */
	int dst_ip;
	int dst_port;
	
	/* �������� */
	char data_type;           //U---�ϴ���D---����      
	
	
} Net_packet;






#endif

