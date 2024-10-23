#include <cstdio>

#include "i2c_oled.h"
#include "esp_lvgl_port.h"

#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "cbspI2C.h"
#include "cBMP280.h"
#include "cSMP3011.h"

#define KPA_PSI 0.14503
#define PSI_BAR 0.06894
#define PSI_ATM 0.06804

cbspI2C I2CChannel1;
cBMP280 BMP280;
cSMP3011 SMP3011;

// esp_adc_cal_characteristics_t adc1_chars;

// Recebe o tempo atual
int time_offset = esp_timer_get_time() * 1000;
int last_time = 0;


// Inicializa variáveis dos sensores de pressão

float pressure_atm = 0, pressure_smp3011 = 0, pressure_smp3011_bar = 0, avg_pressure = 0;

//extern void temp(lv_disp_t *disp);

/*extern "C" void getTemp()
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    float temp = BMP280.getTemperature();

    lv_style_init(&estilo_fonte);

    // Definição temperatura do BMP280 no display
    static lv_style_t estilo_fonte;
    lv_obj_t *labelBMP280Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelBMP280Temp, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelBMP280Temp, " ");
    lv_obj_set_width(labelBMP280Temp, 128);
    lv_obj_align(labelBMP280Temp, LV_ALIGN_TOP_MID, 0, 16);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_18);
    lv_obj_add_style(labelBMP280Temp, &estilo_fonte, 0);
    lv_label_set_text(labelBMP280Temp, "Temperatura: %6.2f\xb0\ C", temp);

}*/

extern "C" void getPressure()
{
    int i = 0;

    pressure_smp3011 = (SMP3011.getPressure()) * KPA_PSI;
    pressure_atm = pressure_smp3011 * PSI_ATM;

    // Verifica se houve alguma alteração na pressão atmosférica através do bmp280
    // Caso haja, calcula a pressão interna do pneu
    //  Caso contrário, o valor de pressão padrão será zero
    //if (pressure_atm > 1)
    //{
        // Realiza 5 leituras por segundo e some ao valor anterior
        while (i <= 5) 
        {
            if ((time_offset - last_time) >= 200)
            {
                last_time = time_offset;
                pressure_smp3011 += pressure_smp3011;
            }
            i += 1;
        }
    //}
    /*else
    {
        avg_pressure = 0;
    }*/

    avg_pressure = pressure_smp3011 / 5;
    pressure_smp3011_bar = avg_pressure * PSI_BAR;
}

extern "C" void app_main()
{
    esp_timer_early_init();

    i2c_oled_init();

    I2CChannel1.init(I2C_NUM_1, GPIO_NUM_33, GPIO_NUM_32);
    I2CChannel1.openAsMaster(100000);

    BMP280.init(I2CChannel1);
    SMP3011.init(I2CChannel1);

    float temp = BMP280.getTemperature();

    lvgl_port_lock(0);

    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);

    static lv_style_t estilo_fonte;
    lv_style_init(&estilo_fonte);

    // Definição temperatura do BMP280 no display
    lv_obj_t *labelBMP280Temp = lv_label_create(scr);
    lv_label_set_long_mode(labelBMP280Temp, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text_fmt(labelBMP280Temp, "%6.0f", temp);
    lv_obj_set_width(labelBMP280Temp, 128);
    lv_obj_align(labelBMP280Temp, LV_ALIGN_TOP_MID, 0, 16);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelBMP280Temp, &estilo_fonte, 0);

    // Definição do SMP3011 no display
    lv_obj_t *labelSMP3011Press = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Press, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelSMP3011Press, " ");
    lv_obj_set_width(labelSMP3011Press, 128);
    lv_obj_align(labelSMP3011Press, LV_ALIGN_TOP_MID, 0, 32);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelSMP3011Press, &estilo_fonte, 0);

    lvgl_port_unlock();

    while(true)
    {
        BMP280.poll();
        SMP3011.poll();

        getPressure();


        printf("\nPressão: %6.2fpsi --- %6.2fbar", avg_pressure, pressure_smp3011_bar);

        lvgl_port_lock(0);
        // Para inserção de caractéres especiais, utilize "\hex\"
        lv_label_set_text_fmt(labelSMP3011Press, "%6.0f psi", avg_pressure);
        //lv_label_set_text_fmt(labelSMP3011Press, "%6.2fbar", pressure_smp3011_bar);
        
        lvgl_port_unlock();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
