#include "pico-sin-audio.h"
 
// Audio PIN is to match some of the design guide shields. 
//#define AUDIO_PIN 26  // you can change this to whatever you like

/* 
 * This include brings in static arrays which contain audio samples. 
 * if you want to know how to make these please see the python code
 * for converting audio samples into static arrays. 
 */
//#include "../sample.h"
int pwm_wav_position = 0;
int pwm_counter = 0;
int pwm_counter_max = 25;  // 1.85kHz
int pwm_amplitude = 120;
int pwm_value = 128 - pwm_amplitude;
int pwm_rnd_bit = 1;
int pwm_audio_pin = 26;
bool pwm_audio_on = false;

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */

void pwm_sin_set_pin(int pin) {
 
  pwm_audio_pin = pin;
 
}

void pwm_interrupt_handler3() {
  pwm_clear_irq(pwm_gpio_to_slice_num(pwm_audio_pin)); 
  if (pwm_audio_on) {
    pwm_counter++;   
    if (pwm_counter > pwm_counter_max) {
      pwm_counter -= pwm_counter_max;
    }

    pwm_value = int (128 + pwm_amplitude * sin(pwm_counter * 6.28 / (float)(pwm_counter_max)));
 //   Serial.println(value);

    pwm_set_gpio_level(pwm_audio_pin, pwm_value);  
  }
}

void pwm_sin_start() {
    /* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
//    stdio_init_all();
//    set_sys_clock_khz(176000, true); 
//    set_sys_clock_khz(125000, true); 
    gpio_set_function(pwm_audio_pin, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(pwm_audio_pin);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler3); 
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 250); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(pwm_audio_pin, 0);
 
     pwm_audio_on = true;
}

void pwm_sin_stop() {
 
    pwm_audio_on = false;
 
}
