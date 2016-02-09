#include "test_tsaheylu.h"

#define NUM_TEST_TYPES 2


typedef void (*check_value_t) (struct test_tsaheylu_data *data);

/* Test 1: checks whether job_to_task() is "NULL proof" */
struct ss_job ss_job_zero = {0, 0, 0, 0, 0, 0, 0, 0, "", NULL};
struct ss_container container_jobs_zero = {&ss_job_zero, NULL};
struct ss_container *container_jobs_zero_ptr = &container_jobs_zero;

static struct hp_header hp_header_zero = {0, 0, 0, 0, 0};
static int type_periodic = TYPE_PERIODIC;


static void check_tsa_job_to_task_null_proof(struct test_tsaheylu_data *data)
{
	data->tsa->job_to_task(NULL, &container_jobs_zero_ptr,
			&hp_header_zero, type_periodic);
	data->tsa->job_to_task(data->tsa, NULL,
			&hp_header_zero, type_periodic);
	data->tsa->job_to_task(data->tsa, &container_jobs_zero_ptr,
			NULL, type_periodic);
}

/* Test 2: checks whether intr_task_bond() is "NULL proof" */
static struct task task_zero = {0, 0, "", 0, NULL};
static struct ss_container container_task_zero = {&task_zero, NULL};

static struct ss_intr intr_zero = {0, 0, 0, 0, 0, 0};
static struct ss_container container_intr_zero = {&intr_zero, NULL};

static void check_tsa_intr_task_bond_null_proof(struct test_tsaheylu_data *data)
{
	data->tsa->intr_task_bond(NULL, &container_task_zero,
					&container_intr_zero);
	data->tsa->intr_task_bond(data->tsa, NULL, &container_intr_zero);
	data->tsa->intr_task_bond(data->tsa, &container_task_zero, NULL);
}


/* Test 3: checks whether the returned values from job_to_task() in `jobs`.

******************
 I am finding it really difficult to build these tests. Maybe there is a
 better way I am not seeing now. I will leave these for the future.
******************

static struct hp_header hp_header_normal = {3, 4, 3, 10, 1};

struct ss_job ss_job_normal = {0, 0, 0, 0, 0, 0, 0, 0, "", NULL};
struct ss_container container_jobs_normal = {&ss_job_zero, NULL};
struct ss_container *container_jobs_normal_ptr = &container_jobs_zero;

static void check_tsa_job_to_task_null_proof(struct test_tsaheylu_data *data)
{
	
}

*/


/* Basic infrastructure */

static check_value_t check_value[NUM_TEST_TYPES] = {
	check_tsa_job_to_task_null_proof,
	check_tsa_intr_task_bond_null_proof,
};

static void sanity_checks(unsigned int check)
{
	if (check >= NUM_TEST_TYPES) {
		CTEST_LOG("WARNING: argument check out of bounds");
		ASSERT_FAIL();
	}
}

void check_test_tsaheylu(struct test_tsaheylu_data *data, unsigned int check)
{
	sanity_checks(check);

	check_value[check](data);
}


