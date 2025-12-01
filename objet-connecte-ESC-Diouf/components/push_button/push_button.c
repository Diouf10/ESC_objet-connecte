#include "push_button.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "BUTTONS";

static gpio_num_t s_button_pins[4];
static int s_button_count = 0;

// Initialise les boutons 
void buttons_init(gpio_num_t *pins, int count) {
    s_button_count = count;
    for (int i = 0; i < count; i++) {
        s_button_pins[i] = pins[i];
        gpio_reset_pin(s_button_pins[i]);
        gpio_set_direction(s_button_pins[i], GPIO_MODE_INPUT);
        gpio_pullup_en(s_button_pins[i]);
        ESP_LOGI(TAG, "Bouton %d initialisé sur GPIO %d", i + 1, s_button_pins[i]);
    }
}

int read_button(void) {
    for (int i = 0; i < s_button_count; i++) {
        if (gpio_get_level(s_button_pins[i]) == 0) {
            ESP_LOGD(TAG, "Bouton %d pressé (GPIO %d)", i + 1, s_button_pins[i]);
            return i + 1;
        }
    }
    return -1;
}
