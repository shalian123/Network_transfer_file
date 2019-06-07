#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


#define PORT 6464           //�������˿ں�

#define MAX_DATA_SIZE 256   //��󳤶�,�ļ�·��
#define BUFF_SIZE 1024      //��д�ļ�����������

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
	char data_type;         //U---�ϴ���D---����
	
} Net_packet;





#endif

