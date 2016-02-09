#ifndef SS_DATA_H
#define SS_DATA_H

#include "ss_parser.conf"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/syscall.h>

#include "ss_inj_data.h"

#define OFFSET(absl_addr, rel_addr) ((void*)((long)absl_addr) + ((long)rel_addr))

typedef unsigned long long lt_t;

/*
 *	name: fsm_task_type
 *	type: enum
 *		used for switching between
 *		different class of methods.
 */
enum task_type {
	TYPE_PERIODIC = 0,
	TYPE_FIRM_APERIODIC,
	TYPE_SOFT_APERIODIC,
	TYPE_END
};


/*Generic Container to handle disjoint data */
struct ss_container {
	void* data; /// generic data
	struct ss_container* nxt; /// pointer to nxt
};



/*
 * 1. this is first level
 */
struct task {
	unsigned int pid; ///pid from fork.
	unsigned int period; /// TODO: yet to be done, lets not use this for now
	unsigned char bin[PATH_MAX]; /// binary itself
	int ss_tsk_len; /// holds length of the ss_tsk used for injection process.
	struct ss_task *ss_tsk;	 /// injectable data of task.
};



/*Interval Specific Data */


/*************************************************************/
/* very specific to slot shifting  aligning jobs and tasks*/

/*
 * TODO: There is a need for seperate table that would hold task details...
	task details:
		1. binary,
		2. count of jobs
		3. period
 */
struct ss_intr_bond {
	unsigned int intr_id;
	unsigned int est;
};
struct ss_job {
	unsigned int jid; /// minor id from offline schedule
	unsigned int major_id; /// task to which this job is associated
	unsigned int est; /// early start time, TODO: remove this
	unsigned int wcet; /// Worst case execution time
	unsigned int dl; /// deadline
	unsigned int coreid; /// core
	unsigned int interval_id; /// interval, TODO:this should be removed
	unsigned int intr_count; /// number of intervals to which this job belongs.
	char binary[PATH_MAX]; /// binary, every job would hold a binary that is same as task.
				/// TODO: change this redundancy.
	struct ss_container  *intr; /// list of interval ids
};

struct hp_header {
	unsigned int job_count;
	unsigned int int_count;
	unsigned int intr_job_asc;
	unsigned int slot_count;
	unsigned int slot_quantum;
};


/*This could be made into a array of ss_container pointer */
struct ss_data {
	struct ss_container *interval;
	struct ss_container *task;
	struct ss_container *res_data;
};

struct ss_data* parsing_main(char *offline_schedule_path, enum task_type type);


/*************************************************************
 *               LOW LEVEL SS_INJECT FUNCTIONS               *
 *************************************************************/
/* It would be better not to leave these declarations visible, but it seems it
 * will be needed, so that others can implement their own versions of these
 * functions. */
struct ss_inject_os;

/** Initializes a ss_inject_os structure.
 *
 * Expects `inj` to be already allocated. If it is NULL, the function returns
 * without doing anything.
 * \param inj An *already allocated* ss_inject_os to be initialized. */
void ss_inject_os_init(struct ss_inject_os *inj);

/* "Destroys" a ss_inject_os structure.
 *
 * Supposes everything inside it is allocated. Still, will check for NULL before
 * trying to deallocate anything.
 * \param inj The ss_inject_os object to destroyed. */
void ss_inject_os_del(struct ss_inject_os *inj);


/* No need to know the definitions of these structs. */
struct td_descriptor;
struct res_data;

/* TODO: Should we make these typedefs more generic? Something like
 * transforming them "call_syscall_t"? */
typedef int (*ss_inj_os_create_reservation_t) (
	//struct ss_inject_os,
	struct res_data *rtype,
	void *config
);

typedef int (*ss_inj_os_interval_t) (
	//struct ss_inject_os,
	struct res_data *res_type,
	void *inj
);

typedef int (*ss_inj_os_task_table_t) (
	//struct ss_inject_os,
	struct td_descriptor *place_holder,
	struct ss_task *inj
);

typedef int (*ss_inj_os_wait_ts_release_t) (void);

typedef int (*ss_inj_os_release_ts_t) (
	//struct ss_inject_os,
	lt_t *delay
);

typedef int (*ss_inj_os_prepare_binary_t) (
	//struct ss_inject_os,
	struct task *task,
	struct hp_header *header,
	int type,
	char *(*param)[],
	int reservation_id
);

typedef pid_t (*ss_inj_os_fork_t) (void);

typedef int (*ss_inj_os_execvp_t) (
	const char *file,
	char *const argv[]
);

typedef void (*ss_inj_os_exit_t) (
	int status
);

struct ss_inject_os {
	ss_inj_os_create_reservation_t	create_reservation;
	ss_inj_os_interval_t		inj_interval;
	ss_inj_os_task_table_t		inj_task_table;
	ss_inj_os_wait_ts_release_t	wait_ts_release;
	ss_inj_os_release_ts_t		release_ts;
	ss_inj_os_prepare_binary_t	prepare_binary;

	/* System calls */
	ss_inj_os_fork_t		fork;
	ss_inj_os_execvp_t		execvp;
	ss_inj_os_exit_t		exit;
};


