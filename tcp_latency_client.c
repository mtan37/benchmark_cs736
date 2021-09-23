/*Measuring RTT in linux
client end program
pre-define SIGNUNSED to 31

Reference: CS736 fall 2021
*/

#define SIGUNUSED 31
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

extern char *strsignal (int __sig);

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>
#include <signal.h>
#include <arpa/inet.h>

const size_t BUF_SIZE = 512 * 1024;//512K bytes

#define MAX_MSG_SIZE 200
#define MAX_ARG_SIZE 128

#define min_t(type, x, y) \
({ type __x=x; type __y=y; __x<__y? __x:__y })

struct s_args {
 long server_port;
 struct addrinfo *addr;
 struct addrinfo *result;
 long num_rt;
 long data_size;
 int ms;
} s_args;

int g_cur_rtn = 0;
unsigned long S_TO_NS = 1000000000;

uint64_t calculate_tick(unsigned a, unsigned b) {
    uint64_t tick;
    tick = (((uint64_t)a) | (((uint64_t)b) << 32));// calculating tick value
    return tick;
} // caculate the ticker

double tick_to_ns(uint64_t tick2, uint64_t tick1) {
    return (double)(tick2 - tick1)*S_TO_NS/3200000000;
}

void set_cur_rtn(int cur_rtn)
{
        g_cur_rtn = cur_rtn;
}

int get_cur_rtn()
{
        return g_cur_rtn;
}

void sig_handler(int sigid)
{
        fprintf(stderr, "client: current RT nr = %d\n", get_cur_rtn());
        fprintf(stderr, "client: unexpected signal: %s: exit \n",strsignal(sigid));
        exit(-1);
}

void init_sig()
{
         struct sigaction sa_s;
         int sn;

         sa_s.sa_handler = sig_handler;
         sa_s.sa_flags = 0;
         sigemptyset(&(sa_s.sa_mask));
         for (sn=0; sn <= SIGUNUSED; sn++)
                sigaction(sn, &sa_s, NULL);
}

void init_message(int argc, char *argv[], struct s_args *p)
{

        long server_port;
        long num_rt;
        long data_size;
        int ms;
        int rc;

        server_port = strtol(argv[2], NULL, 0);
        p->server_port = server_port;

        rc = getaddrinfo(argv[4], argv[2], p->addr, &(p->result));
        num_rt = strtol(argv[6], NULL, 0);
        p->num_rt = num_rt;

        data_size = strtol(argv[8], NULL, 0);         
        p->data_size = data_size;
	ms = strtol(argv[10], NULL, 0);
        
        p->ms = ms;
}


int socket_and_connect(struct addrinfo *ps)
{
        struct addrinfo *rp;
        int client_socket;
        int success = 0;

        for (rp = ps; rp != NULL; rp = rp->ai_next)
        {
                if ((client_socket=socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
                        continue;
                }

                if (connect(client_socket, rp->ai_addr, rp->ai_addrlen) != - 1)
                {
                        success = 1;
                        break;
                }
                else {
                        continue;
                }
        }
        if (success > 0 )
                printf("client: connected ... \n");
        else {
                printf("client: cannot connect: exit. \n");
                exit(-1);
        }
        return client_socket;
}

void send_round_trip_num_and_data_size(int client_socket, long num_rt, long data_size)
{
        char buf[MAX_ARG_SIZE];

        memcpy((void *)buf, (void *)&num_rt, sizeof(num_rt));
        if (write(client_socket, buf, sizeof(num_rt)) == - 1) {
                perror("write rt_num failed\n");
                exit(-1);
        }

        memcpy((void *)buf,  (void *)&data_size, sizeof(data_size));
        if (write(client_socket, buf, sizeof(data_size)) == - 1) {
                perror("write data_size failed\n");
                exit(-1);
        }
}

char *alloc_data(long data_size)
{
        char *data;
        int i;

        data = (char *)malloc(data_size);
        if ( data ==  NULL ) {
              fprintf(stderr, "client: mmaloc %ld failed \n",data_size);
              exit(-1);
        }
        for (i=0; i < data_size; i++)
                *(data + i) = 'P';
        return data;
}

double send_receive_tcp(int rc, long num_rt, char *data, long data_size, int client_socket)
{
        uint64_t start_ct_rdp, end_ct_rdp;
        double diff = 0, diff_com = 0;
        long byte_left, byte_written, byte_read;
        unsigned a,b,c,d; // for assembly instruction
        
        for (int i = 0; i < num_rt; i++)
        {
        	__asm__ volatile("rdtsc" : "=a" (a), "=d" (b)); 
        	start_ct_rdp = calculate_tick(a,b);

                byte_left = data_size;

                while (byte_left > 0 && (-1 != (byte_written = write(client_socket, data, byte_left))))
                {
                        byte_left = byte_left - byte_written;      
                }

                byte_left = data_size;
                
                while( byte_left > 0 && (-1 != (byte_read = read(client_socket, data, byte_left))))
                {
                        byte_left = byte_left - byte_read; 
           
                }
                                       
                __asm__ volatile("rdtsc" : "=a" (c), "=d" (d));

               wait(NULL);    
        	end_ct_rdp = calculate_tick(c,d);
        	
        	diff = tick_to_ns(end_ct_rdp, start_ct_rdp);
        	
        	if((i == 0) && (diff > 0))
        	{
        		diff_com = diff;
        	}

                if((i > 0) && (diff < diff_com))
                {
                        diff_com = diff;
                }
        }
        
        return diff_com;
}

int main(int argc, char*argv[])
{
        int client_socket;
        struct addrinfo hints, *s_addr;
        int rc, rc2;
        long num_rt;
        long data_size;
        char *data;
        struct tms tms_s;
        clock_t start_ct, end_ct;
        int ms;
        struct timespec ts_s;
        struct rusage ru_s;
        struct s_args args_s;
        long m_sizes[10] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024, 64 * 1024, 256 * 1024, 512 * 1024};

        init_sig();

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = 0;
        hints.ai_protocol = 0;
        args_s.addr = &hints;
	init_message(argc, argv, &args_s);
        s_addr = args_s.result;
        num_rt = args_s.num_rt;
        ms = args_s.ms;
	data_size = 2 * m_sizes[9];
        printf("num_rt = %ld \n", num_rt);
        printf("data_size = %ld \n", data_size);

        client_socket = socket_and_connect(s_addr);
	
	send_round_trip_num_and_data_size(client_socket, num_rt, data_size);

        data = alloc_data(data_size);

        ts_s.tv_sec = ms / 1000;
        ts_s.tv_nsec = (ms % 1000) * 1000000;
        
        for(int i = 0;i < 10;i++)
        {
             printf("client: start sending %ld data bytes round trips ...\n", m_sizes[i]);
             double diff = send_receive_tcp(rc, num_rt, data, m_sizes[i], client_socket);
             printf("elapsed time        : %f ns \n",diff);
    	     printf("estimated latency   : %f ns \n" ,diff/2);
    	     printf("\n");
        }
         free(data);
       
        rc = getrusage(RUSAGE_SELF, &ru_s);

        return(0);
}


