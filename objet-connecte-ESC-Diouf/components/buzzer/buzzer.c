#include "buzzer.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "buzzer";
static gpio_num_t s_buzzer_gpio = -1;

////////////////////// Générer avec Chat GPT

typedef struct {
    int freq;
    int duration;   
} note_t;

static note_t melody[] = {
    {392, 200}, // G
    {523, 200}, // C
    {659, 200}, // E
    {784, 300}, // G
    {659, 200}, // E
    {523, 400}, // C
    {0, 300},   // silence
    {784, 300}, // G
    {880, 500}  // A
};

//////////////////////

void buzzer_init(gpio_num_t pin) {
    s_buzzer_gpio = pin;

    ledc_timer_config_t timer_conf = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .freq_hz          = 1000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num   = s_buzzer_gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&channel_conf);
}

void buzzer_beep_short(void) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 1000);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(200));
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
}

void buzzer_beep_long(void) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 1000);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
}

void buzzer_play_victory(void) {
    ESP_LOGI(TAG, "Lecture de la mélodie de victoire");
    int num_notes = sizeof(melody) / sizeof(melody[0]);

    for (int i = 0; i < num_notes; i++) {
        if (melody[i].freq > 0) {
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, melody[i].freq);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        } else {
            ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(melody[i].duration));
        ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
