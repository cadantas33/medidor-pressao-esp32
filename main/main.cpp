#include <cstdio>

#include "i2c_oled.h"
#include "esp_lvgl_port.h"

#include "lvgl.h"
// #include "lv_theme_simple.h"
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
#define PA_PSI 0.001450

#define BASE_ATM 1.0
// #define LV_FONT_CUSTOM_DECLARE CONFIG_LV_FONT_DECLARE(cousine_18)
// LV_FONT_DECLARE(cousine_18)

cbspI2C I2CChannel1;
// cBMP280 BMP280;
cSMP3011 SMP3011;

// esp_adc_cal_characteristics_t adc1_chars;

// Recebe o tempo atual
int time_offset = esp_timer_get_time() * 1000;
int last_time = 0;
int last_time2 = 0;

uint16_t press_tskPRIORITY = 21;
uint16_t press_tskSIZE = 2048;

uint16_t display_tskPRIORITY = 20;
uint16_t display_tskSIZE = 4096;

// Inicializa variáveis dos sensores de pressão

float raw_value = 0, atm_pressure = 0, pressure_psi = 0, avg_pressure = 0, avg_pressure_bar = 0;

extern "C" void readPressure(void *params)
{
    configASSERT(((uint32_t)params) == 1);

    int i = 0;

    for (;;)
    {
        SMP3011.poll();

        raw_value = SMP3011.getPressure();
        /*voltage = raw_value * (3.3 / 4095);
        pressure_psi = (voltage - 0.45) * (100.0 / 3.3 - 0.45);
        //pressure_psi = ((raw_value - 1843.2) * 20000) / (13516.8 - 1843.2);
        //pressure_psi = ((raw_value - 7372.8) * 20000) / (13516.8 - 7372.8);
        //pressure_psi = raw_press * 0.0145;*/
        pressure_psi = raw_value * PA_PSI;
        atm_pressure = pressure_psi * PSI_ATM;

        // Caso haja pressão atmosférica maior, calcula a pressão interna do pneu
        //  Caso contrário, o valor de pressão padrão será zero
        if (atm_pressure > BASE_ATM)
        {
            // Realiza 5 leituras por segundo e some ao valor anterior
            while (i <= 5)
            {
                if ((time_offset - last_time) >= 200)
                {
                    last_time = time_offset;
                    pressure_psi += pressure_psi;
                }
                i += 1;
            }
        }
        /*else if (atm_pressure <= BASE_ATM)         // Este bloco condicional foi jogado para o loop infinito
        {                                          // do 'while(true), já que não funcionava como esperado
            avg_pressure = 0;
        }*/

        avg_pressure = pressure_psi / 5;
        avg_pressure_bar = avg_pressure * PSI_BAR;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void displayPressure(void *params)
{
    configASSERT(((uint32_t)params) == 2);

    lvgl_port_lock(0);

    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);

    // Inicializa fonte a ser utilizada
    // (Neste caso, apenas para ajustar o tamanho do texto)
    static lv_style_t press_style;
    static lv_style_t text_style;
    lv_style_init(&press_style);
    lv_style_init(&text_style);

    // Definição do SMP3011 no display
    lv_obj_t *labelSMP3011Press = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Press, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelSMP3011Press, " ");
    lv_obj_set_width(labelSMP3011Press, 128);
    lv_obj_align(labelSMP3011Press, LV_ALIGN_TOP_LEFT, 4, 0);
    lv_style_set_text_font(&press_style, &lv_font_montserrat_18);
    lv_obj_add_style(labelSMP3011Press, &press_style, 0);

    // Definição dos avisos quanto à pressão
    lv_obj_t *labelPressWarn = lv_label_create(scr);
    lv_label_set_long_mode(labelPressWarn, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelPressWarn, " ");
    lv_obj_set_width(labelPressWarn, 128);
    lv_obj_align(labelPressWarn, LV_ALIGN_BOTTOM_LEFT, 2, 2);
    lv_style_set_text_font(&text_style, &lv_font_montserrat_20);
    lv_obj_add_style(labelPressWarn, &text_style, 0);

    // Definição dos ícones de avisos
    lv_obj_t *labelPressIcon = lv_label_create(scr);
    lv_label_set_long_mode(labelPressIcon, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelPressIcon, " ");
    lv_obj_set_width(labelPressIcon, 128);
    lv_obj_align(labelPressIcon, LV_ALIGN_BOTTOM_LEFT, 102, 2);

    lvgl_port_unlock();

    for (;;)
    {
        // BMP280.poll();

        // Zera os valores ao ligar, calculando a partir da pressão do ambiente
        // Deve haver alguma maneira mais enxuta de fazer isso, mas o que foi tentado, não funcionou
        if (atm_pressure <= BASE_ATM)
        {
            avg_pressure = 0;
            avg_pressure_bar = 0;
        }

        printf("\nPressão: %6.2fpsi -- %6.2fbar\nATM: %6.2f\nRaw Value: %6.2f\n", avg_pressure,
               avg_pressure_bar, atm_pressure, raw_value);

        lvgl_port_lock(0);
        // Para inserção de caractéres especiais, utilize "\hex\"
        lv_label_set_text_fmt(labelSMP3011Press, "%6.2f psi\n %6.2f bar", avg_pressure, avg_pressure_bar);
        /*if (avg_pressure > 0)
        {
            lv_label_set_text_fmt(labelSMP3011Press, "%6.2f psi\n %6.2f bar", avg_pressure, avg_pressure_bar);
            lv_timer_handler_run_in_period(5000);
        }
        else
        {
            lv_label_set_text_fmt(labelSMP3011Press, "0 psi\n 0 bar");
        }*/
        // lv_label_set_text_fmt(labelBMP280Temp, "%6.2f\xb0\ C", temp); // exibição da temperatura

        // Avisa se a pressão dos pneus é ou não adequada
        if (avg_pressure > 32 && avg_pressure <= 40)
        {
            lv_label_set_text(labelPressWarn, "Adequado!");
            lv_label_set_text(labelPressIcon, LV_SYMBOL_OK);
        }
        else if (avg_pressure <= 32 && avg_pressure > 27)
        {
            lv_label_set_text(labelPressWarn, "Calibragem recomendada!");
            lv_label_set_text(labelPressIcon, LV_SYMBOL_WARNING);
        }
        else if (avg_pressure <= 27)
        {
            lv_label_set_text(labelPressWarn, "Calibrar!");
            lv_label_set_text(labelPressIcon, LV_SYMBOL_CLOSE);
        }
        /*switch (avg_pressure)
        {
        case 32 ... 38:
            lv_label_set_text_fmt(labelPressWarn, "Pressao adequada para o veiculo");
            break;
        case 27 ... 31:
            lv_label_set_text_fmt(labelPressWarn, "Pressao intermediaria, calibragem recomendada");
            break;
        case 0 ... 26:
            lv_label_set_text_fmt(labelPressWarn, "Pressao baixa, calibragem necessaria");
            break;
        default:
            lv_label_set_text_fmt(labelPressWarn, "Pressao acima do ideal, calibragem necessaria");
            break;
        }*/

        lvgl_port_unlock();

        // lv_timer_handler_run_in_period(5);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main()
{
    esp_timer_early_init();

    i2c_oled_init();

    I2CChannel1.init(I2C_NUM_1, GPIO_NUM_33, GPIO_NUM_32);
    I2CChannel1.openAsMaster(100000);

    // BMP280.init(I2CChannel1);

    // Inicializa o smp no canal i2c
    SMP3011.init(I2CChannel1);

    // Inicializa task da função de leitura da pressão
    xTaskCreate(readPressure, "read_pressure", configMINIMAL_STACK_SIZE, (void *)1, press_tskPRIORITY, NULL);

    // Inicializa task das exibições no display
    xTaskCreate(displayPressure, "display", display_tskSIZE, (void *)2, display_tskPRIORITY, NULL);

    // float temp = BMP280.getTemperature();
    /*lvgl_port_lock(0);

    lv_obj_t *scr = lv_disp_get_scr_act(nullptr);
    static lv_style_t estilo_fonte;
    lv_style_init(&estilo_fonte);

    // Definição do SMP3011 no display
    lv_obj_t *labelSMP3011Press = lv_label_create(scr);
    lv_label_set_long_mode(labelSMP3011Press, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelSMP3011Press, " ");
    lv_obj_set_width(labelSMP3011Press, 128);
    lv_obj_align(labelSMP3011Press, LV_ALIGN_TOP_LEFT, 2, 0);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_18);
    lv_obj_add_style(labelSMP3011Press, &estilo_fonte, 0);

    // Definição dos avisos quanto à pressão
    lv_obj_t *labelPressWarn = lv_label_create(scr);
    lv_label_set_long_mode(labelPressWarn, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(labelPressWarn, " ");
    lv_obj_set_width(labelPressWarn, 128);
    lv_obj_align(labelPressWarn, LV_ALIGN_BOTTOM_LEFT, 2, 2);
    lv_style_set_text_font(&estilo_fonte, &lv_font_montserrat_12);
    lv_obj_add_style(labelPressWarn, &estilo_fonte, 0);*/

    // lvgl_port_unlock();

    // for (;;)
    //{
    //  BMP280.poll();

    // Zera os valores ao ligar, calculando a partir da pressão do ambiente
    // Deve haver alguma maneira mais enxuta de fazer isso, mas o que foi tentado, não funcionou
    /*if (atm_pressure <= BASE_ATM)
    {
        avg_pressure = 0;
    }*/

    /*printf("\nPressão: %6.2fpsi --- %6.2fbar -- \nATM: %6.2f", avg_pressure,
           avg_pressure_bar, atm_pressure);

    lvgl_port_lock(0);
    // Para inserção de caractéres especiais, utilize "\hex\"

    if (avg_pressure > 0)
    {
        if ((time_offset - last_time2) >= 30000)
        {
            last_time2 = time_offset;
            lv_label_set_text_fmt(labelSMP3011Press, "%6.0f psi\n %6.0f bar", avg_pressure, avg_pressure_bar);
        }
    }
    else
    {
        lv_label_set_text_fmt(labelSMP3011Press, "0 psi\n 0 bar");
    }
    // lv_label_set_text_fmt(labelBMP280Temp, "%6.2f\xb0\ C", temp); // exibição da temperatura

    // Avisa se a pressão dos pneus é ou não adequada
    if (avg_pressure > 32 && avg_pressure <= 40)
    {
        lv_label_set_text_fmt(labelPressWarn, "Pressao adequada para o veiculo");
    }
    else if (avg_pressure <= 32 && avg_pressure > 27)
    {
        lv_label_set_text_fmt(labelPressWarn, "Pressao intermediaria, calibragem recomendada");
    }
    else if (avg_pressure <= 27)
    {
        lv_label_set_text_fmt(labelPressWarn, "Pressao baixa, calibragem necessaria");
    }
    switch (avg_pressure)
    {
    case 32 ... 38:
        lv_label_set_text_fmt(labelPressWarn, "Pressao adequada para o veiculo");
        break;
    case 27 ... 31:
        lv_label_set_text_fmt(labelPressWarn, "Pressao intermediaria, calibragem recomendada");
        break;
    case 0 ... 26:
        lv_label_set_text_fmt(labelPressWarn, "Pressao baixa, calibragem necessaria");
        break;
    default:
        lv_label_set_text_fmt(labelPressWarn, "Pressao acima do ideal, calibragem necessaria");
        break;
    }*/

    // lvgl_port_unlock();

    // vTaskDelay(pdMS_TO_TICKS(1000));
    //}
}
