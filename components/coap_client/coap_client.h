#ifndef _COAP_WEATHER_H_
#define _COAP_WEATHER_H_

/**>说明：
 * "coap://api.gaojulong.com/weather/now?  可以换成自己局域网中的Python服务程序
 * location=117.282488,31.775297 所在地域的坐标，可以在  https://api.map.baidu.com/lbsapi/getpoint/index.html?qq-pf-to=pcqq.group查询
 * &key=9e54b1e3d00e4f36b813065e30b0eec7    在和风平台中更换自己的Key  https://dev.qweather.com/docs/api/weather/weather-daily-forecast/
 * &lang=en" 目前仅支持英文
*/
#define COAP_WEATHER_NOW_URL "coap://api.gaojulong.com/weather/now?location=117.282488,31.775297&key=9e54b1e3d00e4f36b813065e30b0eec7&lang=en"
#define COAP_WEATHER_3D_URL "coap://api.gaojulong.com/weather/3d?location=117.282488,31.775297&key=9e54b1e3d00e4f36b813065e30b0eec7&lang=en"
#define COAP_NTP_URI "coap://api.gaojulong.com/ntp"

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