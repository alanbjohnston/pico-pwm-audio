#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
 
// Audio PIN is to match some of the design guide shields. 
#define AUDIO_PIN 26  // you can change this to whatever you like

/* 
 * This include brings in static arrays which contain audio samples. 
 * if you want to know how to make these please see the python code
 * for converting audio samples into static arrays. 
 */
//#include "sample.h"
int wav_position = 0;
int counter = 0;
int counter_max = 100;
int amplitude = 100;
int value = 128 - amplitude;
int rnd_bit = 1;

bool polarity = 1;
pwm_config config;
int audio_pin_slice;

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */
/*
void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (WAV_DATA_LENGTH<<3) - 1) { 
        // set pwm level 
        // allow the pwm value to repeat for 8 cycles this is >>3 
        pwm_set_gpio_level(AUDIO_PIN, WAV_DATA[wav_position>>3]);  
        wav_position++;
    } else {
        // reset to start
        wav_position = 0;
    }
}


void pwm_interrupt_handler2() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN)); 
    counter++;   
 //   if (wav_position < (WAV_DATA_LENGTH<<3) - 1) { 
        // set pwm level 
        // allow the pwm value to repeat for 8 cycles this is >>3 


        if (counter > 450) {
          counter -= 450;
          if (random(0,2) == 1)
            rnd_bit *= (-1.0);
        
          if ((value == (128 - amplitude)) && (rnd_bit == 1)) {
            value = 128 + amplitude;
            Serial.println("High");
          }
          else {
            value = 128 - amplitude; 
            Serial.println("Low");
          }
        }  
        pwm_set_gpio_level(AUDIO_PIN, value);  
        wav_position++;
//    } else {
        // reset to start
//        wav_position = 0;
//    }
}
*/

void pwm_interrupt_handler4() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN)); 
    counter++; 
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 pinMode(17, OUTPUT);
  digitalWrite(17, LOW);  // start transmitting

  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);

  pinMode(28, INPUT);
  pinMode(27, INPUT);
 
  
//  Serial1.begin(9600);
  delay(500);
//  Serial1.println("AT+DMOSETGROUP=0,434.9000,434.9000,1,2,1,1\r");
//  Serial1.println("AT+DMOSETGROUP=0,434.9000,434.9000,0,2,0,1\r");
  delay(500);
  
    set_sys_clock_khz(125000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler4); 
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    config = pwm_get_default_config();
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
    pwm_config_set_clkdiv(&config, 250.0); //8.0f); 
    pwm_config_set_wrap(&config, 250); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 128);
  
}

void loop() {
    /* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
//    stdio_init_all();
   
  /*
   pwm_config_set_output_polarity  ( pwm_config *  c, bool   a, bool   b ) 
   */
  pwm_config_set_output_polarity  ( &config, polarity, polarity ); 
  pwm_init(audio_pin_slice, &config, true); 
  pwm_set_gpio_level(AUDIO_PIN, 128); 
  polarity = not(polarity); 
//  Serial.println(polarity);
  delay(5);

  
//    while(1) {
//        __wfi(); // Wait for Interrupt
//    }
}
