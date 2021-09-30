#include "stdio.h"

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