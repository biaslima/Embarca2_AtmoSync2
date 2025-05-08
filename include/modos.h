#ifndef MODOS_H
#define MODOS_H

#include "ssd1306.h"

typedef enum {
    MODO_NORMAL,
    MODO_FESTA,
    MODO_SEGURANCA,
    MODO_SONO
} ModoSistema;

extern ModoSistema modo_atual;

void atualiza_display();
void atualiza_buzzer();
void atualiza_matriz_leds();
void atualiza_rgb_led(); // você ainda vai implementar

#endif
