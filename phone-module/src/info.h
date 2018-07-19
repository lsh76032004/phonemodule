#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>

#define TCP_BUFFER	1024
#define IPADDR_SIZE	32

enum callback_condition {
	RECEIVE_DATA = 0,
	SEND_DATA,
	START_MONITOR,
	STOP_MONITOR,
	MAX_CALLBACK_CONDITION,
};

struct info_t {
	char server_name[IPADDR_SIZE];
	int server_port;

	struct sockaddr_in server_address;
	int sock;

	pthread_t tid;
	bool is_run;
	bool stop;

	char receive_msg[TCP_BUFFER];
	char send_msg[TCP_BUFFER];

	int (*send)(struct info_t *info, const char *msg);

	struct tcp_callback_list *callback_list[MAX_CALLBACK_CONDITION];
};
