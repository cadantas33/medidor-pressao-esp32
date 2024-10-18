#include <cstdio>

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "i2c_oled.h"
#include "esp_lvgl_port.h"

#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "cbspI2C.h"
#include "cBMP280.h"
#include "cSMP3011.h"

#define PSI 0.000145
#define BAR 0.0689

cbspI2C I2CChannel1;
cBMP280 BMP280;
cSMP3011 SMP3011;

//esp_adc_cal_characteristics_t adc1_chars;

// Recebe o tempo atual
int time_offset = esp_timer_get_time();

// Inicializa variáveis para leitura da bateria
int adc_value;
float vbat;

// Inicializa variáveis dos sensores de pressão
float press_bmp280 = (BMP280.getPressure()) * PSI;
float press_smp3011 = (SMP3011.getPressure()) * PSI;

extern void example_lvgl_demo_ui(lv_disp_t *disp);

extern "C" void app_main()
{
    esp_timer_early_init();
    //gpio_set_direction(VBAT_PIN, GPIO_MODE_INPUT);
    //adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_2_5);
    //adc1_config_width(ADC_WIDTH_BIT_12);

    i2c_oled_init();

    I2CChannel1.init(I2C_NUM_1, GPIO_NUM_33, GPIO_NUM_32);
    I2CChannel1.openAsMaster(100000);

    BMP280.init(I2CChannel1);
    SMP3011.init(I2CChannel1);

    lvgl_port_lock(0);

    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);

    // Definição do BMP280 no display
    lv_obj_t *labelBMP280Press = lv_label_create(scr);
    static lv_style_t estilo_fonte;
    lv_style_init(&estilo_fonte);

    lv_label_set_long_mode(labelBMP280Press, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelBMP280Press, " ");
    lv_obj_set_width(labelBMP280Press, 128);
    lv_obj_align(labelBMP280Press, LV_ALIGN_TOP_MID, 0, 0);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelBMP280Press, &estilo_fonte, 0);

     // Definição temperatura do BMP280 no display
    lv_obj_t *labelBMP280Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelBMP280Temp, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelBMP280Temp, " ");
    lv_obj_set_width(labelBMP280Temp, 128);
    lv_obj_align(labelBMP280Temp, LV_ALIGN_TOP_MID, 0, 16);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelBMP280Temp, &estilo_fonte, 0);

     // Definição do SMP3011 no display
    lv_obj_t *labelSMP3011Press = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Press, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelSMP3011Press, " ");
    lv_obj_set_width(labelSMP3011Press, 128);
    lv_obj_align(labelSMP3011Press, LV_ALIGN_TOP_MID, 0, 32);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelSMP3011Press, &estilo_fonte, 0);

     // Definição temperatura do SMP3011 no display
    lv_obj_t *labelSMP3011Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Temp, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(labelSMP3011Temp, " ");
    lv_obj_set_width(labelSMP3011Temp, 128);
    lv_obj_align(labelSMP3011Temp, LV_ALIGN_TOP_MID, 0, 48);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelSMP3011Temp, &estilo_fonte, 0);

    lvgl_port_unlock();

    while (true)
    {
       
        //adc_value = adc1_get_raw(ADC1_CHANNEL_1);
        //vbat = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_1), &adc1_chars);
        BMP280.poll();
        SMP3011.poll();

        press_bmp280 = (BMP280.getPressure()) * PSI;
        press_smp3011 = (SMP3011.getPressure()) * PSI;
        //printf("\nVoltagem da bateria: %f\n", vbat);

        printf("\rPressao e temperatura: %6.0fpsi %6.2fºC Pressao SMP: %6.0fpsi %6.2fC",
               press_bmp280, BMP280.getTemperature(),
               press_smp3011, SMP3011.getTemperature());

        lvgl_port_lock(0);
        // Para inserção de caractéres especiais, utilize "\hex\" 
        lv_label_set_text_fmt(labelBMP280Press, "%6.2fpsi", press_bmp280);
        lv_label_set_text_fmt(labelBMP280Temp, "%6.2f\xb0\ C", BMP280.getTemperature());
        lv_label_set_text_fmt(labelSMP3011Press, "%6.2fpsi", press_smp3011);
        lv_label_set_text_fmt(labelSMP3011Temp, "%6.2fC", SMP3011.getTemperature());
        lvgl_port_unlock();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
