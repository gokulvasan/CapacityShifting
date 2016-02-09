#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "ss_data.h"
#define __NR_reservation_create 363
#define __NR_reservation_destroy 364
#define __NR_wait_ts_release 360
#define __NR_release_ts 361
#define __NR_set_task_table_data 365
#define __NR_slot_shift_add_interval 367


/*************************************************************
 *               LOW LEVEL SS_INJECT FUNCTIONS               *
 *************************************************************/
struct td_descriptor {
	pid_t pid;
	size_t  type;
	size_t size;
};

struct res_data {
	int type;
	int id;
	int param_len;
};

#ifndef SYS_HOST
/* XXX: Why is ss_create_reservation a "long", while the others are int? */
static int ss_create_reservation(struct res_data *rtype, void *config)
{ return syscall(__NR_reservation_create, rtype, config); }

static int ss_inj_os_interval(struct res_data *res_type, void *inj)
{ return syscall(__NR_slot_shift_add_interval, res_type, inj); }

static int ss_inj_task_table(struct td_descriptor *place_holder, struct ss_task *inj)
{ return syscall(__NR_set_task_table_data, place_holder, inj); }

static int ss_wait_ts_release(void)
{ return syscall(__NR_wait_ts_release); }

static int ss_release_ts(lt_t *delay)
{ return syscall(__NR_release_ts, delay); }

#else
/* Useless functions for debugging */
static int ss_create_reservation(struct res_data *rtype, void *config)
{ return 0; }
static int ss_inj_os_interval(struct res_data *res_type, void *inj)
{ return 0; }
static int ss_inj_task_table(struct td_descriptor *place_holder, struct ss_task *inj)
{ return 0; }
static int ss_wait_ts_release(void)
{ sleep(10); return 0; }
static int ss_release_ts(lt_t *delay)
{ return 1; }
#endif

/* After refactoring, the definition of prepare_binary should come here (not
 * only its declaration. */
static int ss_prepare_binary(struct task *task,
			struct hp_header *header,
			int type,
			char *(*param)[],
			int reservation_id);

void ss_inject_os_init(struct ss_inject_os *i)
{
	if (!i)
		return;

	i->create_reservation = ss_create_reservation;
	i->inj_interval = ss_inj_os_interval;
	i->inj_task_table = ss_inj_task_table;
	i->wait_ts_release = ss_wait_ts_release;
	i->release_ts = ss_release_ts;
	i->prepare_binary = ss_prepare_binary;

	i->fork = fork;
	i->execvp = execvp;
	i->exit = exit;
}

/* Purposefully does nothing -- but is prepared to do something in the future */
void ss_inject_os_del(struct ss_inject_os *i)
{ return; }


/*************************************************************
 *                          SS_INJECT                        *
 *************************************************************/

void ss_inj_set_res_id(struct ss_inject *inj, int res_id)
{ inj->reservation_id = res_id; }

int ss_inj_get_res_id(struct ss_inject *inj)
{ return inj->reservation_id; }

void ss_inject_init(struct ss_inject *i)
{
	/* This function expects i to be already allocated */
	if (!i)
		return;

	i->res_create = ss_inj_res_create;
	i->inj_interval = ss_inj_interval;
	i->inj_per_task = ss_inj_per_task;
	i->wait_till_release = ss_inj_wait_till_release;
	//i->inject_table = ss_inject_table;

	i->set_res_id = ss_inj_set_res_id;
	i->get_res_id = ss_inj_get_res_id;

	i->set_res_id(i, 0);

	i->os = malloc(sizeof(*i->os));
	ss_inject_os_init(i->os);
}

void ss_inject_del(struct ss_inject *i)
{
	if (!i)
		return;
	ss_inject_os_del(i->os);
	free(i->os);
}

static struct minor_id_bond* get_minor_id(struct ss_task *major_id)
{
	struct minor_id_bond *t;
	t = (struct minor_id_bond*)OFFSET(major_id, major_id->minor_id_bonds[0]);
	return t;
}

enum ss_task_type {
	PERIODIC = 0,
	HARD_APERIODIC,
	SOFT_APERIODIC,
	TASK_UNKNOWN
};

const long k_tsk_type[TYPE_END] = {PERIODIC, HARD_APERIODIC, SOFT_APERIODIC};


#define PARAM_LEN 64

#ifdef SYS_HOST
	#define BIN_FOR_SIM "/bin/ls"
	#define BIN_LEN_SIM 8
	#define BIN "/bin/ls"
	#define BIN_LEN 8
