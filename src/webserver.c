#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

#include "include/webserver.h"
#include "include/modos.h"
#include "setup.h"

// Tratamento do request do usuário
void user_request(char **request) {
    if (strstr(*request, "GET /modo_conforto") != NULL) {
        set_modo(MODO_CONFORTO);
    }
    else if (strstr(*request, "GET /modo_festa") != NULL) {
        set_modo(MODO_FESTA);
    }
    else if (strstr(*request, "GET /modo_seguranca") != NULL) {
        set_modo(MODO_SEGURANCA);
    }
    else if (strstr(*request, "GET /modo_sono") != NULL) {
        set_modo(MODO_SONO);
    }
}

// Função de callback para processar requisições HTTP
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    // Alocação do request na memória dinâmica
    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    // Tratamento de request - Controle dos LEDs
    user_request(&request);

    // Cria a resposta HTML
    char html[1024];

    // Instruções html do webserver
    snprintf(html, sizeof(html),
            "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
            "<title>AtmoSync</title>"
            "<style>"
            "body{background:#0075ac;font-family:sans-serif;text-align:center;margin-top:40px;color:#fff;}"
            "h1{font-size:48px;margin:20px;}"
            "button{background:#39a6ff;font-size:24px;margin:10px;padding:15px 25px;border-radius:100px;color:#fff;width: 300px;}"
            ".redondo{background:#c00000;border-radius:500px;width: 150px;}"
            "</style>"
            "</head><body>"
            "<h1>AtmoSync</h1><h2>Selecione o modo:</h2>"
            "<form action=\"./modo_conforto\"><button>Modo Conforto</button></form>"
            "<form action=\"./modo_festa\"><button>Modo Festa</button></form>"
            "<form action=\"./modo_seguranca\"><button>Modo Segurança</button></form>"
            "<form action=\"./modo_sono\"><button>Modo Sono</button></form>"
            "<form action=\"./alternar_leds\"><button class=\"redondo\">On/Off Luz</button></form>"
            "<form action=\"./desligar_alarme\"><button class=\"redondo\">Desligar Alarme</button></form>"
            "</body></html>"

        );

    // Escreve dados para envio
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);

    // Envia a mensagem
    tcp_output(tpcb);

    // Libera memória alocada dinamicamente
    free(request);
    
    // Libera o buffer de pacote
    pbuf_free(p);

    return ERR_OK;
}

// Função de callback ao aceitar conexões TCP
err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

// Inicializa o servidor web
void webserver_init(void) {
    // Configura servidor TCP
    struct tcp_pcb *server = tcp_new();
    if (!server || tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao criar servidor\n");
        return;
    }
    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");
}