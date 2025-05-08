#include "modos.h"
#include "buzzer.h"
#include "ssd1306.h"
#include "matriz_led.h"

// Variável de display global (você pode mover isso para main se preferir)
extern ssd1306_t oled_display;

void atualiza_display() {
    ssd1306_fill(&oled_display, false);  // limpa tela
    switch (modo_atual) {
        case MODO_NORMAL:
            ssd1306_draw_string(&oled_display, "Modo: Normal", 0, 0);
            break;
        case MODO_FESTA:
            ssd1306_draw_string(&oled_display, "Modo: Festa!", 0, 0);
            break;
        case MODO_SEGURANCA:
            ssd1306_draw_string(&oled_display, "Modo: Seguranca", 0, 0);
            break;
        case MODO_SONO:
            ssd1306_draw_string(&oled_display, "Modo: Sono", 0, 0);
            break;
    }
    ssd1306_send_data(&oled_display);
}

void atualiza_buzzer() {
    switch (modo_atual) {
        case MODO_NORMAL:
        case MODO_SEGURANCA:
        case MODO_SONO:
            buzzer_desliga(BUZZER_PIN);
            break;
        case MODO_FESTA:
            tocar_frequencia(440, 200);
            break;
    }
}

void atualiza_matriz_leds() {
    switch (modo_atual) {
        case MODO_NORMAL:
            // exemplo: exibir um padrão estável
            exibir_padrao(0); // ajustar na sua função
            break;
        case MODO_FESTA:
            // usar padrão animado, se tiver
            exibir_padrao(1);
            break;
        case MODO_SEGURANCA:
            // padrão de presença
            exibir_padrao(2);
            break;
        case MODO_SONO:
            clear_matrix(pio0, 0); // ou algo em azul
            update_leds(pio0, 0);
            break;
    }
}