#else
	#define BIN_FOR_SIM "/bin/rtspin"
	#define BIN_LEN_SIM 12
	#define BIN "/bin/rt_launch"
	#define BIN_LEN 14
#endif

#ifdef SYS_HOST
	#define NO_OF_PARAM_SIM 2
	#define NO_OF_PARAM 2
#elif DEBUG
	#define NO_OF_PARAM_SIM 13
	#define NO_OF_PARAM 11
#else
	#define NO_OF_PARAM_SIM 13
	#define NO_OF_PARAM 11
#endif

static char * const bin[2] = {BIN_FOR_SIM, BIN};
static const int bin_len[2] = {BIN_LEN_SIM, BIN_LEN};
static const int param_max[2] = {NO_OF_PARAM_SIM, NO_OF_PARAM};

static int  ss_prepare_binary(struct task *task,
			struct hp_header *header,
			int type,
			char *(*param)[],
			int reservation_id)
{
	struct minor_id_bond *job;
	lt_t period; // milliseconds
	lt_t wcet; // milliseconds
	lt_t duration; //seconds;
	lt_t slot_quantum;
	int index;
	int i;

	index = !task->bin[0]?0:1;

	(*param)[0] = bin[index];
	job = get_minor_id(task->ss_tsk);
	/* FIXME: needs check of second kind of job, i.e. minor id*/
	period = job->intr_bond[1].est - job->intr_bond[0].est;
	period = period; // * 5; //(header->slot_quantum);
	wcet = job->wcet; // * 5; //* (header->slot_quantum);
	duration = job->intr_count; //header->slot_count * 5;
	//duration = duration * .001;
	//(wcet * job->intr_count); //header->slot_count * (header->slot_quantum + 5);
	//duration = duration * header->slot_quantum;
	//duration = duration * 0.001;
	duration = 1500;
	slot_quantum = header->slot_quantum;

#ifdef DEBUG
	printf("%s:binary: %s ",
		 __FUNCTION__, (*param)[0]);
	printf("wcet: %lld slot ", wcet);
	printf("period: %lld slot ", period);
	printf("Duration: %lld job ", duration);
	printf("slot_quantum: %lld ms\n", slot_quantum);
#endif

#ifdef SYS_HOST
	(*param)[1] = NULL;
#else
	for(i=1; i< (param_max[index] - 1); i++) {
		(*param)[i] = malloc(PARAM_LEN);
	}
	i =1;
	sprintf((*param)[i++], "%s", "-p");
	sprintf((*param)[i++], "%d", 0);
	sprintf((*param)[i++], "%s", "-r");
	sprintf((*param)[i++], "%d", reservation_id);
	sprintf((*param)[i++], "%lld", wcet);
	sprintf((*param)[i++], "%lld", period);
	if(index) {
		sprintf((*param)[i++], "%s", task->bin);
	} else {
		sprintf((*param)[i++], "%lld", duration);
		sprintf((*param)[i++], "%lld", slot_quantum);
		memcpy(task->bin, (*param)[0] , bin_len[index]);
	}
#if 0
	if(TYPE_PERIODIC != type) {
		sprintf((*param)[i++], "%s", "t");
		sprintf((*param)[i++], "%d", 10);
	}
#endif

#ifdef DEBUG
	//sprintf((*param)[i++], "%s", "-v");
	(*param)[i++] = NULL;

	for (i = 0; i < param_max[index] && (*param)[i] != NULL; i++) {
		printf("param[%d]: %s\n", i, (*param)[i]);
	}
#else
	//sprintf((*param)[i++], "%s", "-w");
	(*param)[i++] = NULL;
#endif // DEBUG

#endif // SYS_HOST

	return 0;
}

static int ss_inject_table(struct ss_inject *inj,
			struct task *task,
			struct hp_header *header,
			int type, pid_t pid)
{
	struct td_descriptor tmp;

	tmp.pid = pid;
	tmp.type = type; //k_tsk_type[type];
	printf("TYPE: %lu\n", tmp.type);
	tmp.size = task->ss_tsk_len;

	task->pid = pid;
	if(inj->os->inj_task_table(&tmp, task->ss_tsk) ) {
		fprintf(stderr,
		"Error: %s: task(%d) table inject fail\n",
		 __FUNCTION__,pid);
		return -1;
	}
	return 0;
}

/*
	expected synchronous flow of aperiodic tasks in simulation environment.
	where    * : syscall.
	      ---- : blocked state.
	      ____ : running.

		sleep(1)          exec()     kernel_blocks
		__*----------------*_________----------------orphan--------
	fork()	|
		|
ss_parser_______|
		|
		|
		|  inj_table()    sleep(1)               ss_parser completes.
		---_*_____________*--------------______________*
*/

