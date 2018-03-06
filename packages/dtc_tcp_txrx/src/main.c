/* 
 * Customized TCP sender and receiver
 *
 * Note:
 * 1) Server only accepts one connection
 *
 * Huanle Zhang at UC Davis
 * www.huanlezhang.com
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // exit
#include <unistd.h> // close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

static const int print_interval = 5; // 5 seconds

static const int send_buffer_len = 2000;
static const int recv_buffer_len = 5000;
static char *send_buffer;
static char *recv_buffer;

static struct sockaddr_in si_target;
static struct sockaddr_in si_self;

static unsigned long packet_recv_count = 0;
static unsigned long packet_send_count = 0;

static char debug_string_buffer[100];

static struct timespec delay; 

// ------ argument processing ------
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

static int print_packet = 0; // print packet or not
static int echo_back = 0; // 

// ------ end of argument processing ------

void startServer(void);
void startClient(void);



void die(const char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(-1);
}

void usage(void)
{
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

/*
 * If --client && --send-file is not specified, then put seq number in the first 4 bytes 
 *      of payload. Packet length is set to the buffer length
 *
 *  --echo-back only works on server
 */
}

void argumentProcess(int argc, char **argv)
{

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
        } else if (strcmp(argv[i], "--print-packet") == 0){
            print_packet = 1;
        } else if (strcmp(argv[i], "--echo-back") == 0){
           echo_back = 1; 
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

    send_buffer = (char *) malloc(send_buffer_len);
    recv_buffer = (char *) malloc(recv_buffer_len);
    memset(send_buffer, 0, send_buffer_len);
    memset(recv_buffer, 0, recv_buffer_len);

    delay.tv_sec = 1;
    delay.tv_nsec = 0;
    
    // print detailed information
    printf("------ Configuration Info ------\n");
    printf("Mode: ");
    if (server_or_client == 1){
        printf("TCP server\n");
    } else if (server_or_client == 2){
        printf("TCP client\n");
    }
   
    if (server_or_client == 1){
        // server info
        printf("Self IP: %s\n", self_ip_str);
        printf("Self Port: %s\n", self_port_str);
        
        printf("Print packet: " );
        print_packet == 0 ? printf("False\n") : printf("True\n");

        printf("Echo back: ");
        echo_back == 0 ? printf("Flase\n") : printf("True\n");
    } else if (server_or_client == 2){
        printf("Target IP: %s\n", target_ip_str);
        printf("Target Port: %s\n", target_port_str);
        
        printf("Print packet: " );
        print_packet == 0 ? printf("False\n") : printf("True\n");
    }
    
    printf("------ End of Configuration Info ------\n");
}

static int packet_read_size = 0;
void timer_handler(int signum)
{
    if (packet_read_size > 0)
        printf("packet #: %ld %.*s\n", packet_recv_count, packet_read_size, recv_buffer);
}

int main(int argc, char **argv)
{
    argumentProcess(argc, argv);

    if (server_or_client == 1){
        startServer();
    } else if (server_or_client == 2){
        startClient();
    }

    return 0;
}

void startServer(void)
{
    int server_fd = 0, connection_fd = 0;
    int addrlen = 0;
    struct sockaddr_in si_client;
    const int MAX_ALLOW_PENDING_CONNECTION = 10;
    char ip_str[INET_ADDRSTRLEN];
    // timer related
    struct sigaction sa;
    struct itimerval timer;

    if (print_packet == 1){
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &timer_handler;
        sigaction(SIGALRM, &sa, NULL);
        timer.it_value.tv_sec = print_interval;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = print_interval;
        timer.it_interval.tv_usec = 0;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1){
        die("*** Error\n socket in socket() failed");
    }

    if (bind(server_fd, (struct sockaddr *)&si_self, sizeof(si_self)) == -1){
        die("*** Error\n bind in startServer() failed");
    }
    
    if (listen(server_fd, MAX_ALLOW_PENDING_CONNECTION) == -1){
        die("*** Error\n listen in startServer() failed");
    }

    if (inet_ntop(AF_INET, &si_self.sin_addr.s_addr, ip_str, INET_ADDRSTRLEN) == NULL){
        die("*** Error\n inet_ntop in startServer() failed");
    }
    printf("TCP server: %s %u waiting for incoming connection...\n", 
                ip_str, ntohs(si_self.sin_port));
    
    // only accept one client 
    connection_fd = accept(server_fd, (struct sockaddr *)&si_client, (socklen_t *)&addrlen);
    if (connection_fd == -1){
        die("*** Error\n accept in startServer() failed");
    }

    if (print_packet == 1) setitimer(ITIMER_REAL, &timer, NULL);

    while (1){
        packet_read_size = recv(connection_fd, recv_buffer, recv_buffer_len, 0);
        
        if (packet_read_size > 0) {
            packet_recv_count++;

            if (echo_back == 1 && 
                    write(connection_fd, recv_buffer, packet_read_size) == -1){
                die("*** Error\n write in startServer() failed");
            }
        }
    }

    if (print_packet == 1){
        // cancel timer
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);
    }

    printf("Server exit\n");
    close(server_fd);
}

void startClient(void)
{
    int server_fd = 0;
    
    // timer related
    struct sigaction sa;
    struct itimerval timer;

    if (print_packet == 1){
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &timer_handler;
        sigaction(SIGALRM, &sa, NULL);
        timer.it_value.tv_sec = print_interval;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = print_interval;
        timer.it_interval.tv_usec = 0;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1){
        die("*** Error\n socket in startClient() failed");
    }

    if (connect(server_fd, (struct sockaddr *)&si_target, sizeof(si_target)) == -1){
        die ("** Error\n connect in startClient() failed");
    }

    if (print_packet == 1) setitimer(ITIMER_REAL, &timer, NULL);

    while (1){
        packet_send_count++;
        /*
        send_buffer[0] = (char) (packet_send_count >> 24);
        send_buffer[1] = (char) (packet_send_count >> 16);
        send_buffer[2] = (char) (packet_send_count >> 8);
        send_buffer[3] = (char) (packet_recv_count >> 0);
        */
        send_buffer[0] = '0' + (packet_send_count/1000) % 10;
        send_buffer[1] = '0' + (packet_send_count/100) % 10;
        send_buffer[2] = '0' + (packet_send_count/10) % 10;
        send_buffer[3] = '0' + (packet_send_count) % 10;

        nanosleep(&delay, NULL);

        if (send(server_fd, send_buffer, 4, 0) < 0){
            die("*** Error\n send in startClient() failed");
        }

        packet_read_size = recv(server_fd, recv_buffer, recv_buffer_len, MSG_DONTWAIT); 
        if (packet_read_size > 0)
            packet_recv_count++;
    }

    close(server_fd);
    printf("Exit Client\n");
}
