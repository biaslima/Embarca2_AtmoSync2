// Bibliotecas padrão
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>           
#include <time.h>

// Bibliotecas da Pico e do driver Wi-Fi CYW43
#include "pico/stdlib.h"         
#include "hardware/adc.h"        
#include "pico/cyw43_arch.h"     

// Pilha TCP/IP leve usada para comunicação via rede
#include "lwip/pbuf.h"           //  manipulação de buffers de pacotes de rede
#include "lwip/tcp.h"            // funções e estruturas para protocolo TCP
#include "lwip/netif.h"          // funções e estruturas para interfaces de rede (netif)

#include "setup.h"
#include "include/modos.h"
#include "include/webserver.h"
#include "lib/display/ssd1306.h"
#include "lib/matriz/matriz_led.h"
#include "lib/buzzer/buzzer.h"

#define WIFI_SSID "Sua rede"
#define WIFI_PASSWORD "Sua senha"

// Inicializa os GPIOs conectados aos LEDs
void gpio_led_bitdog(void) {
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, false);

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, false);

    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, false);
}



void iniciar_display(){
    //Inicializar display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    
    gpio_pull_up(I2C_SDA);                                        
    gpio_pull_up(I2C_SCL);                                        
                                          
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd);                                         
    ssd1306_send_data(&ssd);                                      
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

}

// Função principal
int main(){
    stdio_init_all();
    gpio_led_bitdog();
    buzzer_init(BUZZER_PIN);

    //Inicia mariz
    matrix_init(pio0, 0, LED_MATRIX_PIN);
    clear_matrix(pio0, 0);
    update_leds(pio, sm);

    iniciar_display();


    //Inicializa o Wi-Fi
    while (cyw43_arch_init()){
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
    cyw43_arch_gpio_put(LED_PIN, 0);
    cyw43_arch_enable_sta_mode();

    // Conectar à rede
    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)){
        printf("Falha ao conectar\n");
        sleep_ms(100);
        return -1;
    }
    printf("Conectado!\n");

    if (netif_default){
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }
    webserver_init();

    // Inicializa o conversor ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);

    set_modo(MODO_CONFORTO);

    while (true){
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        executar_modulo_modos();
        sleep_ms(100);
    }

    //Desligar a arquitetura CYW43.
    cyw43_arch_deinit();
    return 0;
}

















