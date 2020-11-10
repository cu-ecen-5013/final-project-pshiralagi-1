/*
/ @description: Socket program as described in assignment 6 of AESD
/ @date: 24th September 2020, recent edit 8th October
/ @author: Pavan Shiralagi
/ Reference: https://beej.us/guide/bgnet/html/#system-calls-or-bust, LSP, StackOverflow
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <sys/queue.h>
#include <sys/time.h>

//#define USE_AESD_CHAR_DEVICE 1 //Comment to use aesdsocketdata

#define PORT "9000"
#define BACKLOG 10
#define	SLIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = SLIST_FIRST((head));				\
	    (var) && ((tvar) = SLIST_NEXT((var), field), 1);		\
	    (var) = (tvar))
typedef struct slist_data_s slist_data_t;
struct slist_data_s {
	pthread_t threadNo;
	int thread_complete_flag, newFD;
	SLIST_ENTRY(slist_data_s) entries;
};

pthread_mutex_t lock;
int readLine(int fd, void *buffer, size_t n);
static void signal_handler (int signo);
void *socket_thread(void *arg);
void *timer_signal (void *arg);
int terminate, socket_descriptor;
char str[INET_ADDRSTRLEN];

int main(int argc, char *argv[])
{
	terminate = 0;

	int yes = 1;
	int get_addr, i, newFD, fd;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo *res, hints;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;  // use IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (pthread_mutex_init(&lock, NULL) != 0) { 
        fprintf(stderr, "\n mutex init has failed\n"); 
        exit(EXIT_FAILURE); 
    } 
	/*
	* Register signal_handler as our signal handler
	* for SIGINT.
	*/
	if (signal (SIGINT, signal_handler) == SIG_ERR) {
		fprintf (stderr, "Cannot handle SIGINT!\n");
		exit (EXIT_FAILURE);
	}
	/*
	* Register signal_handler as our signal handler
	* for SIGTERM.
	*/
	if (signal (SIGTERM, signal_handler) == SIG_ERR) {
		fprintf (stderr, "Cannot handle SIGTERM!\n");
		exit (EXIT_FAILURE);
	}
	if ((get_addr = getaddrinfo(NULL, PORT, &hints, &res)) != 0)
	{
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(get_addr));
        	return 1;
        }
	printf("Creating socket\n\r");
	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("server: socket");
		exit(1);
	}
	if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}

	printf("Binding\n\r");
	if (bind(socket_descriptor, res->ai_addr, res->ai_addrlen) == -1)
	{
		close(socket_descriptor);
		freeaddrinfo(res);
		perror("server: bind");
		exit(1);
	}
	freeaddrinfo(res);
	for(i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			printf("Daemon\n\r");
			daemon(1, 1);
		}	
	}
	printf("Listening\n\r");
	if (listen(socket_descriptor, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}
	addr_size = sizeof their_addr;
	fcntl(socket_descriptor, F_SETFL, O_NONBLOCK);
#ifdef USE_AESD_CHAR_DEVICE
	if ( access("/dev/aesdchar", F_OK) == -1)
	{
		fd = open ("/dev/aesdchar", O_RDWR | O_CREAT | O_APPEND, 0644);
	}
#else
	if ( access("/var/tmp/aesdsocketdata", F_OK) == -1)
	{
		fd = open ("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_APPEND, 0644);
	}
#endif
	if (fd == -1)
	{
		syslog(LOG_ERR, "Error Opening File");
		perror("open");
		exit(1);
	}
	close(fd);
	SLIST_HEAD(thread_list, slist_data_s) head;
	SLIST_INIT(&head);
	slist_data_t *datap = NULL;
	slist_data_t *datap_temp = NULL;
	i = 0;
#ifndef USE_AESD_CHAR_DEVICE
	datap = malloc(sizeof(slist_data_t));
	datap->threadNo = i++;
	datap->thread_complete_flag = 0;
	SLIST_INSERT_HEAD(&head, datap, entries);
	if(pthread_create(&datap->threadNo, NULL, timer_signal, NULL))
		printf("Failed to create thread");
#endif
	while(terminate != 1)
	{
		printf("Accepting\n\r");
		while ((newFD = accept(socket_descriptor, (struct sockaddr *)&their_addr, &addr_size)) <= 0)
		{
			if (terminate == 1)
				break;
			if ((errno != EAGAIN) && (newFD == -1))
			{
				perror("Accept");
				exit(1);
			}
		}
		datap = malloc(sizeof(slist_data_t));
		datap->threadNo = i++;
		datap->newFD = newFD;
		datap->thread_complete_flag = 0;
		SLIST_INSERT_HEAD(&head, datap, entries);
		if(pthread_create(&datap->threadNo, NULL, socket_thread, datap))
			printf("Failed to create thread");

		if ((terminate == 1) && (newFD <= 0))
			break;

		SLIST_FOREACH_SAFE(datap, &head, entries, datap_temp)
		{
			if (datap->thread_complete_flag == 1)
			{
				pthread_join(datap->threadNo, NULL);
				SLIST_REMOVE(&head, datap, slist_data_s, entries);
				free(datap);
			}
			
		}
		openlog("aesdsocket", LOG_CONS, LOG_USER);
		inet_ntop(AF_INET, &(their_addr), str, INET_ADDRSTRLEN);
		printf("Accepted connection from %s\n\r", str);
		syslog(LOG_DEBUG, "Accepted connection from %s", str);

	}
	pthread_mutex_destroy(&lock);
	close(socket_descriptor);
#ifdef USE_AESD_CHAR_DEVICE
	remove("/dev/aesdchar");
#else
	remove("/var/tmp/aesdsocketdata");
#endif
	printf("Graceful exit\n\r");
	exit (EXIT_SUCCESS);
}

// Reference: https://man7.org/tlpi/code/online/dist/sockets/read_line.c.html
int readLine(int fd, void *buffer, size_t n)
{
    int numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    static char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
			{
				return 0;
			}
            else                        /* Some bytes read; add '\0' */
			{
                return totRead;
			}

        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                return totRead;
        }
    }
}

