#ifndef __SS_LITMUS_H_
#define __SS_LITMUS_H_

/// syscall numbers hardcoded 
#define __NR_reservation_create 363
#define __NR_reservation_destroy 364
#define __NR_wait_ts_release 360
#define __NR_release_ts 361
#define __NR_set_task_table_data 365
#define __NR_slot_shift_add_interval 367

struct litmus_res_data {
	int reservation_id;
	int cpu;
	lt_t res_len;	
};

typedef enum {
        PERIODIC_POLLING,
        SPORADIC_POLLING,
        TABLE_DRIVEN,
	SLOT_SHIFTING
} reservation_type_t;

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

#endif
