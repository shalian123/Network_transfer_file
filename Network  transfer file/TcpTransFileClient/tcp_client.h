#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__


/* tcp文件发送线程 */
extern void* tcp_send_thread(void *tcp_fd);

/* tcp文件接收线程 */
extern void* tcp_recv_thread(void *tcp_fd); 

/* udp文件发送线程 */
extern void* udp_send_thread(void *tcp_fd);

/* udp文件接收线程 */
extern void* udp_recv_thread(void *tcp_fd); 


#endif

