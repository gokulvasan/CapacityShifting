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
	unsigned int jid; 	/// minor id from offline schedule
	unsigned int major_id; 	/// task to which this job is associated
	unsigned int est; 	/// early start time, TODO: remove this
	unsigned int wcet; 	/// Worst case execution time
	unsigned int dl; 	/// deadline
	unsigned int coreid; 	/// core
	unsigned int interval_id; /// interval, TODO:this should be removed
	unsigned int intr_count;  /// number of intervals to which this 
				  /// job belongs.
	char binary[PATH_MAX]; 	  /// binary, every job would 
				  /// hold a binary that is same as task.
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

/*************************************************************
 *                          SS_INJECT                        *
 *************************************************************/
struct ss_inject;
struct platform;

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

typedef void (*ss_inj_set_res_id_t) (
	struct ss_inject *inj,
	int res_id );

typedef int (*ss_inj_get_res_id_t)(struct ss_inject *inj);

/*************************************************************
 *		PLATFORM SPECIFIC FUNCTORS
 ************************************************************/
typedef int (*platform_intr_inj)(struct platform *p, int len, 
					struct ss_intr_inj *intr);
typedef int (*platform_run)(struct platform*);

typedef int (*platform_exit)(struct platform*, int exit_code);
typedef int (*platform_parse_data)(struct platform*, enum task_type,
				struct hp_header *, void *data);
typedef int (*platform_configure)(struct platform*, enum task_type,
				struct hp_header*); 
typedef int (*platform_task_inj)(struct platform *p, struct task *tsk,
				struct hp_header *header, int type);
struct platform {
	platform_parse_data parse_data;
	platform_configure configure;
	platform_run run;
	platform_exit exit;
	platform_intr_inj intr_inj;
	platform_task_inj tsk_inj;
	void *data;
};

struct ss_inject {
	ss_inj_interval_t		inj_interval;		
	ss_inj_per_task_t		inj_per_task;
	struct platform			*plat;			
};

struct platform* platform_init(); 

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
int ss_tsaheylu_init(struct ss_tsaheylu *tsa, struct ss_inject *inj);

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

#endif /* SS_DATA_H */

