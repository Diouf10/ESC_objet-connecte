#include "pir_sensor.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "PIR";

static gpio_num_t s_pir_gpio = -1;

void pir_init(gpio_num_t pin) {
    s_pir_gpio = pin;
    gpio_reset_pin(s_pir_gpio);
    gpio_set_direction(s_pir_gpio, GPIO_MODE_INPUT);

    ESP_LOGI(TAG, "Capteur PIR initialisé sur la broche GPIO %d", s_pir_gpio);
}

int pir_detected(void) {
    int state = gpio_get_level(s_pir_gpio);

    if (state) {
        ESP_LOGD(TAG, "Présence détectée");
    } else {
        ESP_LOGV(TAG, "Aucune présence");
    }

    return state;
}
