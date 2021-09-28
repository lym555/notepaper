#ifndef _EPD_TEST_H_
#define _EPD_TEST_H_

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "cJSON.h"
// #include "ImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()

void ped_app_start(void);
void ped_display_text(cJSON *json);
#endif
