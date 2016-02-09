#include "test_inject.h"

#define NUM_TEST_TYPES 7

typedef void (*check_value_t) (struct test_inject_data *data);

/* We need these two types -- which are not visible in ss_data.h -- to be able
 * to test ss_inject.
 * XXX: Should they not be visible? */
struct res_data {
	int type;
	int id;
	int param_len;
};

struct ss_res_config {
	long priority;
	int cpu;
	lt_t slot_quantum;
	lt_t cycle_length;
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


/* Test 1: checks that inj->set_res_id() and get_res_id() are working */
static void check_res_id(struct test_inject_data *data)
{
	int const_val = 10;
	data->inj->set_res_id(data->inj, const_val);
	ASSERT_EQUAL(data->inj->get_res_id(data->inj), const_val);
}

/* Test 2: checks whether res_create() is "NULL proof" */
static int create_reservation_fail(struct res_data *rtype, void *config)
{ ASSERT_FAIL(); return 0; }

static struct hp_header hp_header_zero = {0, 0, 0, 0, 0};

static void check_res_create_null_proof(struct test_inject_data *data)
{
	data->inj->os->create_reservation = create_reservation_fail;

	data->inj->res_create(NULL, 0, 0, 0, &hp_header_zero);
	data->inj->res_create(data->inj, 0, 0, 0, NULL);
}

/* Test 3: checks whether the right values are passed to create_reservation() */
static struct hp_header hp_header_normal = {3, 4, 3, 10, 1};
static int res_id_normal = 100;
static int cpu_normal = 1;
static int res_len_normal = 10;

static int create_reservation_normal(struct res_data *rtype, void *_config)
{
	struct ss_res_config *config = _config;
	ASSERT_NOT_NULL(rtype);
	ASSERT_NOT_NULL(config);

	ASSERT_EQUAL(rtype->type, SLOT_SHIFTING);
	ASSERT_EQUAL(rtype->id, res_id_normal);
	ASSERT_EQUAL(rtype->param_len, sizeof(struct ss_res_config));

	ASSERT_EQUAL(config->priority, 0);
	ASSERT_EQUAL(config->cpu, cpu_normal);
	ASSERT_EQUAL(config->cycle_length, res_len_normal);
	ASSERT_EQUAL(config->slot_quantum, hp_header_normal.slot_quantum);

	return 0;
}

static void check_res_create_normal(struct test_inject_data *data)
{
	data->inj->set_res_id(data->inj, res_id_normal);
	data->inj->os->create_reservation = create_reservation_normal;

	data->inj->res_create(data->inj, res_len_normal,
					cpu_normal, 0, &hp_header_normal);
}

/* Test 4: checks whether inj_interval() is "NULL proof" */
static int inj_interval_fail(struct res_data *rtype, void *config)
{ ASSERT_FAIL(); return 0; }

static struct ss_intr_inj intr_zero = { {0, 0, 0, 0, 0, 0} , {{0, 0, 0}} };

static void check_inj_interval_null_proof(struct test_inject_data *data)
{
	data->inj->os->inj_interval = inj_interval_fail;

	data->inj->inj_interval(data->inj, 0, NULL);
	data->inj->inj_interval(NULL, 0, &intr_zero);
}

/* Test 5: checks whether the right values are passed to os->inj_interval() */
static struct ss_intr_inj intr_normal = { {2, 10, 3, 10, 2, 1},
					  {{0, 3, 100}} };

static int inj_interval_normal(struct res_data *res_type, void *_intr)
{
	struct ss_intr_inj *intr = _intr;
	ASSERT_NOT_NULL(res_type);
	ASSERT_NOT_NULL(intr);

	ASSERT_EQUAL(res_type->type, SLOT_SHIFTING);
	ASSERT_EQUAL(res_type->id, res_id_normal);
	ASSERT_EQUAL(res_type->param_len, res_len_normal);

	ASSERT_EQUAL(intr->intr.intr_id, intr_normal.intr.intr_id);
	ASSERT_EQUAL(intr->intr.core, intr_normal.intr.core);
	ASSERT_EQUAL(intr->intr.start, intr_normal.intr.start);
	ASSERT_EQUAL(intr->intr.end, intr_normal.intr.end);
	ASSERT_EQUAL(intr->intr.sc, intr_normal.intr.sc);
	ASSERT_EQUAL(intr->intr.no_of_tsk, intr_normal.intr.no_of_tsk);

	ASSERT_EQUAL(intr->tsks[0].major_id, intr_normal.tsks[0].major_id);
	ASSERT_EQUAL(intr->tsks[0].minor_id, intr_normal.tsks[0].minor_id);
	ASSERT_EQUAL(intr->tsks[0].pid, intr_normal.tsks[0].pid);

	return 0;
}

static void check_inj_interval_normal(struct test_inject_data *data)
{
	data->inj->set_res_id(data->inj, res_id_normal);
	data->inj->os->inj_interval = inj_interval_normal;

	data->inj->inj_interval(data->inj, res_len_normal,
					&intr_normal);
}

/* Test 6: checks whether inj_per_task() is NULL proof */
static int ss_prepare_binary_fail (struct task *task,
				struct hp_header *header,
				int type,
				char *(*param)[] )
{ ASSERT_FAIL(); return 0; }

static struct task task_zero = {0, 0, "", 0, NULL};
static enum task_type type_periodic = TYPE_PERIODIC;

static void check_inj_per_task_null_proof(struct test_inject_data *data)
{
	data->inj->os->prepare_binary = ss_prepare_binary_fail;

	data->inj->inj_per_task(NULL, &task_zero,
				&hp_header_normal, type_periodic);
	data->inj->inj_per_task(data->inj, NULL,
				&hp_header_normal, type_periodic);
	data->inj->inj_per_task(data->inj, &task_zero,
				NULL, type_periodic);
	//data->inj->inj_per_task(data->inj, &task_zero,
	//			&hp_header_normal, TYPE_END);
}


/* Test 7: Checks whether the right values are passed to os->prepare_binary() */
struct ss_task ss_task_zero = {0, 0, {0}};
struct task task_normal = {100, 10, "./test/test", 20, &ss_task_zero};

static pid_t ss_fork_zero()
{ return 0; }
static pid_t ss_fork_negative()
{ return -1; }
static pid_t ss_fork_positive()
{ return 1; }

static int execvp_none (const char *file, char *const argv[])
{ return 0; }
static void exit_none (int status)
{ return; }
static int wait_ts_release_none (void)
{ return 0; }

static int ss_prepare_binary_normal (struct task *task,
				struct hp_header *header,
				int type,
				char *(*param)[] )
{
	ASSERT_NOT_NULL(task);
	ASSERT_NOT_NULL(header);
	ASSERT_NOT_NULL(param);

	/* XXX: The casts are there only to avoid warnings... */

	ASSERT_EQUAL(task->pid, task_normal.pid);
	ASSERT_EQUAL(task->period, task_normal.period);
	ASSERT_STR((char*)task->bin, (char*)task_normal.bin);
	ASSERT_EQUAL(task->ss_tsk_len, task_normal.ss_tsk_len);
	ASSERT_EQUAL((long)task->ss_tsk, (long)task_normal.ss_tsk);

	ASSERT_EQUAL(header->job_count, hp_header_normal.job_count);
	ASSERT_EQUAL(header->int_count, hp_header_normal.int_count);
	ASSERT_EQUAL(header->intr_job_asc, hp_header_normal.intr_job_asc);
	ASSERT_EQUAL(header->slot_count, hp_header_normal.slot_count);
	ASSERT_EQUAL(header->slot_quantum, hp_header_normal.slot_quantum);

	ASSERT_EQUAL(type, type_periodic);

	/* At least for now, we don't have any requirement about how `param`
	 * should be, since is expected to be modified by this function. */
	return 0;
}

static int ss_inj_os_task_table_fail (struct td_descriptor *desc,
				struct ss_task *t)
{ ASSERT_FAIL(); return 0; }

static int ss_inj_os_task_table_normal (struct td_descriptor *desc,
				struct ss_task *t)
{
	ASSERT_EQUAL(desc->pid, ss_fork_positive());
	ASSERT_EQUAL(desc->type, type_periodic);
	ASSERT_EQUAL(desc->size, task_normal.ss_tsk_len);

