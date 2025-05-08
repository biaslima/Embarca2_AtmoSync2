// Bibliotecas padrão
#include <stdio.h>               
#include <string.h>              
#include <stdlib.h>              

// Bibliotecas da Pico e do driver Wi-Fi CYW43
#include "pico/stdlib.h"         
#include "hardware/adc.h"        
#include "pico/cyw43_arch.h"     

// Pilha TCP/IP leve usada para comunicação via rede
#include "lwip/pbuf.h"           //  manipulação de buffers de pacotes de rede
#include "lwip/tcp.h"            // funções e estruturas para protocolo TCP
#include "lwip/netif.h"          // funções e estruturas para interfaces de rede (netif)

//Bibliotecas de apoio
#include "include/buzzer.h"
#include "include/matriz_led.h"
#include "include/ssd1306.h"
#include "include/font.h"
#include "include/modos.h"

// Dados da rede Wi-Fi (ATENÇÃO: ALTERAR ANTES DE SUBIR NO GIT)
#define WIFI_SSID "BRENO-2G"
#define WIFI_PASSWORD "991773729"

// Definição dos pinos dos LEDs
#define LED_PIN CYW43_WL_GPIO_LED_PIN  
#define LED_BLUE_PIN 12                 
#define LED_GREEN_PIN 11                
#define LED_RED_PIN 13           


// Tipos de modo do sistema
typedef enum {
    MODO_CONFORTO,
    MODO_FESTA,
    MODO_SEGURANCA,
    MODO_SONO
} ModoSistema;

//Variável global do atual modo do sistema
ModoSistema modo_atual = MODO_CONFORTO;

// -------------------------------------- Funções ---------------------------------
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
}

// Tratamento do request do usuário
void user_request(char **request){

    if (strstr(*request, "GET /modo_conforto") != NULL)
    {
        set_modo(MODO_CONFORTO);
    }
    else if (strstr(*request, "GET /modo_festa") != NULL)
    {
        set_modo(MODO_FESTA);
    }
    else if (strstr(*request, "GET /modo_seguranca") != NULL)
    {
        set_modo(MODO_SEGURANCA);
    }
    else if (strstr(*request, "GET /modo_sono") != NULL)
    {
        set_modo(MODO_SONO);
    }
};

// Leitura da temperatura interna
float temp_read(void){
    adc_select_input(4);
    uint16_t raw_value = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float temperature = 27.0f - ((raw_value * conversion_factor) - 0.706f) / 0.001721f;
        return temperature;
}

// Função de callback para processar requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    // Alocação do request na memória dinámica
    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    // Tratamento de request - Controle dos LEDs
    user_request(&request);
    
    // Leitura da temperatura interna
    float temperature = temp_read();

    // Cria a resposta HTML
    char html[1024];

    // Instruções html do webserver
    snprintf(html, sizeof(html), // Formatar uma string e armazená-la em um buffer de caracteres
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<!DOCTYPE html>\n"
             "<html>\n"
             "<head>\n"
             "<title> Embarcatech - LED Control </title>\n"
             "<style>\n"
             "body { background-color: #b5e5fb; font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
             "h1 { font-size: 64px; margin-bottom: 30px; }\n"
             "button { background-color: LightGray; font-size: 36px; margin: 10px; padding: 20px 40px; border-radius: 10px; }\n"
             ".temperature { font-size: 48px; margin-top: 30px; color: #333; }\n"
             "</style>\n"
             "</head>\n"
             "<body>\n"
             "<h1>AtmoSync</h1>\n"
             "<h2>Selecione o modo:</h1>\n"
             "<form action=\"./modo_conforto\"><button>Modo Conforto</button></form>\n"
             "<form action=\"./modo_festa\"><button>Modo Festa</button></form>\n"
             "<form action=\"./modo_seguranca\"><button>Modo Segurança</button></form>\n"
             "<form action=\"./modo_sono\"><button>Modo Sono</button></form>\n"
             "<p class=\"temperature\">Temperatura Interna: %.2f &deg;C</p>\n"
             "</body>\n"
             "</html>\n",
             temperature);

    // Escreve dados para envio (mas não os envia imediatamente).
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);

    // Envia a mensagem
    tcp_output(tpcb);

    //libera memória alocada dinamicamente
    free(request);
    
    //libera um buffer de pacote (pbuf) que foi alocado anteriormente
    pbuf_free(p);

    return ERR_OK;
}

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}


// Função principal
int main(){
    stdio_init_all();
    gpio_led_bitdog();

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

    // Configura servidor TCP
    struct tcp_pcb *server = tcp_new();
    if (!server || tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao criar servidor\n");
        return -1;
    }
    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    // Inicializa o conversor ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);

    while (true){
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);      // Reduz o uso da CPU
    }

    //Desligar a arquitetura CYW43.
    cyw43_arch_deinit();
    return 0;
}












