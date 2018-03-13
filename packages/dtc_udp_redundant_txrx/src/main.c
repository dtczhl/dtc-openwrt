/* 
 * Customized UDP sender and receiver
 *      with redundant path scheduling
 *
 *  Huanle Zhang at UC Davis
 *  www.huanlezhang.com
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
#include <sys/select.h>
#include <pthread.h>

static const int print_interval = 5; // in seconds
static const int log_packet_length = 4; // record first bytes

#define send_buffer_len 2000
#define recv_buffer_len 5000
static char send_buffer[send_buffer_len];
static char recv_buffer[recv_buffer_len];


#define MAX_SOCKADDR_NUM 10
static struct sockaddr_in si_target[MAX_SOCKADDR_NUM];
static struct sockaddr_in si_self[MAX_SOCKADDR_NUM];

static unsigned int packet_recv_count = 0;
static unsigned int packet_send_count = 0;

static char debug_string_buffer[100];

// ------ argument processing ------
static int server_or_client = -1; // 1 - server; 2 - client

#define INET_PORTSTRLEN 6

static char target_ip_str[MAX_SOCKADDR_NUM][INET_ADDRSTRLEN];
static char target_port_str[MAX_SOCKADDR_NUM][INET_PORTSTRLEN];
static int num_target_sockaddr = 0; // number of target sockaddr

static char self_ip_str[MAX_SOCKADDR_NUM][INET_ADDRSTRLEN];
static char self_port_str[MAX_SOCKADDR_NUM][INET_PORTSTRLEN];
static int num_self_sockaddr = 0; // number of self sockaddr

static int print_packet = 0; // print packet or not
static int echo_back = 0; // echo back or not

static struct timespec send_interval; // packet send interval for client

static int send_log = 0;
static int recv_log = 0;
static int record_id = 0;
static FILE *fd_send_record = NULL;
static FILE *fd_recv_record = NULL;
static char record_directory[100];
static char sendRecordFilenamePrefix[] = "dtc_udp_redundant_txrx-send";
static char sendRecordFilename[100];
static char recvRecordFilenamePrefix[] = "dtc_tcp_redundant_txrx-recv";
static char recvRecordFilename[100];
static struct timespec ts_current_send; // send time log
static struct timespec ts_current_recv; // recv time log

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
    printf("Usage: dtc_udp_redundant_txrx [--server|--client]\n");
    printf("        [--target-address ip port] [--self-address ip port]\n");
    printf("        [--record-directory path] [--record-id id]\n");
    printf("        [--send-log] [--recv-log]\n");
    printf("        [--send-interval nanoseconds]\n");
    printf("        [--print-packet] [--echo-back]\n");
    printf("        [--help]\n");
    printf("    --server|--client (required)    UDP server or client\n");
    printf("    --target-address ip port        0.0.0.0 for any ip, 0 for any port\n");
    printf("    --self-address ip port          0.0.0.0 for any ip, 0 for any port\n");
    printf("    --record-directory path         directory fo logging to files\n");
    printf("    --send-log                      log detailed send information\n");
    printf("    --send-interval nanoseconds     packet send interval\n");
    printf("    --recv-log                      log detailed recv information\n");
    printf("    --print-packet                  print packet seq and content every %d seconds\n", print_interval);
    printf("    --echo-back                     echo back the received packets\n");
    printf("    --help                          print help information\n");

    printf("Server side only:\n");
    printf("    --echo-back\n");
    printf("Client side only:\n");
    printf("    --send-interval\n");
}

void argumentProcess(int argc, char **argv)
{
    if (argc == 1) {
        usage ();
        exit (0);
    }

    send_interval.tv_sec = 0;
    send_interval.tv_nsec = 0;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--server") == 0) {
            server_or_client = 1;
        } else if (strcmp(argv[i], "--client") == 0) {
            server_or_client = 2;
        } else if (strcmp(argv[i], "--target-address") == 0) {
            i++;
            if (inet_pton(AF_INET, argv[i], &si_target[num_target_sockaddr].sin_addr.s_addr) != 1)
                die ("*** Error\n check --target-address");
            i++;
            si_target[num_target_sockaddr].sin_port = htons((unsigned short) atoi(argv[i]));
            num_target_sockaddr++; // increase the number of target sockaddr
        } else if (strcmp(argv[i], "--self-address") == 0) {
            i++;
            if (inet_pton(AF_INET, argv[i], &si_self[num_self_sockaddr].sin_addr.s_addr) != 1)
                die ("*** Error\n check --self-address");
            i++;
            si_self[num_self_sockaddr].sin_port = htons((unsigned short) atoi(argv[i]));
            num_self_sockaddr++; // increase the number of self sockaddr
        } else if (strcmp(argv[i], "--print-packet") == 0) {
            print_packet = 1;
        } else if (strcmp(argv[i], "--echo-back") == 0) {
            echo_back = 1;
        } else if (strcmp(argv[i], "--send-interval") == 0) {
            i++;
            send_interval.tv_sec = 0;
            send_interval.tv_nsec = atol(argv[i]);
            while (send_interval.tv_nsec >= 1E9) {
                send_interval.tv_sec++;
                send_interval.tv_nsec -= 1E9;
            }
        } else if (strcmp(argv[i], "--record-id") == 0) {
            i++;
            record_id = atoi(argv[i]);
        } else if (strcmp(argv[i], "--send-log") == 0) {
            send_log = 1;
        } else if (strcmp(argv[i], "--recv-log") == 0) {
            recv_log = 1;
        } else if (strcmp(argv[i], "--record-directory") == 0) {
            i++;
            if (argv[i][strlen(argv[i]) - 1 ] == '/')
                argv[i][strlen(argv[i]) - 1 ] = 0;
            if (snprintf(record_directory, sizeof(record_directory), "%s", argv[i]) < 0)
                die ("*** Error\n check --record-directory");
        } else { // unknown 
            snprintf(debug_string_buffer, sizeof(debug_string_buffer), 
                    "*** Error\n unknown argument: %s", argv[i]);
            die (debug_string_buffer);
        }
    }

    // check arguments
    if (server_or_client == -1) die ("*** Error\n --server or --client must be specified");
    if (server_or_client == 1) {
        // server
        if (num_self_sockaddr <= 0) die ("*** Error\n self for server must be specified");

        for (int i = 0; i < num_self_sockaddr; i++) {
            if (si_self[i].sin_port == 0){
                die ("*** Error\n No server port specified");
            }
            si_self[i].sin_family = AF_INET;
        }
    } else if (server_or_client == 2){
        // client
        if (num_target_sockaddr <= 0) die ("*** Error\n target for server must be specified");

        for (int i = 0; i < num_target_sockaddr; i++) {
            if (si_target[i].sin_addr.s_addr == 0 || si_target[i].sin_port == 0)
                die ("*** Error\n No server ip port specified");
            si_target[i].sin_family = AF_INET;

        }

        if (send_interval.tv_sec == 0 && send_interval.tv_nsec == 0)
            die ("*** Error\n Client must specify --send-interval");

        for (int i = 0; i < num_self_sockaddr; i++) {
            si_self[i].sin_family = AF_INET;
        }
    } else {
        die ("*** Error\n check server_or_client");
    }

    // common for client and server
    if (send_log == 1 || recv_log == 1) {
        if (strlen(record_directory) == 0) {
            die ("*** Error\n If you wanna save data, must specify the directory as well");
        }
    }

    if (send_log == 1) {
        if (snprintf(sendRecordFilename, sizeof(sendRecordFilename), "%s/%s-%d.csv",
                record_directory, sendRecordFilenamePrefix, record_id) < 0)
            die ("*** Error\n sendRecordFilename snprintf failed");
        fd_send_record = fopen(sendRecordFilename, "w");
        if (fd_send_record == NULL) 
            die ("*** Error\n cannot open file for send record");
    }

    if (recv_log == 1) {
        if (snprintf(recvRecordFilename, sizeof(recvRecordFilename), "%s/%s-%d.csv",
                record_directory, recvRecordFilenamePrefix, record_id) < 0)
            die ("*** Error\n recvRecordFilename snprintf failed");
        fd_recv_record = fopen(recvRecordFilename, "w");
        if (fd_recv_record == NULL)
            die ("*** Error\n cannot open file for recv record");
    }

    // print detailed information
    printf ("------ Configuration Info ------\n");
    printf ("Mode: ");
    if (server_or_client == 1)
        printf ("UDP server\n");
    else if (server_or_client == 2) 
        printf ("UDP client\n");

    if (server_or_client == 1) {
        // server info
        for (int i = 0; i < num_self_sockaddr; i++) {
            if (inet_ntop(AF_INET, &si_self[i].sin_addr.s_addr, 
                    debug_string_buffer, sizeof(debug_string_buffer)) == NULL) 
                die ("*** Error\n inet_ntop in server info failed");
            printf ("Self IP: %s\n", debug_string_buffer);
            printf ("Self Port: %u\n", ntohs(si_self[i].sin_port));
        }

        printf ("Print packet: ");
        print_packet == 0 ? printf ("False\n") : printf("True\n");

        printf("Echo back: ");
        echo_back == 0 ? printf ("False\n") : printf("True\n");
    } else if (server_or_client == 2) {
        // client info
        for (int i = 0; i < num_target_sockaddr; i++) {
            if (inet_ntop(AF_INET, &si_target[i].sin_addr.s_addr, 
                    debug_string_buffer, sizeof(debug_string_buffer)) == NULL)
                die ("*** Error\n inet_ntop in client info failed");
            printf ("Target IP: %s\n", debug_string_buffer);
            printf ("Target Port: %u\n", ntohs(si_target[i].sin_port));
        }

        printf ("Print packet: ");
        print_packet == 0 ? printf("False\n") : printf("True\n");

        printf ("Packet send interval: %ld (s) %ld (ns)\n", send_interval.tv_sec, send_interval.tv_nsec);
    }
}

int main(int argc, char **argv)
{
    argumentProcess(argc, argv);


    return 0;
}
