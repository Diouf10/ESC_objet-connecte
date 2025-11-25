#ifndef BUZZER_H
#define BUZZER_H

#include "driver/gpio.h"

void buzzer_init(gpio_num_t pin);
void buzzer_beep_short(void);
void buzzer_beep_long(void);
void buzzer_play_victory(void);

#endif
