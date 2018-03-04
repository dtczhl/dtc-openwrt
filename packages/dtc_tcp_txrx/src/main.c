/* 
 * Customized TCP sender and receiver
 *
 *
 * Huanle Zhang at UC Davis
 * www.huanlezhang.com
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static int send_buffer_len = 2000;
static int recv_buffer_len = 5000;

static struct sockaddr_in *si_server;
static struct sockaddr_in *si_client;

static unsigned long packet_recv_count = 0;
static unsigned long packet_send_count = 0;

static char debug_string_buffer[100];

void die(const char *s){
    fprintf(stderr, "%s\n", s);
    exit(-1);
}

void usage(void){
    printf("Usage: dtc_tcp_txrx [--server|--client] \n");
    printf("        [--server-address ip port] [--client-address ip port]\n");
    printf("        [--send-file file_path] [--recv-file filename]\n");
    printf("        [--record-directory directory_path] [--record-id id]\n");
    printf("        [--send-log] [--recv-log]\n");
    printf("        [--print-packet]\n");
    printf("        [--help]\n");
    printf("    --server|--client (required)    TCP server or client\n");
    printf("    --server-address ip port        0.0.0.0 for any ip, 0 for any port\n");
    printf("    --client-address ip port        0.0.0.0 for any ip, 0 for any port\n");
    printf("    --send-file file_path           point to the file to be send\n");
    printf("    --recv-file filename            save payload to file\n");
    printf("    --send-log                      log detailed send information\n");
    printf("    --recv-log                      log detailed recv information\n");
    printf("    --print-packet                  print packet seq and content every 5 seconds\n");
    printf("    --help                          print help information\n");
}

void argumentProcess(int argc, char **argv){

    if (argc == 1){
        usage();
        exit(0);
    }

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--server") == 0){
        
        } else if (strcmp(argv[i], "--client") == 0){
        
        } else {
            snprintf(debug_string_buffer, sizeof(debug_string_buffer), 
                    "*** Error\n unknown argument: %s", argv[i]);
            die(debug_string_buffer);
        } 
    }

}

int main(int argc, char **argv){

    argumentProcess(argc, argv);

    return 0;
}