/*************************************************************
 *                          SS_INJECT                        *
 *************************************************************/
struct ss_inject;

/* Initializes a ss_inject structure.
 *
 * Expects `inj` to be already allocated. If it is NULL, the function returns
 * without doing anything. The function will suppose `inj->os` is not allocated
 * and initialize it.
 * \param inj An *already allocated* ss_inject to be initialized. */
void ss_inject_init(struct ss_inject *inj);

/* "Destroys" a ss_inject structure.
 *
 * Supposes everything inside it is allocated. Still, will check for NULL before
 * trying to deallocate anything.
 * \param inj The ss_inject object to destroyed. */
void ss_inject_del(struct ss_inject *inj);

typedef int (*ss_inj_res_create_t) (
	struct ss_inject *inj,
	lt_t res_len,
	int cpu,
	enum task_type task,
	struct hp_header *header
);

typedef int (*ss_inj_interval_t) (
	struct ss_inject *inj,
	int len,
	struct ss_intr_inj *intr
);

typedef int (*ss_inj_per_task_t) (
	struct ss_inject *inj,
	struct task *task,
	struct hp_header *header,
	int type
);

typedef int (*ss_inj_wait_till_release_t) (struct ss_inject *inj);

//typedef int inject_table(
//	struct ss_inject *inj,
//	struct task *task,
//	struct hp_header *header,
//	int type, pid_t pid
//);

typedef void (*ss_inj_set_res_id_t) (
	struct ss_inject *inj,
	int res_id
);

typedef int (*ss_inj_get_res_id_t)(struct ss_inject *inj);


struct ss_inject {
	ss_inj_res_create_t		res_create;
	ss_inj_interval_t		inj_interval;
	ss_inj_per_task_t		inj_per_task;
	ss_inj_wait_till_release_t	wait_till_release;

	ss_inj_set_res_id_t		set_res_id;
	ss_inj_get_res_id_t		get_res_id;
	int				reservation_id;

	struct ss_inject_os		*os;
};



/* We don't need these declarations here anymore */
/* Default functions for struct ss_inject */
void ss_inj_update_res_id(struct ss_inject *inj, int res_id);

int ss_inj_res_create(struct ss_inject *inj, lt_t res_len, int cpu,
			enum task_type task, struct hp_header *header);

//int ss_inj_release_tasks();
int ss_inj_interval(struct ss_inject *inj, int len, struct ss_intr_inj *intr);

int ss_inj_per_task(struct ss_inject *inj, struct task *task,
			struct hp_header *header, int type);
int ss_inj_wait_till_release(struct ss_inject *inj);



/*************************************************************
 *                        SS_TSAHEYLU                        *
 *************************************************************/

struct ss_tsaheylu;

/* Initializes a ss_tsaheylu structure.
 *
 * Expects `tsa` to be already allocated. If it is NULL, the function returns
 * without doing anything.
 *
 * The function will check whether `inj` points to something already or not. If
 * it doesn't, it will allocate memory for `inj` and initialize it. Else, it
 * will only point to it.
 *
 * \param tsa An *already allocated* ss_inject to be initialized.
 * \param inj A ss_inject structure pointer that will be stored in `tsa`. */
void ss_tsaheylu_init(struct ss_tsaheylu *tsa, struct ss_inject *inj);

/* "Destroys" a ss_tsaheylu structure.
 *
 * Supposes everything inside it is allocated. Still, will check for NULL before
 * trying to deallocate anything.
 * \param inj The ss_tsaheylu object to destroyed.
void ss_tsaheylu_del(struct ss_inject *inj);
 */



typedef int (*ss_tsa_job_to_task_t) (
	struct ss_tsaheylu *tsa,
	struct ss_container **jobs,
	struct hp_header *rest_data ,
	int type
);

typedef int (*ss_tsa_intr_task_bond_t) (
	struct ss_tsaheylu *tsa,
	struct ss_container *tsk,
	struct ss_container *intr
);

struct ss_tsaheylu {
	ss_tsa_job_to_task_t	job_to_task;
	ss_tsa_intr_task_bond_t	intr_task_bond;

	struct ss_inject	*inj;
};



/* We don't need these declarations anymore here */

/* Default functions for struct ss_tsaheylu */

/********************************************************
* Function Name: ss_job_to_task
* 	@jobs: pointer to list of jobs
*  @rest_data: pointer to hyper period data
*  Return val: 0: success / -ve on failure
*
*Description : Converts jobs to task,
*		i.e. struct ss_job -> struct task
*
*********************************************************/
int ss_tsa_job_to_task(
		struct ss_tsaheylu *tsa,
		struct ss_container **jobs,
		struct hp_header *rest_data ,
		int type);


/* ****************************************************************
 * function name: ss_intr_task_bond
 *	@tsk	: list of tasks
 *	@intr	: list of intervals of type struct ss_int
 * return val	: 0: Success, -ve: failure
 * Description  : converts interval from struct ss_intr
 *		to struct ss_intr_inj.
 *		basically creates bonding between interval and task.
 *******************************************************************/
int ss_tsa_intr_task_bond(struct ss_tsaheylu *tsa,
		struct ss_container *tsk,
		struct ss_container *intr);


#endif /* SS_DATA_H */

