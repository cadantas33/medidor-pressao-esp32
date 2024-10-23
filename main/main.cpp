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

#define KPA_PSI 0.14503
#define KPA_ATM 0.01
#define PSI_BAR 0.06894
#define PSI_ATM 0.068047
#define PA_PSI 0.0001450

cbspI2C I2CChannel1;
cBMP280 BMP280;
cSMP3011 SMP3011;

// esp_adc_cal_characteristics_t adc1_chars;

// Recebe o tempo atual
int time_offset = esp_timer_get_time() * 1000;
int last_time = 0;

// Inicializa variáveis para leitura da bateria
int adc_value;
float vbat;

// Inicializa variáveis dos sensores de pressão

float atm_pressure = 0, pressure_smp3011 = 0, pressure_smp3011_bar = 0, avg_pressure = 0;

extern void example_lvgl_demo_ui(lv_disp_t *disp);

extern "C" void updatePressure()
{
    int i = 0;

    pressure_smp3011 = (SMP3011.getPressure() * -1) * KPA_PSI;
    // atm_pressure = pressure_smp3011 * PSI_ATM;
    atm_pressure = (BMP280.getPressure() * PA_PSI) * PSI_ATM;

    // Verifica se houve alguma alteração na pressão do ambiente através do smp3011

    // Caso haja, calcula a pressão interna do pneu
    //  Caso contrário, o valor de pressão padrão será zero

    // Num ambiente aberto, a pressão do smp3011 equivale a 6atm, sendo que este será o ponto de
    // partida para as medições

    // Também pode ser feito com o bmp280, mas requer uma câmara acima do sensor, de forma que
    // receba ar constantemente para fazer as medições precisamente
    if (atm_pressure > 1)
    {
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
    }
    else
    {
        pressure_smp3011 = 0;
    }

    avg_pressure = pressure_smp3011 / 5;
    pressure_smp3011_bar = avg_pressure * PSI_BAR;
}

extern "C" void app_main()
{
    esp_timer_early_init();
    // gpio_set_direction(VBAT_PIN, GPIO_MODE_INPUT);
    // adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_2_5);
    // adc1_config_width(ADC_WIDTH_BIT_12);

    i2c_oled_init();

    I2CChannel1.init(I2C_NUM_1, GPIO_NUM_33, GPIO_NUM_32);
    I2CChannel1.openAsMaster(100000);

    BMP280.init(I2CChannel1);
    SMP3011.init(I2CChannel1);

    lvgl_port_lock(0);

    // float temp = BMP280.getTemperature();

    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);

    static lv_style_t estilo_fonte;
    lv_style_init(&estilo_fonte);

    // Definição temperatura do BMP280 no display
    // lv_obj_t *labelBMP280Temp = lv_label_create(scr);
    /*lv_label_set_long_mode(labelBMP280Temp, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text_fmt(labelBMP280Temp, "%6.0f", temp);
    lv_obj_set_width(labelBMP280Temp, 128);
    lv_obj_align(labelBMP280Temp, LV_ALIGN_TOP_MID, 0, 16);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelBMP280Temp, &estilo_fonte, 0);*/

    // Definição do SMP3011 no display
    lv_obj_t *labelSMP3011Press = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Press, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelSMP3011Press, " ");
    lv_obj_set_width(labelSMP3011Press, 128);
    lv_obj_align(labelSMP3011Press, LV_ALIGN_TOP_MID, 10, 12);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_16);
    lv_obj_add_style(labelSMP3011Press, &estilo_fonte, 0);

    lvgl_port_unlock();

    for (;;)
    {
        // adc_value = adc1_get_raw(ADC1_CHANNEL_1);
        // vbat = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_1), &adc1_chars);
        // printf("\nVoltagem da bateria: %f\n", vbat);
        // printf("\rPressao e temperatura: %6.0fpsi %6.2fºC Pressao SMP: %6.0fpsi %6.2fC");

        BMP280.poll();
        SMP3011.poll();

        updatePressure();

        printf("\nPressão: %6.2fpsi --- %6.2fbar -- \nATM: %6.2f", avg_pressure,
               pressure_smp3011_bar, atm_pressure);

        lvgl_port_lock(0);
        // Para inserção de caractéres especiais, utilize "\hex\"
        lv_label_set_text_fmt(labelSMP3011Press, "%6.2f psi\n %6.2f bar", avg_pressure, pressure_smp3011_bar);
        // lv_label_set_text_fmt(labelBMP280Temp, "%6.2f\xb0\ C", temp); // exibição da temperatura
        lvgl_port_unlock();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
