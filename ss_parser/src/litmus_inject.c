#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "ss_data.h"
#include "litmus.h"
/*************************************************************
 *               LOW LEVEL SS_INJECT FUNCTIONS               *
 *************************************************************/
#ifndef SYS_HOST
/* XXX: Why is ss_create_reservation a "long", while the others are int? */
static int litmus_create_reservation(struct res_data *rtype, void *config)
{ return syscall(__NR_reservation_create, rtype, config); }

static int litmus_inj_os_interval(struct res_data *res_type, void *inj)
{ return syscall(__NR_slot_shift_add_interval, res_type, inj); }

static int litmus_inj_os_task_table(struct td_descriptor *place_holder, struct ss_task *inj)
{ return syscall(__NR_set_task_table_data, place_holder, inj); }

static int litmus_wait_ts_release(void)
{ return syscall(__NR_wait_ts_release); }

static int litmus_release_ts(lt_t *delay)
{ return syscall(__NR_release_ts, delay); }

#else
/* Dummy functions for debugging */
static int litmus_create_reservation(struct res_data *rtype, void *config)
{ return 0; }
static int litmus_inj_os_interval(struct res_data *res_type, void *inj)
{ return 0; }
static int litmus_inj_os_task_table(struct td_descriptor *place_holder, struct ss_task *inj)
{ return 0; }
static int litmus_wait_ts_release(void)
{ sleep(10); return 0; }
static int litmus_release_ts(lt_t *delay)
{ return 1; }
#endif
static struct litmus_res_data res_data;

int litmus_exit(struct platform *plt) {
	exit(1);
}

/*************************************************************
 *                          SS_INJECT                        *
 *************************************************************/

/// ========================TASK INJ ==================================
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

static int ss_prepare_binary(struct task *task,
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
#ifndef SYS_HOST
	int i;
#endif
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

static int litmus_inject_table( struct task *task,
			struct hp_header *header,
			int type, pid_t pid)
{
	struct td_descriptor tmp;

	tmp.pid = pid;
	tmp.type = type;
	printf("TYPE: %d\n", tmp.type);
	tmp.size = task->ss_tsk_len;

	task->pid = pid;
	if(litmus_inj_os_task_table(&tmp, task->ss_tsk)) {
		return -1;
	}
	return 0;
}

static int litmus_tsk_inj(struct platform *p, 
			 struct task *task,
			struct hp_header *header,
			int type) 
{
	pid_t pid;
	char *param[NO_OF_PARAM_SIM];
	long sys_ret;
	struct litmus_res_data *data =(struct litmus_res_data *)p->data;

	ss_prepare_binary(task, header, type, 
			&param, data->reservation_id);
	
	pid = fork();
	
	if(0 == pid) {
		long sys_ret;
		sys_ret = litmus_wait_ts_release();
		if(0 != sys_ret) {
			return -1;
		}
		execvp(param[0], param);
		perror("execvp failure\n");
		exit(1);
	} else {
		if(-1 == pid) {
			return -1;
		}
		sys_ret = litmus_inject_table(task,
					header, type, pid);
		if(sys_ret < 0) {
			perror("task table injection failure\n");
			exit(1);
		}
		return 0;	
	}
}

/// ========================== RESERVATION CREATE ==========================
/// ========================== LITMUS SPECIFIC    ==========================
/// platform configure
static int litmus_res_create(struct platform *plt,
		enum task_type task, struct hp_header *header)
{
	long ret;
	struct res_data type;
	struct litmus_res_data *data =(struct litmus_res_data *)plt->data;
	if(!header)
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
	type.id = data->reservation_id;
	type.param_len = sizeof(config);

	config.priority = 0; //LITMUS_HIGHEST_PRIORITY;
	config.cpu = 0;
	config.cycle_length = data->res_len;
	config.slot_quantum = header->slot_quantum;

	/// sys call to kernel
	ret = litmus_create_reservation(&type, &config);
	if(ret < 0)
	{
		fprintf(stderr,
		"Error: %s:litmus_create_reservation(ret: %d)\n",
		__FUNCTION__, errno);
		return -1;
	}
	return 0;
}

static int litmus_parse_data(struct platform *plt,
			enum task_type type,
			struct hp_header *header,
			void *plat_data) {
	lt_t res_len;
	struct litmus_res_data	*data =(struct litmus_res_data*)plt->data;

	if(TYPE_PERIODIC == type) {
		if(!header->slot_quantum) {
			fprintf(stderr, "ERROR:%s: Slot quantum is 0\n",
			 	__FUNCTION__);
			return -1;
		}
	}
	res_len = header->slot_count * header->slot_quantum;
	if(res_len <= 0) {
		fprintf(stderr, "Error %s: reservation len should be <0\n",
			__FUNCTION__);
		return -1;
	}
	data->res_len = res_len;
	data->cpu = 0;
	data->reservation_id = 1234;
	
	return 0;
}

///====================================== RUN  =============================
static int litmus_release_tasks(struct platform *plt)
{
	lt_t delay = 0;
	return (litmus_release_ts(&delay)) ;
}
/// run
static int litmus_wait_till_release(struct platform *plt)
{
	int sleep_cnt = 0;
	int count = 0;

	#define SLEEP_TIME 5
	while(!(count = litmus_release_tasks(plt))
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

///====================== INTERVAL ===============================
static int litmus_inj_interval(struct platform *p, int len, 
			struct ss_intr_inj *intr) 
{
	struct res_data type;
	struct litmus_res_data *data = (struct litmus_res_data*)p->data;

	type.type = SLOT_SHIFTING;
	type.id = data->reservation_id;
	type.param_len = len;

	if( litmus_inj_os_interval(&type, intr) < 0)
		return -1;

	return 0;
}

struct platform* platform_init() 
{
	struct platform *p;
	p = malloc(sizeof(*p));
	if(!p)
		return NULL;

	p->configure = litmus_res_create;
	p->parse_data = litmus_parse_data;
	p->intr_inj = litmus_inj_interval; 
	p->tsk_inj =  litmus_tsk_inj;
	p->run = litmus_wait_till_release;
	p->exit = litmus_exit;
	p->data = &res_data;
	
	return p;
}

