#ifndef _EPD_2IN3_DISPLAY_H_
#define _EPD_2IN3_DISPLAY_H_

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "cJSON.h"
// #include "ImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()

void EPD_init(void);
void EPD_start(void);
void EPD_display_text(char *json_str, uint16_t str_len);
void EPD_display_refresh(char *json_str, uint16_t str_len);
void show_text(uint8_t x, uint8_t y, char *test);
#endif