static void signal_handler (int signo)
{
	terminate = 1;
	if (signo == SIGINT || signo == SIGTERM)
	{
		if(shutdown(socket_descriptor, SHUT_RDWR))
			perror("shutdown");
		syslog(LOG_DEBUG, "Caught signal exiting - %d", signo);
	}
		
}

void *socket_thread(void *arg)
{
	int read_bytes, write_bytes;
	slist_data_t *datat = (slist_data_t *)arg;
	int new_fd = datat->newFD;
	char *buff = NULL;
	const unsigned long LEN = 10;
	unsigned long bytes_received = 0;
	unsigned long cur_size = 10;
	int status = 0;
	int fdr, fdw;
	
	buff = malloc(cur_size);
	do
	{
		if (bytes_received >= cur_size)
		{
			char * tmp;
			cur_size += LEN;
			tmp = realloc(buff, cur_size);
			if (NULL == tmp)
			{
				fprintf(stderr, "realloc error\n\r");
				break;
			}

			buff = tmp;
		}

		status = recv(new_fd, buff + bytes_received, LEN, 0);
		if (status == 0)
		{
			printf("Nothing to receive\n\r");
		}
		else if (status > 0)
		{
			bytes_received += status;
			// printf("%d\n", status);           
		}
		else /* < 0 */
		{
			perror("recv");
			exit(1);
		}
	} while ((buff[bytes_received-1] != '\n') | (status == 0));
#ifdef USE_AESD_CHAR_DEVICE
	fdw = open ("/dev/aesdchar", O_RDWR | O_APPEND, 0644);
#else
	fdw = open ("/var/tmp/aesdsocketdata", O_RDWR | O_APPEND, 0644);
	pthread_mutex_lock(&lock);
#endif
	if (write (fdw, buff, bytes_received) < 0)
	{
		perror("write");
		exit(1);
	}
	close(fdw);
#ifndef USE_AESD_CHAR_DEVICE
	pthread_mutex_unlock(&lock);
#endif
	memset(buff, 0, bytes_received);
#ifdef USE_AESD_CHAR_DEVICE
	fdr = open ("/dev/aesdchar", O_RDWR, 0644);
#else
	fdr = open ("/var/tmp/aesdsocketdata", O_RDWR, 0644);
#endif
	while ((read_bytes = read(fdr, buff, bytes_received)) > 0)
	{
		if ((write_bytes = send(new_fd, buff, read_bytes, 0)) == -1)
		{
			perror("send");
			exit(1);
		}
	}
	close(fdr);
	if (read_bytes == -1)
	{
		perror("read");
		exit(1);
	}
	free(buff);	
	close(new_fd);
	datat->thread_complete_flag = 1;
	
	printf("Closed connection from %s\n\r", str);
	syslog(LOG_DEBUG, "Closed connection from %s", str);
	pthread_exit(NULL);
}


void *timer_signal (void *arg)
{
	char time_buff[50] = {};
	time_t rawtime;
	struct tm *tm;
	int fdw = open ("/var/tmp/aesdsocketdata", O_RDWR | O_APPEND, 0644);
	while(1)
	{
		time(&rawtime);
		tm = localtime(&rawtime);
		strftime(time_buff, 50, "timestamp:%a, %d %b %Y %T\n", tm);
		pthread_mutex_lock(&lock);
		write (fdw, time_buff, strlen(time_buff));
		pthread_mutex_unlock(&lock);
		memset(time_buff, 0, 50);
		sleep(10);
	}
}
