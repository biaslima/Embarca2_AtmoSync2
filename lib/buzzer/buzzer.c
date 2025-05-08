#include "buzzer.h"
#include "hardware/pwm.h"
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>   


static int buzzer_pin;
static uint slice_num;
static uint channel;

void buzzer_init(int pin) {
    buzzer_pin = pin;

    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    channel = pwm_gpio_to_channel(buzzer_pin);
    
    pwm_set_clkdiv(slice_num, 125.0);  
    pwm_set_wrap(slice_num, 1000);     
    pwm_set_chan_level(slice_num, channel, 500); 
    
    printf("Buzzer inicializado no pino %d (PWM slice %d, canal %d)\n", 
           pin, slice_num, channel);
}

// Liga o buzzer com um tom padrão
void buzzer_liga(int pin) {
    // O ciclo de clock do RP2040 é de 125MHz por padrão
    uint32_t clock = 125000000;
    
    // Frequência desejada (por exemplo, 440Hz para Lá)
    uint32_t freq = 440;
    
    // Cálculo dos valores para o PWM
    uint32_t wrap = clock / freq / 2;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, wrap / 2);
}

void buzzer_desliga(int pin){
    pwm_set_enabled(slice_num, false);
    gpio_put(pin, 0);
}

void tocar_frequencia(int frequencia, int duracao_ms) {
    // Fórmula: wrap = 1_000_000 / frequência
    uint32_t wrap = 1000000 / frequencia;
    
    printf("Tocando frequência %d Hz (wrap=%d)\n", frequencia, wrap);
    
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, channel, wrap / 2);
    pwm_set_enabled(slice_num, true);
    
    sleep_ms(duracao_ms);
    
    pwm_set_enabled(slice_num, false);
}