	ASSERT_EQUAL((long)t, (long)&ss_task_zero);

	return 0;
}

static void check_inj_per_task_normal(struct test_inject_data *data)
{
	data->inj->os->execvp = execvp_none;
	data->inj->os->exit = exit_none;
	data->inj->os->wait_ts_release = wait_ts_release_none;

	/* First part: fork returns a 0. We "are a child". Therefore,
	 * inj_task_table() should never be called. We set its pointer to a
	 * function that causes the test to fail. */
	data->inj->os->prepare_binary = ss_prepare_binary_normal;
	data->inj->os->inj_task_table = ss_inj_os_task_table_fail;
	data->inj->os->fork = ss_fork_zero;

	CTEST_LOG("First part passed");

	data->inj->inj_per_task(data->inj, &task_normal,
				&hp_header_normal, type_periodic);

	/* Second part: fork returns a -1. It means an error occurred. We keep
	 * the pointer to inj_task_table() to a negative value, since it should
	 * also never be called. */
	data->inj->os->fork = ss_fork_negative;

	data->inj->inj_per_task(data->inj, &task_normal,
				&hp_header_normal, type_periodic);

	CTEST_LOG("Second part passed");

	/* Third part: fork return a 1. This means success. inj_task_table()
	 * must be called, and we need to check the values that were passed to
	 * it. */
	data->inj->os->inj_task_table = ss_inj_os_task_table_normal;
	data->inj->os->fork = ss_fork_positive;

	data->inj->inj_per_task(data->inj, &task_normal,
				&hp_header_normal, type_periodic);

	CTEST_LOG("Third part passed");
}



/* Basic infrastructure */

static check_value_t check_value[NUM_TEST_TYPES] = {
	check_res_id,
	check_res_create_null_proof,
	check_res_create_normal,
	check_inj_interval_null_proof,
	check_inj_interval_normal,
	check_inj_per_task_null_proof,
	check_inj_per_task_normal,
};

static void sanity_checks(unsigned int check)
{
	if (check >= NUM_TEST_TYPES) {
		CTEST_LOG("WARNING: argument check out of bounds");
		ASSERT_FAIL();
	}
}

void check_test_inject(struct test_inject_data *data, unsigned int check)
{
	sanity_checks(check);

	check_value[check](data);
}


