#include "lvgl.h"
#include "cBMP280.h"

cBMP280 BMP280;

void getTemperature(lv_disp_t *disp)
{
    BMP280.init(I2CChannel1);
    
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

}
