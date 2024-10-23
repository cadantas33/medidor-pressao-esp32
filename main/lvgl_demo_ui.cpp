/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"
#include "cBMP280.h"

cBMP280 BMP280;

void example_lvgl_demo_ui(lv_disp_t *disp)
{
    
    float temp = BMP280.getTemperature();
    
    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);
    lv_obj_t *labelBMP280Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelBMP280Temp, LV_LABEL_LONG_SCROLL_CIRCULAR);
    //lv_label_set_text_fmt(labelBMP280Temp, "%6.0f", temp);
    lv_label_set_text(labelBMP280Temp, "Hello");
    lv_obj_set_width(labelBMP280Temp, 128);
    lv_obj_align(labelBMP280Temp, LV_ALIGN_TOP_MID, 0, 16);

}
