#ifndef __SS_INJ_DATA_H
#define __SS_INJ_DATA_H

/*
 * 4. this is used for associating the task with interval
 */
struct job_bond {
	int intr_id; /// interval to which this job belongs
	unsigned int est; /// est of the task
	unsigned int exec_count; /// keeping a count of how much is completed.
};

/*
 * 3.
 */

struct minor_id_bond {
	unsigned int minor_id; ///job id of the task.
	unsigned int est; /// early start time TODO: remove This
	unsigned int wcet; /// Worst case execution time
	unsigned int dl; /// deadline
	unsigned int coreid; /// core
	int intr_count; ///no of intervals to which this job belongs
	struct job_bond intr_bond[1]; ///list of interval offsets
};

/* 
 * 2.
 */
struct ss_task{
	unsigned int major_id; ///task id defined by offline scheduler
	int minor_id_count; ///Total count of the jobs that belongs to this tasks
	long minor_id_bonds[1]; ///lists offsets of minor ids.
};

/****************************** INTERVAL DATA ***********************************/

/*Table parsed Data */

struct ss_intr {
	unsigned int intr_id; /// interval_id
	unsigned int core; /// core to which interval belongs
	unsigned int start; /// start of this interval
	unsigned int end; /// end of this interval
	signed int sc; /// spare capacity
	unsigned int no_of_tsk; /// number of tasks
};

struct ss_intr_task {
	int major_id;
	int minor_id;
	int pid;
};

struct ss_intr_inj {
	struct ss_intr intr;
	struct ss_intr_task tsks[1];		
};

#endif
