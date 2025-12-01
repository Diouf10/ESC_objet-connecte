#include "led.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "LED";

static gpio_num_t s_led_gpio = -1;

// Initialise une LED en mode sortie et l'éteint par défaut
void led_init(gpio_num_t pin) {
    s_led_gpio = pin;
    gpio_reset_pin(s_led_gpio);
    gpio_set_direction(s_led_gpio, GPIO_MODE_OUTPUT);
    led_off();

    ESP_LOGI(TAG, "LED initialisée sur la broche GPIO %d", s_led_gpio);
}

// Allume la LED
void led_on(void) {
    gpio_set_level(s_led_gpio, 1);
    ESP_LOGD(TAG, "LED ON");
}

// Éteint la LED
void led_off(void) {
    gpio_set_level(s_led_gpio, 0);
    ESP_LOGD(TAG, "LED OFF");
}
