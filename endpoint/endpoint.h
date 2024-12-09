#pragma once
//不加 extern 则被多次包含的时候会出现重定义错误
extern int client_end_point();
extern int server_end_point();
extern int create_tcp_socket();
extern int create_acceptor_socket();
extern int bind_acceptor_socket();
extern int connect_to_end();
extern int dns_connect_to_end();