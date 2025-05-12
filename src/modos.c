//Bibliotecas de apoio
#include "lib/buzzer/buzzer.h"
#include "lib/matriz/matriz_led.h"
#include "lib/display/ssd1306.h"
#include "include/modos.h"
#include "setup.h"
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>   

// Definição da variável global declarada como extern em modos.h
ModoSistema modo_atual = MODO_CONFORTO;

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
            alarme_ativo = true;
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
    switch (modo_atual) {
        case MODO_CONFORTO:
            ssd1306_draw_string(&ssd, "Modo: Conforto", 0, 0);
            break;
        case MODO_FESTA:
            ssd1306_draw_string(&ssd, "Modo: Festa", 0, 0);
            break;
        case MODO_SEGURANCA:
            ssd1306_draw_string(&ssd, "Modo: Seguranca", 0, 0);
            break;
        case MODO_SONO:
            ssd1306_draw_string(&ssd, "Modo: Sono", 0, 0);
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
            exibir_padrao(2);
            break;
        case MODO_SONO:
            clear_matrix(pio0, 0);
            update_leds(pio0, 0);
            break;
    }
}

//Atualizar LED RGB de acordo com modo
void atualiza_rgb_led() {
    // Implementação da função de LED RGB
    switch (modo_atual) {
        case MODO_CONFORTO:
            gpio_put(LED_GREEN_PIN, true);
            gpio_put(LED_RED_PIN, false);
            gpio_put(LED_BLUE_PIN, false);
            break;
        case MODO_FESTA:
            gpio_put(LED_GREEN_PIN, false);
            gpio_put(LED_RED_PIN, false);
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
            gpio_put(LED_BLUE_PIN, false);
            break;
    }
}