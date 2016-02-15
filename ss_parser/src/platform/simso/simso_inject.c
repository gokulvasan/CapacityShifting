#include "ss_data.h"
#include "simso_inject.h"

static struct simso_inj  simso_data;

static int simso_parse_data(struct platform* self, enum task_type type, 
			struct hp_header *header, void *data) 
{
	struct simso_inj *sdata = self->data;
	
	strcpy(sdata->path, "/tmp/ss_parser");
	return 0;
}

static int simso_configure(struct platform* self, enum task_type type,
			struct hp_header *header)
{
	struct simso_inj *data = self->data;
	socklen_t len;
	struct header sock_header;

	data->socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(data->socket < 0) {
		perror("socket");
		return -1;
	}
	strcpy(data->addr.sun_path, data->path);
	data->addr.sun_family = AF_UNIX;
	if(bind(data->socket, (struct sockaddr*)&data->addr,
	strlen(data->addr.sun_path)+sizeof(data->addr.sun_family))) {
		perror("BIND");
		return -1;
	}
	listen(data->socket, 1);
	
	len = sizeof(data->client);
	data->client_id = accept(data->socket,0,0);	
	if(-1 == data->client_id) {
		perror("client");
		return -1;
	}
	printf("Client is accepted\n");
	sock_header.intr_count = header->int_count;
	sock_header.tsk_count = header->job_count;
	len =  sizeof(sock_header);
	send(data->client_id, &sock_header,len, 0);
	recv(data->client_id, &len, sizeof(len), 0);
	fprintf(stdout, "server has written a val %d\n", len);

	return 0;
}

static int simso_run(struct platform* self) 
{
	return 0;
}

static int simso_exit(struct platform *self, int exit_code)
{
	struct simso_inj *d = self->data;
	printf("unlinking %s\n", d->path);
	unlink(d->path);
	if(exit_code != 0) {
		perror("platform fails");
		exit(exit_code);
	}
	return 0;
}

static int simso_intr_inj(struct platform *self, int len, 
			struct ss_intr_inj *intr)
{
	struct simso_inj *d = self->data; 
	struct simso_intr inj_intr;
	int rlen;
	
	inj_intr.id = intr->intr.intr_id;
	inj_intr.start = intr->intr.start;
	inj_intr.end = intr->intr.end;
	inj_intr.sc = intr->intr.sc;
	inj_intr.job_count = intr->intr.no_of_tsk;

	send(d->client_id, &inj_intr, sizeof(inj_intr), 0);
	recv(d->client_id, &rlen, sizeof(rlen), 0);
	printf("interval len sent:%d\n", rlen);		
	return 0;
}

static struct minor_id_bond* get_minor_id(struct ss_task *major_id)
{
	return (struct minor_id_bond*)OFFSET(major_id, 
					major_id->minor_id_bonds[0]);
}


static int simso_tsk_inj(struct platform *self, struct task *task,
			struct hp_header *hp, int type)
{
	struct ss_task *tsk = task->ss_tsk;
	struct minor_id_bond *m;
	struct simso_task *i_tsk;
	struct simso_tsk_len ilen;
	struct simso_inj *data = self->data;
	int i, hlen;
	
	if(tsk->minor_id_count > 1) {
		perror("SIMSO does not support multiple minor id\n");
		self->exit(self, 1);
	}
	m = get_minor_id(tsk);
	ilen.intr_count  = m->intr_count;
	ilen.total_len  = sizeof(*i_tsk) + 
		((ilen.intr_count -1) * sizeof(i_tsk->intr));
	/// This is a variable data, so it first 
	/// needs to send length then payload,
	if(send(data->client_id, &ilen, sizeof(ilen), 0) < 0) {
		perror("sending length failed");
		self->exit(self, 1);
	}
	recv(data->client_id, &hlen, sizeof(hlen), 0);

	i_tsk = malloc(ilen.total_len);
	i_tsk->id = tsk->major_id;
	i_tsk->est = m->intr_bond[0].est;
	i_tsk->wcet = m->wcet;
	i_tsk->dl = m->dl;
	i_tsk->period = m->intr_bond[1].est - m->intr_bond[0].est;
	i_tsk->intr_count = ilen.intr_count;
	for(i =0; i < m->intr_count; i++) {
		i_tsk->intr[i] = m->intr_bond[i].intr_id;
	}
	send(data->client_id, i_tsk, ilen.total_len, 0);
	recv(data->client_id, &hlen, sizeof(hlen), 0);
	free(i_tsk);
	printf("**************len:%d\n", ilen.total_len);
	return 0;
}

struct platform* platform_init() 
{
	struct platform *p;
	p = malloc(sizeof(*p));
	if(!p)
		return NULL;

	p->configure = simso_configure;
	p->parse_data = simso_parse_data;
	p->intr_inj = simso_intr_inj;
	p->tsk_inj = simso_tsk_inj;
	p->run = simso_run;
	p->exit = simso_exit;
	p->data = &simso_data;

	return p;
}
