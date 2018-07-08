/* 
 * Customized TCP sender and receiver
 *
 * Note:
 * 1) Server only accepts one connection and then dead
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
#include <sys/select.h>
#include <pthread.h>

#include "dtc_sleep.h"
#include "dtc_write_raw_2_text.h"

// in seconds; print packet info in terminal
#define PRINT_PACKET_INTERVAL 5
// record first bytes
#define LOG_PACKET_LENGTH 4 

// send buffer size
#define SEND_BUFFER_SIZE 5000
static int send_length = 508; // packet send length
// recv buffer size
#define RECV_BUFFER_SIZE 5000
static char send_buffer[SEND_BUFFER_SIZE];
static char recv_buffer[RECV_BUFFER_SIZE];

static struct sockaddr_in si_target; 
static struct sockaddr_in si_self;

static unsigned int packet_recv_count = 0;
static unsigned int packet_send_count = 0;

#define DEBUG_STRING_BUFFER_SIZE 100
static char debug_string_buffer[DEBUG_STRING_BUFFER_SIZE];

// ------ argument processing ------
static int server_or_client = -1; // 1 - server; 2 - client

#define INET_PORTSTRLEN 6
static char target_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
static char target_port_str[INET_PORTSTRLEN] = "0";
static char self_ip_str[INET_ADDRSTRLEN] = "0.0.0.0";
static char self_port_str[INET_PORTSTRLEN] = "0";

static int print_packet = 0;    // print packet or not
static int echo_back = 0;       //  server echo back packets or not

static struct timespec send_interval; // packet send interval for client

static int send_log = 0;    // record send packet info or not
static int recv_log = 0;    // record recv packet info or not
static int record_id = 0;   // filename suffix
static FILE *fd_send_record = NULL;     // file descriptor for send 
static FILE *fd_recv_record = NULL;     // file descriptor for recv
static char record_directory[100];      // length of directory path
static char sendRecordFilenamePrefix[] = "dtc_tcp_txrx-send"; // filename prefix
static char sendRecordFilename[200];    // final path for send
static char recvRecordFilenamePrefix[] = "dtc_tcp_txrx-recv"; // filename prefix
static char recvRecordFilename[200];    // final path for recv
static struct timespec ts_send;         // for interval sending
static struct timespec ts_current_send; // timestamp for sending
static struct timespec ts_current_recv; // timestamp for receiving

// ------ end of argument processing ------

static int server_fd = 0;

void startServer(void);
void startClient(void);

void die(const char *s)
{
    fprintf (stderr, "%s\n", s);
    exit (-1);
}

void usage(void)
{
printf ("Usage: dtc_tcp_txrx [--server|--client] \n");
printf ("        [--target-address ip port] [--self-address ip port]\n");
printf ("        [--record-directory path] [--record-id id]\n");
printf ("        [--send-log] [--recv-log]\n");
printf ("        [--send-interval nanoseconds]\n");
printf ("        [--send-length length]\n");
printf ("        [--print-packet] [--echo-back]\n");
printf ("        [--help]\n");
printf ("    --server|--client               TCP server or client\n");
printf ("    --target-address ip port        0.0.0.0 for any ip, 0 for any port\n");
printf ("    --self-address ip port          0.0.0.0 for any ip, 0 for any port\n");
printf ("    --record-directory path         directory for logging to files\n");
printf ("    --send-log                      log detailed send information\n");
printf ("    --send-interval nanoseconds     packet sending interval\n");
printf ("    --send-length length            packet length of sending\n");
printf ("    --recv-log                      log detailed recv information\n");
printf ("    --print-packet                  print packet seq and content every %d seconds\n",
                                                    PRINT_PACKET_INTERVAL);
printf ("    --echo-back                     echo back the received packets\n");
printf ("    --help                          print help information\n");

printf ("Server side only:\n");
printf ("    --echo-back\n");
printf ("Client side only:\n");
printf ("    --send-interval\n");
printf ("    --send-length\n");
}

void argumentProcess(int argc, char **argv)
{
    if (argc == 1) {
        usage ();
        exit (0);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp (argv[i], "--server") == 0) {
            server_or_client = 1;
        } else if (strcmp (argv[i], "--client") == 0) {
            server_or_client = 2;
        } else if (strcmp (argv[i], "--target-address") == 0) {
            i++;
            if (inet_pton (AF_INET, argv[i], &si_target.sin_addr.s_addr) != 1) 
                die ("*** Error\n check --target-address");
            i++;
            si_target.sin_port = htons ((unsigned short) atoi(argv[i]));
        } else if (strcmp (argv[i], "--self-address") == 0) {
            i++;
            if (inet_pton (AF_INET, argv[i], &si_self.sin_addr.s_addr) != 1) 
                die ("*** Error\n check --self-address");
            i++;
            si_self.sin_port = htons ((unsigned short) atoi(argv[i]));
        } else if (strcmp (argv[i], "--print-packet") == 0) {
            print_packet = 1;
        } else if (strcmp (argv[i], "--echo-back") == 0) {
           echo_back = 1; 
        } else if (strcmp (argv[i], "--send-interval") == 0) {
            i++;
            send_interval.tv_sec = 0;
            send_interval.tv_nsec = atol (argv[i]);
            while (send_interval.tv_nsec >= 1E9){
                send_interval.tv_sec++;
                send_interval.tv_nsec -= 1E9;
            }
        } else if (strcmp (argv[i], "--record-id") == 0) {
            i++;
            record_id = atoi (argv[i]);
        } else if (strcmp (argv[i], "--send-log") == 0) {
            send_log = 1;
        } else if (strcmp (argv[i], "--recv-log") == 0) {
            recv_log = 1;
        } else if (strcmp (argv[i], "--record-directory") == 0) {
            i++;
            if (argv[i][strlen (argv[i]) - 1] == '/') 
                argv[i][strlen (argv[i]) - 1] = 0;
            if (snprintf (record_directory, sizeof (record_directory), "%s", argv[i]) < 0) 
                die ("*** Error\n check --record-directory");
        } else if (strcmp (argv[i], "--send-length") == 0){
            i++;
            send_length = atoi (argv[i]);
        } else {
            snprintf (debug_string_buffer, DEBUG_STRING_BUFFER_SIZE, 
                    "*** Error\n unknown argument: %s", argv[i]);
            die (debug_string_buffer);
        } 
    }

    // ------ check arguments
    
    if (server_or_client == -1) 
        die("*** Error\n --server or --client must be specified");

    if (server_or_client == 1) {
        // server
        if (si_self.sin_port == 0) {
            // need port to bind
            die ("*** Error\n no server port specified");
        }

        si_self.sin_family = AF_INET;
    } else if (server_or_client == 2) {
        // client 
        if (si_target.sin_addr.s_addr == 0 || si_target.sin_port == 0)
            die ("*** Error\n no server ip port specified");
        si_target.sin_family = AF_INET;
       
        if (send_interval.tv_sec == 0 && send_interval.tv_nsec == 0)
            die ("*** Error\n client must specify --send-interval");

        if (send_length < LOG_PACKET_LENGTH) 
            die ("*** Error\n send_length < LOG_PACKET_LENGTH");
    } else {
        die ("*** Error\n check server_or_client");
    }

    // common for client and server
    if (send_log == 1 || recv_log == 1)
        if (strlen (record_directory) == 0)
            die ("*** Error\n If you wanna save data, specify the directory as well");

    if (send_log == 1) {
        if (snprintf (sendRecordFilename, sizeof (sendRecordFilename), "%s/%s-%d.csv", 
                record_directory, sendRecordFilenamePrefix, record_id) < 0)
            die ("*** Error\n sendRecordFilename snprintf failed");
        
        fd_send_record = fopen (sendRecordFilename, "w");

        if (fd_send_record == NULL) 
            die ("*** Error\n cannot open file for send record");
    }

    if (recv_log == 1) {
        if (snprintf (recvRecordFilename, sizeof (recvRecordFilename), "%s/%s-%d.csv",
                record_directory, recvRecordFilenamePrefix, record_id) < 0)
            die("*** Error\n recvRecordFilename snprintf failed");

        fd_recv_record = fopen (recvRecordFilename, "w");

        if (fd_recv_record == NULL) 
            die ("*** Error\n cannot open file for recv record");
    }

    // print detailed information
    printf ("------ Configuration Info ------\n");
    printf ("Mode: ");
    if (server_or_client == 1)
        printf("TCP server\n");
    else if (server_or_client == 2)
        printf("TCP client\n");
   
    if (server_or_client == 1){
        // server info
        if (inet_ntop (AF_INET, &si_self.sin_addr.s_addr,
                debug_string_buffer, DEBUG_STRING_BUFFER_SIZE) == NULL)
            die ("*** Error\n inet_ntop in server info failed");
        printf ("Self IP: %s\n", debug_string_buffer);
        printf ("Self Port: %u\n", ntohs (si_self.sin_port));
        
        printf ("Print packet: " );
        print_packet == 0 ? printf ("False\n") : printf ("True\n");

        printf ("Echo back: ");
        echo_back == 0 ? printf ("Flase\n") : printf ("True\n");
    } else if (server_or_client == 2){
        if (inet_ntop (AF_INET, &si_target.sin_addr.s_addr,
                debug_string_buffer, DEBUG_STRING_BUFFER_SIZE) == NULL)
            die ("*** Error\n inet_ntop in client info failed");
        printf ("Target IP: %s\n", debug_string_buffer);
        printf ("Target Port: %u\n", ntohs (si_target.sin_port));
        
        printf ("Print packet: " );
        print_packet == 0 ? printf ("False\n") : printf ("True\n");

        printf ("Packet send interval: %ld (s) %ld (ns)\n", 
                send_interval.tv_sec, send_interval.tv_nsec);

        printf ("Packet send length: %d\n", send_length);
    }

    if (send_log) 
        printf ("Save send log at %s\n", sendRecordFilename);

    if (recv_log) 
        printf ("Save recv log at %s\n", recvRecordFilename);
    
    printf ("------ End of Configuration Info ------\n");
}

static int packet_read_size = 0;
void timer_handler(int signum)
{
    if (packet_read_size >= LOG_PACKET_LENGTH)
        printf ("packet #: %u %.*s\n", 
                packet_recv_count, packet_read_size, recv_buffer);
}

int main(int argc, char **argv)
{
    argumentProcess (argc, argv);

    if (server_or_client == 1)
        startServer ();
    else if (server_or_client == 2)
        startClient ();

    return 0;
}

void startServer(void)
{
    int connection_fd = 0;
    int addrlen = 0;
    struct sockaddr_in si_client;
    const int MAX_ALLOW_PENDING_CONNECTION = 10;
    // timer related
    struct sigaction sa;
    struct itimerval timer;

    if (print_packet == 1) {
        memset (&sa, 0, sizeof (sa));
        sa.sa_handler = &timer_handler;
        sigaction (SIGALRM, &sa, NULL);
        timer.it_value.tv_sec = PRINT_PACKET_INTERVAL;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = PRINT_PACKET_INTERVAL;
        timer.it_interval.tv_usec = 0;
    }

    server_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) 
        die ("*** Error\n cannot create socket for server");

    if (bind (server_fd, (struct sockaddr *)&si_self, sizeof (si_self)) == -1)
        die ("*** Error\n cannot bind server socket");
    
    if (listen (server_fd, MAX_ALLOW_PENDING_CONNECTION) == -1)
        die ("*** Error\n cannot listen to server socket");
    
    printf ("Server: waiting for incoming connection...\n");
    
    // only accept one client 
    connection_fd = accept (server_fd, (struct sockaddr *)&si_client, (socklen_t *)&addrlen);
    if (connection_fd == -1)
        die ("*** Error\n cannot accept connection in server");

    printf ("Connection Accepted\n");

    if (print_packet == 1) 
        setitimer (ITIMER_REAL, &timer, NULL);

    while (1) {
        packet_read_size = recv(connection_fd, recv_buffer, RECV_BUFFER_SIZE, 0);
        
        if (packet_read_size >= LOG_PACKET_LENGTH) {
            packet_recv_count++;

            if (recv_log == 1) {
                if (clock_gettime (CLOCK_MONOTONIC, &ts_current_recv) != 0)
                    die ("*** Error\n clock_gettime in server recv failed");

                fprintf (fd_recv_record, "%lu,%lu,", 
                        ts_current_recv.tv_sec, ts_current_recv.tv_nsec);
                dtcWriteRaw2Text (fd_recv_record, recv_buffer, LOG_PACKET_LENGTH);
                fprintf (fd_recv_record, "\n");
            }
           
            if (echo_back == 1) {
                if (write (connection_fd, recv_buffer, packet_read_size) == -1)
                    die("*** Error\n server failed to echo back");
                
                if (send_log == 1) {
                    if (clock_gettime (CLOCK_MONOTONIC, &ts_current_send) != 0)
                        die ("*** Error\n clock_gettime in server send failed");

                    fprintf(fd_send_record, "%lu,%lu,",
                            ts_current_send.tv_sec, ts_current_send.tv_nsec);
                    dtcWriteRaw2Text (fd_send_record, recv_buffer, LOG_PACKET_LENGTH);
                    fprintf (fd_send_record, "\n");
                    fflush (fd_send_record);
                }
            }        
            if (recv_log == 1) fflush (fd_recv_record);
        }
    }

    printf("Server exit\n");

    if (print_packet == 1) {
        // cancel timer
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        setitimer (ITIMER_REAL, &timer, NULL);
    }

    // close file descriptors
    close (server_fd);
    if (send_log == 1) fclose (fd_send_record);
    if (recv_log == 1) fclose (fd_recv_record);
}

// ------ client processing ------

// client packet reception thread
void *client_packet_reception(void *threadid)
{
    while (1) {
        packet_read_size = recv (server_fd, recv_buffer, RECV_BUFFER_SIZE, 0); 
       
        if (packet_read_size >= LOG_PACKET_LENGTH) { 
            packet_recv_count++;

            if (recv_log == 1) {
                if (clock_gettime (CLOCK_MONOTONIC, &ts_current_recv) != 0)
                    die ("*** Error\n clock_gettime in client recv failed");

                fprintf (fd_recv_record, "%lu,%lu,", 
                        ts_current_recv.tv_sec, ts_current_recv.tv_nsec);
                dtcWriteRaw2Text (fd_recv_record, recv_buffer, LOG_PACKET_LENGTH);
                fprintf (fd_recv_record, "\n");
                fflush (fd_recv_record);
            }
        }
    }

    if (recv_log == 1)
        fclose (fd_recv_record);
}

void startClient(void)
{
    // timer related
    struct sigaction sa;
    struct itimerval timer;
    pthread_t packet_reception_thread;
    long t;

    if (print_packet == 1) {
        memset (&sa, 0, sizeof (sa));
        sa.sa_handler = &timer_handler;
        sigaction (SIGALRM, &sa, NULL);
        timer.it_value.tv_sec = PRINT_PACKET_INTERVAL;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = PRINT_PACKET_INTERVAL;
        timer.it_interval.tv_usec = 0;
    }

    server_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        die ("*** Error\n cannot create socket in client");

    if (connect (server_fd, (struct sockaddr *)&si_target, sizeof (si_target)) == -1)
        die ("** Error\n cannot connect to server in client");

    // create thread for packet reception
    if (pthread_create (&packet_reception_thread, NULL, 
                client_packet_reception, (void*)t) != 0) 
        die ("Error\n cannot create thread in client");

    if (print_packet == 1) 
        setitimer (ITIMER_REAL, &timer, NULL);

    if (clock_gettime (CLOCK_MONOTONIC, &ts_send) != 0)
        die ("*** Error\n clock_gettime in client send failed");

    while (1) {
        packet_send_count++;
        
        send_buffer[0] = (char) (packet_send_count >> 24);
        send_buffer[1] = (char) (packet_send_count >> 16);
        send_buffer[2] = (char) (packet_send_count >> 8);
        send_buffer[3] = (char) (packet_send_count >> 0);

        dtc_sleep (&ts_send, &send_interval);

        while (send (server_fd, send_buffer, send_length, 0) < 0)
            printf ("send in client send failed\n");

        if (send_log == 1) {
            if (clock_gettime (CLOCK_MONOTONIC, &ts_current_send) != 0)
                die ("*** Error\n clock_gettime in client send failed");
            fprintf (fd_send_record, "%lu,%lu,",
                     ts_current_send.tv_sec, ts_current_send.tv_nsec);
            dtcWriteRaw2Text (fd_send_record, send_buffer, LOG_PACKET_LENGTH);
            fprintf (fd_send_record, "\n");
        }

        ts_send.tv_sec += send_interval.tv_sec;
        ts_send.tv_nsec += send_interval.tv_nsec;
        while (ts_send.tv_nsec >= 1E9) {
            ts_send.tv_sec++;
            ts_send.tv_nsec -= 1E9;
        }
        
        if (send_log == 1) 
            fflush (fd_send_record);
    }
    
    printf("Exit Client\n");

    close (server_fd);
    if (send_log) 
        fclose (fd_send_record);
}
