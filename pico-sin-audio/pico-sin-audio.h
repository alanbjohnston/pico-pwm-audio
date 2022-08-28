#ifndef PICO_SIN_AUDIO_H
#define PICO_SIN_AUDIO_H

#include <Arduino.h>

#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 

void pwm_sin_stop();
void pwm_sin_start();
void pwm_sin_set_pin(); 
void pwm_set_freq(int freq);

#endif
