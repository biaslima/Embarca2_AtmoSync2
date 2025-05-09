#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

// Função para inicializar o servidor web
void webserver_init(void);

// Callback para lidar com as requisições recebidas
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

// Callback para aceitar novas conexões
err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);

// Função para processar requisições do usuário
void user_request(char **request);

#endif