int ss_inj_per_task(
	struct ss_inject *inj,
	struct task *task,
	struct hp_header *header,
	int type)
{
	pid_t pid;
	int ret = -1;
	char *param[NO_OF_PARAM_SIM];
	long sys_ret;

	if (!inj || !task || !header)
		return ret;

	do {
		inj->os->prepare_binary(task, header, type, &param,
					inj->get_res_id(inj));
		pid = inj->os->fork();
		if(0 == pid) {
			//sleep(1);
		//	if(TYPE_PERIODIC == type) {
				sys_ret = inj->os->wait_ts_release();
				if(sys_ret != 0)
					fprintf(stderr,
					"Error: %s: Task released with error(%ld)\n",
					__FUNCTION__, sys_ret);
		//	} else {
		//		sleep(1);
		//	}
#ifdef DEBUG
			printf("We are Execing a child: %s\n", param[0]);
#endif
			inj->os->execvp(param[0], param);

			fprintf(stderr,
			"Error: %s:Execing child Failed: %d\n",
			__FUNCTION__, errno);

			inj->os->exit(1);
		} else {
			if(-1 == pid) {
				fprintf(stderr,
					"Error: %s:fork(Major_id:%d) failed\n",
					__FUNCTION__, task->ss_tsk->major_id);
				break;
			}
			if(ss_inject_table(inj, task, header, type, pid) < 0)
				break;
#ifdef DEBUG
			printf("task %d is injected successful\n",
			pid);
#endif
			//if(PERIODIC != type)
			//sleep(2);

			ret = 0;
		}
	} while(0);

	return ret;
}

typedef enum {
        PERIODIC_POLLING,
        SPORADIC_POLLING,
        TABLE_DRIVEN,
	SLOT_SHIFTING
} reservation_type_t;

int ss_inj_res_create(struct ss_inject *inj, lt_t res_len, int cpu,
			enum task_type task, struct hp_header *header)
{
	long ret;
	struct res_data type;

	if(!inj || !header)
		return -1;

	struct ss_res_config {
		long priority;
		int cpu;
		lt_t slot_quantum;
		lt_t cycle_length;
	} config;

	if(task != TYPE_PERIODIC)
		return 0;

	type.type = SLOT_SHIFTING; //slot shifting;
	type.id = inj->get_res_id(inj);
	type.param_len = sizeof(config);

	config.priority = 0; //LITMUS_HIGHEST_PRIORITY;
	config.cpu = cpu;
	config.cycle_length = res_len;
	config.slot_quantum = header->slot_quantum;

	ret = inj->os->create_reservation(&type, &config);
	if(ret < 0)
	{
		fprintf(stderr,
		"Error: %s: inj->os->create_reservation(ret: %d)\n",
		__FUNCTION__, errno);

		return -1;
	}

	return 0;
}

int ss_inj_interval(struct ss_inject *inj, int len, struct ss_intr_inj *intr)
{
	struct res_data type;
	int ret;

	if(!intr || !inj) {
		//fprintf(stderr, "ERROR\n");
		return -1;
	}
#if defined (DEBUG) && defined (SYS_HOST)
	return 0;
#endif
	type.type = SLOT_SHIFTING;
	type.id = inj->get_res_id(inj);
	type.param_len = len;

	ret = inj->os->inj_interval(&type, intr);
	if(ret < 0)
	{
		fprintf(stderr,
		"Error: %s():failed to inject\n",
		 __FUNCTION__);

		return -1;
	}
	return 0;
}

static int ss_inj_release_tasks(struct ss_inject *inj)
{
	lt_t delay = 0;
	return (inj->os->release_ts(&delay)) ;
}

int ss_inj_wait_till_release(struct ss_inject *inj)
{
	int sleep_cnt = 0;
	int count = 0;

	#define SLEEP_TIME 5
	while(!(count = ss_inj_release_tasks(inj))
			&& (sleep_cnt <= 3)) {

		fprintf(stdout,
		"Waiting: %s: released task count is 0, so again sleeping for %d\n",
		__FUNCTION__, SLEEP_TIME);
		sleep(SLEEP_TIME);
		sleep_cnt++;
	}
	if(!count) {
		fprintf(stderr, "Error: release cnt: %d\n", count);
		return -1;
	}
	printf("%d tasks relased\n", count);
	return -1;
}

