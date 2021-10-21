#ifndef _EPD_2IN3_DISPLAY_H_
#define _EPD_2IN3_DISPLAY_H_

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "cJSON.h"
// #include "ImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()

#define INTERVAL 3 //间隔
#define TASKBARH_HIGH 16
#define TASKBARH_IMAGE 16

#define TIME_FONT_WIDTH Font16.Width
#define TIME_FONT_HEIGHT Font16.Height
#define TIME_LEN 5


#define wind_width  160
#define wind_high   72 

// #define TASKBARH_HIGH  16    //状态栏的高度
// #define TASKBARH_IMAGE  16    //状态栏中图片的高度

// #define TIME_FONT  Font16
// #define TIME_LEN 5



PAINT_TIME sPaint_time;
extern uint8_t NPT_FLAG;

extern UBYTE *BlackImage;
void EPD_init(void);
void EPD_start(void);
void EPD_display_text(char *json_str, uint16_t str_len);
void EPD_display_refresh(char *json_str, uint16_t str_len);
void show_text(uint8_t x, uint8_t y, char *test);
#endif
