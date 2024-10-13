#include <cstdio>

#include "i2c_oled.h"
#include "esp_lvgl_port.h"

#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cbspI2C.h"
#include "cBMP280.h"
#include "cSMP3011.h"

cbspI2C I2CChannel1;
cBMP280 BMP280;
cSMP3011 SMP3011;

#define VBAT_PIN GPIO_NUM_35

int adc_value = 0;
float vbat = 0;

esp_err_t adc1_config_channel_atten(ADC1_CHANNEL_8, ADC_ATTEN_DB_2_5);
esp_err_t adc1_config_width(ADC1_WIDTH_BIT_12);

extern void example_lvgl_demo_ui(lv_disp_t *disp);

extern "C" void app_main()
{
    
    //gpio_set_direction(VBAT_PIN, GPIO_MODE_INPUT);

    i2c_oled_init();

    I2CChannel1.init(I2C_NUM_1, GPIO_NUM_33, GPIO_NUM_32);
    I2CChannel1.openAsMaster(100000);

    BMP280.init(I2CChannel1);
    SMP3011.init(I2CChannel1);

    lvgl_port_lock(0);

    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);

    // Exibir pressao do BMP280 no display
    lv_obj_t *labelBMP280Press = lv_label_create(scr);
    lv_label_set_long_mode(labelBMP280Press, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelBMP280Press, " ");
    lv_obj_set_width(labelBMP280Press, 128);
    lv_obj_align(labelBMP280Press, LV_ALIGN_TOP_MID, 0, 0);

     // Exibir temperatura do BMP280 no display
    lv_obj_t *labelBMP280Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelBMP280Temp, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelBMP280Temp, " ");
    lv_obj_set_width(labelBMP280Temp, 128);
    lv_obj_align(labelBMP280Temp, LV_ALIGN_TOP_MID, 0, 16);

     // Exibir pressao do SMP3011 no display
    lv_obj_t *labelSMP3011Press = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Press, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelSMP3011Press, " ");
    lv_obj_set_width(labelSMP3011Press, 128);
    lv_obj_align(labelSMP3011Press, LV_ALIGN_TOP_MID, 0, 32);

     // Exibir temperatura do SMP3011 no display
    lv_obj_t *labelSMP3011Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Temp, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelSMP3011Temp, " ");
    lv_obj_set_width(labelSMP3011Temp, 128);
    lv_obj_align(labelSMP3011Temp, LV_ALIGN_TOP_MID, 0, 48);

    lvgl_port_unlock();

    while (true)
    {
        adc_value = adc_get_raw(ADC1_CHANNEL_8);

        BMP280.poll();
        SMP3011.poll();

        printf("\nValor ADC: %i", adc_value);

        printf("\rPressao e temperatura: %6.0fPa %6.2fC Pressao SMP: %6.0fPa %6.2fC",
               BMP280.getPressure(), BMP280.getTemperature(),
               SMP3011.getPressure(), SMP3011.getTemperature());

        lvgl_port_lock(0);
        lv_label_set_text_fmt(labelBMP280Press, "%6.2fPa", BMP280.getPressure());
        lv_label_set_text_fmt(labelBMP280Temp, "%6.0fC", BMP280.getTemperature());
        lv_label_set_text_fmt(labelSMP3011Press, "%6.2fPa", SMP3011.getPressure());
        lv_label_set_text_fmt(labelSMP3011Temp, "%6.0fC", SMP3011.getTemperature());
        lvgl_port_unlock();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
