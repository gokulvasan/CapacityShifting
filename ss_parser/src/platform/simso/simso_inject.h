#ifndef __SIMSO_INJECT_H__
#define __SIMSO_INJECT_H__

#include <sys/socket.h>
#include <sys/un.h>

#define PATH_LEN 256
struct simso_inj {
	char path[256];			/// path for unix socket
	int socket;			/// socket id
	int client_id;			/// client id
	struct sockaddr_un addr;	/// soket association
	struct sockaddr_un client;	/// socket client
};

struct simso_intr {
	unsigned int id;		
	unsigned int start;
	unsigned int end;
	signed int sc;
	unsigned int job_count;
};

struct simso_tsk_len {
	unsigned int total_len;
	unsigned int intr_count;
};

struct simso_task {
	unsigned int id;
	unsigned int est;
	unsigned int wcet;
	unsigned int dl;
	unsigned int period;
	unsigned int intr_count;
	unsigned int intr[1];
};

struct header {
	int intr_count;
	int tsk_count;
};

#endif
