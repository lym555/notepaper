#include "stdio.h"
#include "string.h"
char* substring(char* ch,int pos,int length) 
{
    char* pch=ch;
    char* subch=(char*)malloc((1+length-pos)*sizeof(char)); 
    pch+=pos;
    for(int i=0;i<length;i++) 
    {
        subch[i]=*(pch++);
    }
    subch[length]='\0';//加上字符串结束符
    return subch;  
}

// int substring(char*  dst, char* src,int pos,int length) 
// {
//     char* q=src;
//     char* p = dst;
//     uint16_t size = strlen(src);
//     // char* subch=(char*)malloc((1+length-pos)*sizeof(char)); 
//     if ((pos+length) > size)
//         return 0;
//     q+=pos;
//     for(int i=0;i<length;i++) 
//     {
//         *(p++)=*(q++);
//     }
//     *(p++)='\0';//加上字符串结束符
//     return 1;  
// }
// uint8_t substring(char *dst, char *src, uint16_t cp_start, uint16_t cp_size)
// {
//     if(dst == NULL) 
//         return 0;
//     char *p = dst;
//     char *q = src+cp_start;
//     uint16_t size = strlen(src);
//     if ((cp_start+cp_size) > size)
//         return 0;

//     while (cp_size--)
//     {
//         *(p++)=*(q++);
//     }
//     *(p++) = '\0';
//     return 1;

// }