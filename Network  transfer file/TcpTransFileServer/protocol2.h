#ifndef     _PROTOCOL2_H
#define     _PROTOCOL2_H

#define		SER_PORT		6464
#define     BRO_ADDR		"255.255.255.255"

#define 	CMD_HEAD 		0x1234

#define 	MAX_RETRY		3
#define 	RESEND_TIME_US 	500000  //500ms


//commands between server and client 
typedef  enum
{
	DISCOVERY = 0x1000, //magic num
	ACK,
	TCP,
	UDP,
	UPLOAD,
	DOWNLOAD,	
} cmd_type;

//命令交互包格式
typedef struct 
{
	int head;
    cmd_type type;    		//DISCOVERY, ACK, TCP, UDP
    int pack_size;			//purpose for verification
	int retry;				//the max times of retry is 3
	struct _file_cmd
	{
		cmd_type protol; 	//TCP, UDP
		char name[100];		//file name 
	}file_cmd;		
} cmd_pack;


#endif