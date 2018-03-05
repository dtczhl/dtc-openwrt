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

static struct sockaddr_in si_target;
static struct sockaddr_in si_self;

static unsigned long packet_recv_count = 0;
static unsigned long packet_send_count = 0;

static char debug_string_buffer[100];

// argument processing
static int server_or_client = -1; // 1 - server; 2 - client

#define INET_PORTSTRLEN 6
static unsigned int target_ip = 0;
static unsigned short target_port = 0;
static char target_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
static char target_port_str[INET_PORTSTRLEN] = "0";

static unsigned int self_ip = 0;
static unsigned short self_port = 0;
static char self_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
static char self_port_str[INET_PORTSTRLEN] = "0";

void die(const char *s){
    fprintf(stderr, "%s\n", s);
    exit(-1);
}

void usage(void){
    printf("Usage: dtc_tcp_txrx [--server|--client] \n");
    printf("        [--target-address ip port] [--self-address ip port]\n");
    printf("        [--send-file file_path] [--recv-file filename]\n");
    printf("        [--record-directory directory_path] [--record-id id]\n");
    printf("        [--send-log] [--recv-log]\n");
    printf("        [--print-packet] [--echo-back]\n");
    printf("        [--help]\n");
    printf("    --server|--client (required)    TCP server or client\n");
    printf("    --target-address ip port        0.0.0.0 for any ip, 0 for any port\n");
    printf("    --self-address ip port          0.0.0.0 for any ip, 0 for any port\n");
    printf("    --send-file file_path           point to the file to be send\n");
    printf("    --recv-file filename            save payload to file\n");
    printf("    --send-log                      log detailed send information\n");
    printf("    --recv-log                      log detailed recv information\n");
    printf("    --print-packet                  print packet seq and content every 5 seconds\n");
    printf("    --echo-back                     echo back the received packets\n");
    printf("    --help                          print help information\n");
}

void argumentProcess(int argc, char **argv){

    if (argc == 1){
        usage();
        exit(0);
    }

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--server") == 0){
            if (server_or_client != -1){
                die("*** Error\n server_or_client has been assigned alreay");
            }
            server_or_client = 1;
        } else if (strcmp(argv[i], "--client") == 0){
            if (server_or_client != -1){
                die("*** Error\n server_or_client has been assigned alreay");
            }
            server_or_client = 2;
        } else if (strcmp(argv[i], "--target-address") == 0){
            if (argc - i < 2) die("*** Error\n check --target-address option");
            i++;
            if (inet_pton(AF_INET, argv[i], &target_ip) != 1) die("*** Error\n check --target-address option");
            // convert ip back to string
            if (inet_ntop(AF_INET, &target_ip, target_ip_str, INET_ADDRSTRLEN) == NULL) 
                die("*** Error\n --target-address cannot convert ip back");
            i++;

            target_port = htons((unsigned short) atoi(argv[i]));
            // convert port back to string
            if (snprintf(target_port_str, INET_PORTSTRLEN, "%u", ntohs(target_port)) < 1)
                die("*** Error\n --target-address cannot convert port back");
        } else if (strcmp(argv[i], "--self-address") == 0){
            if (argc - i < 2) die("*** Error\n check --self-address option");
            i++;
            if (inet_pton(AF_INET, argv[i], &self_ip) != 1) die("*** Error\n check --self-address option");
            // convert ip back to string
            if (inet_ntop(AF_INET, &self_ip, self_ip_str, INET_ADDRSTRLEN) == NULL) 
                die("*** Error\n --self-address cannot convert ip back");
            i++;

            self_port = htons((unsigned short) atoi(argv[i]));
            // convert port back to string
            if (snprintf(self_port_str, INET_PORTSTRLEN, "%u", ntohs(self_port)) < 1)
                die("*** Error\n --self-address cannot convert port back");
        } else {
            snprintf(debug_string_buffer, sizeof(debug_string_buffer), 
                    "*** Error\n unknown argument: %s", argv[i]);
            die(debug_string_buffer);
        } 
    }

    // check arguments
    
    if (server_or_client == -1) die("*** Error\n --server or --client must be specified");
    if (server_or_client == 1){
        // server
        // self process
        if (self_port == 0){
            // need port to bind
            die("*** Error\n No server port specified");
        }
        si_self.sin_family = AF_INET;
        si_self.sin_addr.s_addr = self_ip == 0 ? INADDR_ANY : self_ip;
        si_self.sin_port = self_port;

        // target processing for server side remains extension
    
    } else if (server_or_client == 2){
        // client 
        // target process
        if (target_ip == 0 || target_port == 0)
            die("*** Error\n No server ip port specified");
        si_target.sin_family = AF_INET;
        si_target.sin_addr.s_addr = target_ip;
        si_target.sin_port = target_port;
        
        // self process
        si_self.sin_family = AF_INET;
        si_self.sin_addr.s_addr = self_ip == 0 ? INADDR_ANY : self_ip;
        si_self.sin_port = self_port; // 0 is allowed for any port
    } else {
        die("server_or_client error in checking");
    }
    
    // print detailed information
    printf("------ Configuration Info ------\n");
    printf("Mode: ");
    if (server_or_client == 1){
        printf("TCP server\n");
    } else if (server_or_client == 2){
        printf("TCP client\n");
    }
    
    printf("Target IP: %s\n", target_ip_str);
    printf("Target Port: %s\n", target_port_str);
    
    printf("Self IP: %s\n", self_ip_str);
    printf("Self Port: %s\n", self_port_str);
    
    printf("------ End of Configuration Info ------\n");
}

int main(int argc, char **argv){

    argumentProcess(argc, argv);

    return 0;
}
