/*****************************************************************************
* | File      	:   ImageData.h
* | Author      :   Waveshare team
* | Function    :	
*----------------
* |	This version:   V1.0
* | Date        :   2018-10-23
* | Info        :
*
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

******************************************************************************/

#ifndef _IMAGEDATA_H_
#define _IMAGEDATA_H_
// extern const unsigned char gImage_2in13[];
extern const unsigned char gImage_disconnect[32];
extern const unsigned char gImage_connect[32];
const unsigned char gImage_battery_half[32];
const unsigned char gImage_battery_empty[32];
const unsigned char gImage_battery_full[32];
const unsigned char gImage_battery_charging[32];

//温度和湿度
const unsigned char gImage_humidity_16[32];
const unsigned char gImage_upper_limit_16[32];
const unsigned char gImage_lower_limit_16[32];

//晴天
const unsigned char gImage_clear_32[128];
const unsigned char gImage_clear_48[288];
const unsigned char gImage_clear_64[512];

//大雨
const unsigned char gImage_heavy_rain_32[128];
const unsigned char gImage_heavy_rain_48[288];

//多云
const unsigned char gImage_cloudy_32[128];
const unsigned char gImage_cloudy_48[288];

//阴天
const unsigned char gImage_overcast_32[128];
const unsigned char gImage_overcast_48[288];

//中雨
const unsigned char gImage_moderate_rain_32[128];
const unsigned char gImage_moderate_rain_48[288];

//小雨
const unsigned char gImage_light_rain_32[128];
const unsigned char gImage_light_rain_48[288];

#endif
/* FILE END */


