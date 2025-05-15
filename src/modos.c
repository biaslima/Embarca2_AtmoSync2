//Bibliotecas de apoio
#include "lib/buzzer/buzzer.h"
#include "lib/matriz/matriz_led.h"
#include "lib/display/ssd1306.h"
#include "include/modos.h"
#include "hardware/adc.h"
#include "setup.h"
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>   

// Definição da variável global declarada como extern em modos.h
ssd1306_t ssd;
ModoSistema modo_atual = MODO_CONFORTO;

//Executa os loops do modos
void executar_modulo_modos() {
    if (modo_atual == MODO_SEGURANCA && detect_loud_noise()) {
        if (!alarme_ativo) {
            printf("⚠️ Alarme ativado por ruído!\n");
            tocar_alarme();
            atualiza_display();  // Atualiza o display quando o alarme dispara
            atualiza_matriz_leds();  // Atualiza a matriz LED quando o alarme dispara
        }
    }

    alarme_loop();     // Se alarme ativo, toca
    animacao_festa_loop(); // Animação do modo festa
    musica_festa_loop();   // Música do modo festa
    
    sleep_ms(100);
}

//Define o modo atual no sistema
void set_modo(ModoSistema novo_modo) { 
    modo_atual = novo_modo;

    switch (modo_atual) {
        case MODO_CONFORTO:
            printf("Modo atual: Conforto\n");
            break;
        case MODO_FESTA:
            printf("Modo atual: Festa\n");
            break;
        case MODO_SEGURANCA:
            printf("Modo atual: Segurança\n");
            break;
        case MODO_SONO:
            printf("Modo atual: Sono\n");
            break;
    }
    atualiza_display();
    atualiza_matriz_leds();
    atualiza_buzzer();
    atualiza_rgb_led();
}

// ===========Funções de periféricos=================
//Atualiza o buzzer de acordo com o modo
void atualiza_buzzer() {
    switch (modo_atual) {
        case MODO_CONFORTO:
        case MODO_SEGURANCA:
            if (alarme_ativo) {
                tocar_alarme();
            } else {
                buzzer_desliga(BUZZER_PIN);
            }
            break;
        case MODO_SONO:
            buzzer_desliga(BUZZER_PIN);
            break;
        case MODO_FESTA:
            tocar_frequencia(440, 150);
            break;
    }
}

//Atualiza o display de acordo com o modo
void atualiza_display() {
    ssd1306_fill(&ssd, false);

    if (modo_atual == MODO_SEGURANCA && alarme_ativo) {
        ssd1306_draw_string(&ssd, "!!! INTRUSO !!!", 10, 20);
        ssd1306_send_data(&ssd);
        return;
    }

    switch (modo_atual) {
        case MODO_CONFORTO:
            ssd1306_draw_string(&ssd, "Modo: Conforto", 5, 2);
            ssd1306_draw_string(&ssd, "Status", 5, 2);

            break;
        case MODO_FESTA:
            ssd1306_draw_string(&ssd, "Modo: Festa", 5, 2);
            break;
        case MODO_SEGURANCA:
            ssd1306_draw_string(&ssd, "Modo: Seguranca", 5, 2);
            break;
        case MODO_SONO:
            ssd1306_draw_string(&ssd, "Modo: Sono", 5, 2);
            break;
    }
    ssd1306_send_data(&ssd);
}

//Atualizar matriz de LED de acordo com o modo
void atualiza_matriz_leds() {
    switch (modo_atual) {
        case MODO_CONFORTO:
            exibir_padrao(0);
            break;
        case MODO_FESTA:
            exibir_padrao(1); 
            break;
        case MODO_SEGURANCA:
            if (alarme_ativo) {
                piscar_matriz_intruso(); 
            } else {
            clear_matrix(pio0, 0);
            update_leds(pio0, 0);
        }
        break;
        case MODO_SONO:
            clear_matrix(pio0, 0);
            update_leds(pio0, 0);
            break;
    }
}

//Atualizar LED RGB de acordo com modo
void atualiza_rgb_led() {
    switch (modo_atual) {
        case MODO_CONFORTO:
            gpio_put(LED_GREEN_PIN, true);
            gpio_put(LED_RED_PIN, false);
            gpio_put(LED_BLUE_PIN, false);
            break;
        case MODO_FESTA:
            gpio_put(LED_GREEN_PIN, false);
            gpio_put(LED_RED_PIN, true);
            gpio_put(LED_BLUE_PIN, true);
            break;
        case MODO_SEGURANCA:
            gpio_put(LED_GREEN_PIN, false);
            gpio_put(LED_RED_PIN, true);
            gpio_put(LED_BLUE_PIN, false);
            break;
        case MODO_SONO:
            gpio_put(LED_GREEN_PIN, false);
            gpio_put(LED_RED_PIN, false);
            gpio_put(LED_BLUE_PIN, true);
            break;
    }
}

bool detect_loud_noise(void) {
    uint32_t sum = 0;
    uint16_t max_value = 0;
    uint16_t sample;

    adc_select_input(2);  // Canal onde o microfone está

    for (int i = 0; i < MIC_SAMPLES; i++) {
        sample = adc_read();
        if (sample > max_value) max_value = sample;
        sum += sample;
        sleep_us(100);
    }

    printf("Mic: Max %d | Avg %d\n", max_value, sum / MIC_SAMPLES);
    return (max_value > MIC_THRESHOLD);
}
