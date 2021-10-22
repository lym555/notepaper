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

struct weather
{
    char fxDate[16];
    char tempMin[4];
    char tempMax[4];
    char iconDay[4];
    char textDay[16];
};


struct 
{
    uint8_t flag;
    char code[4];
    struct weather d1;
    struct weather d2;
    struct weather d3;
}weather_3d;


#endif