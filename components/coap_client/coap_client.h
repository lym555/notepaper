#ifndef _COAP_WEATHER_H_
#define _COAP_WEATHER_H_

// void coap_weather_start(void);
void get_ntp(void);
void get_weather_3d(void);
void get_weather_now(void);


struct 
{
    uint8_t flag;
    char code[4];
    char temp[4];
    char icon[4];
    char text[16];
    char humidity[4];
}weather_now;


#endif