#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__


/* tcp�ļ������߳� */
extern void* tcp_send_thread(void *tcp_fd);

/* tcp�ļ������߳� */
extern void* tcp_recv_thread(void *tcp_fd); 

/* udp�ļ������߳� */
extern void* udp_send_thread(void *tcp_fd);

/* udp�ļ������߳� */
extern void* udp_recv_thread(void *tcp_fd); 


#endif

