#ifndef SHA256_H
#define SHA256_H

// 警告，输入消息不能超过这个长度 
#define maxlen 1000000

typedef unsigned int uint;
typedef unsigned char ubyte;
typedef unsigned long long ull;


inline uint rrot(uint val, int pos) ;

void sha_256( char *digest,  char *str) ;

#endif

