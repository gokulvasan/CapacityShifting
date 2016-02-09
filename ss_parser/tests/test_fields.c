#include "test_fields.h"

/* These definitions are here to trigger warnings in case the number of headers
 * is changed, so that I remember to change these numbers to the appropriate
 * ones. They are also used in the sanity checks done before executing the
 * tests. */
#define NUM_FILES 3
#define NUM_TEST_TYPES 19
#define NUM_TASK_TYPES 4

#define TESTS_FOLDER "test_files/fsm_parser_test_fields/"

static char *file_names[NUM_FILES] = {
	"1task1core0bin",
	"2task1core2bin",
	"3task2core0bin"
};


union test_answers {
	unsigned int uint_answer;
	char* str;
};

/* Since this is a static variable, anything that is left not filled (i.e.,
 * "unanswered") will have the value 0. */
static union test_answers answers[NUM_FILES][NUM_TEST_TYPES] = {
	/* The answers are ordered according to the indices of the functions.
	 * If you change the order of one, you will have to change the other. */
	{ // File 1
		{.uint_answer = 1}, // job count
		{.uint_answer = 14}, // int count
		{.uint_answer = 14}, // intr job association
		{.uint_answer = 238}, // slot count
		{.uint_answer = 1}, // slot quantum
		{.uint_answer = 1}, // num_jobs_in_list
		{.uint_answer = 0}, // sum_job_major_id
		{.uint_answer = 0}, // sum_job_est
		{.uint_answer = 15}, // sum_job_wcet
		{.uint_answer = 17}, // sum_job_dl
		{.uint_answer = 1}, // sum_job_coreid
		{.uint_answer = 14}, // sum_job_intr_count
		{.str = ""}, // sum_job_binaries
		{.uint_answer = 14}, // num_intervals_in_list
		{.uint_answer = 91}, // sum_intr_id
		{.uint_answer = 14}, // sum_intr_core
		{.uint_answer = 1547}, // sum_intr_start
		{.uint_answer = 1785}, // sum_intr_end
		{.uint_answer = 28}, // sum_intr_sc
	},
	{ // File 2
		{.uint_answer = 2}, // job count
		{.uint_answer = 7}, // int count
		{.uint_answer = 7}, // intr job association
		{.uint_answer = 75}, // slot count
		{.uint_answer = 5}, // slot quantum
		{.uint_answer = 2}, // num_jobs_in_list
		{.uint_answer = 1}, // sum_job_major_id
		{.uint_answer = 0}, // sum_job_est
		{.uint_answer = 14}, // sum_job_wcet
		{.uint_answer = 40}, // sum_job_dl
		{.uint_answer = 2}, // sum_job_coreid
		{.uint_answer = 7}, // sum_job_intr_count
		{.str = "/bin/ls/bin/dumbloop"}, // sum_job_binaries
		{.uint_answer = 7}, // num_intervals_in_list
		{.uint_answer = 21}, // sum_intr_id
		{.uint_answer = 7}, // sum_intr_core
		{.uint_answer = 225}, // sum_intr_start
		{.uint_answer = 300}, // sum_intr_end
		{.uint_answer = 12}, // sum_intr_sc
	},
	{ // File 3
		{.uint_answer = 3}, // job count
		{.uint_answer = 31}, // int count
		{.uint_answer = 29}, // intr job association
		{.uint_answer = 222}, // slot count
		{.uint_answer = 1}, // slot quantum
		{.uint_answer = 3}, // num_jobs_in_list
		{.uint_answer = 3}, // sum_job_major_id
		{.uint_answer = 0}, // sum_job_est
		{.uint_answer = 34}, // sum_job_wcet
		{.uint_answer = 69}, // sum_job_dl
		{.uint_answer = 4}, // sum_job_coreid
		{.uint_answer = 29}, // sum_job_intr_count
		{.str = ""}, // sum_job_binaries
		{.uint_answer = 31}, // num_intervals_in_list
		{.uint_answer = 465}, // sum_intr_id
		{.uint_answer = 43}, // sum_intr_core
		{.uint_answer = 3502}, // sum_intr_start
		{.uint_answer = 3946}, // sum_intr_end
		{.uint_answer = 57}, // sum_intr_sc
	},
};

typedef void (*check_value_t) (
	struct fsm_parser_test_fields_data *data,
	unsigned int file,
	unsigned int type
);

/* Defined below. Needed by all "check_" functions */
static int get_function_index(check_value_t fn);

static void check_job_count(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned type)
{
	int this = get_function_index(check_job_count);
	struct hp_header *header = data->parsed->res_data->data;
	ASSERT_EQUAL(answers[file][this].uint_answer, header->job_count);
}

static void check_int_count(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned type)
{
	int this = get_function_index(check_int_count);
	struct hp_header *header = data->parsed->res_data->data;
	ASSERT_EQUAL(answers[file][this].uint_answer, header->int_count);
}

static void check_intr_job_asc(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned type)
{
	int this = get_function_index(check_intr_job_asc);
	struct hp_header *header = data->parsed->res_data->data;
	ASSERT_EQUAL(answers[file][this].uint_answer, header->intr_job_asc);
}

static void check_slot_count(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned type)
{
	int this = get_function_index(check_slot_count);
	struct hp_header *header = data->parsed->res_data->data;
	ASSERT_EQUAL(answers[file][this].uint_answer, header->slot_count);
}

static void check_slot_quantum(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	int this = get_function_index(check_slot_quantum);
	struct hp_header *header = data->parsed->res_data->data;
	ASSERT_EQUAL(answers[file][this].uint_answer, header->slot_quantum);
}



/* I noticed that MOST functions were more or less the same, with only some
 * tweaks here and there. I hope this macro doesn't make the code unreadable. */
#define CHECK_JOBS_GENERIC(data, file, function, what_to_check, vars, \
			assert_type, answer_name) \
	CHECK_GENERIC(data, file, function, what_to_check, vars, assert_type, \
			ss_job, task, answer_name)

#define CHECK_INTR_GENERIC(data, file, function, what_to_check, vars, \
			assert_type, answer_name) \
	CHECK_GENERIC(data, file, function, what_to_check, vars, assert_type, \
			ss_intr, interval, answer_name)

#define CHECK_GENERIC(data, file, function, what_to_check, vars, assert_type, \
			job_or_intr, job_intr_name, answer_name) \
do { \
	int this = get_function_index(function); \
	struct ss_container *i; \
	struct job_or_intr *it; \
	vars; \
	for (i = data->parsed->job_intr_name; i != NULL; i = i->nxt) { \
		if (!i->data) \
			ASSERT_FAIL(); \
		it = i->data; \
		what_to_check; \
	} \
	assert_type(answers[file][this].answer_name, count); \
} while (0)

static void check_num_jobs_in_list(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_num_jobs_in_list,
			/* This ends up doing "count++". It is this way only
			 * to prevent the compiler to throw a warning on
			 * "unused variable j". */
			count += it ? 1 : 1, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_major_id(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_major_id,
			count += it->major_id, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_est(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_est,
			count += it->est, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_wcet(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_wcet,
			count += it->wcet, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_dl(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_dl,
			count += it->dl, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_coreid(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_coreid,
			count += it->coreid, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_intr_count(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_intr_count,
			count += it->intr_count, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_job_binaries(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_JOBS_GENERIC(data, file, check_sum_job_binaries,
			strcat(count, it->binary),
			char count[MAX_FILE_NAME_LEN * MAX_NUM_BINS] = "\0",
			ASSERT_STR, str);
}

static void check_num_intervals_in_list(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_INTR_GENERIC(data, file, check_num_intervals_in_list,
			count += it ? 1 : 1, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_intr_id(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_INTR_GENERIC(data, file, check_sum_intr_id,
			count += it->intr_id, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_intr_core(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_INTR_GENERIC(data, file, check_sum_intr_core,
			count += it->core, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_intr_start(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_INTR_GENERIC(data, file, check_sum_intr_start,
			count += it->start, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_intr_end(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_INTR_GENERIC(data, file, check_sum_intr_end,
			count += it->end, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}
static void check_sum_intr_sc(struct fsm_parser_test_fields_data *data,
			unsigned int file, unsigned int type)
{
	CHECK_INTR_GENERIC(data, file, check_sum_intr_sc,
			count += it->sc, int count = 0,
			ASSERT_EQUAL,
			uint_answer);
}

static check_value_t check_value[NUM_TEST_TYPES] = {
	/* The functions are ordered according to their index in the answers.
	 * If you change their order, you will have to change the indices of
	 * their answers */
	check_job_count,
	check_int_count,
	check_intr_job_asc,
	check_slot_count,
	check_slot_quantum,

	check_num_jobs_in_list,
	check_sum_job_major_id,
	check_sum_job_est,
	check_sum_job_wcet,
	check_sum_job_dl,
	check_sum_job_coreid,
	check_sum_job_intr_count,
	check_sum_job_binaries,

	check_num_intervals_in_list,
	check_sum_intr_id,
	check_sum_intr_core,
	check_sum_intr_start,
	check_sum_intr_end,
	check_sum_intr_sc,
};

static int get_function_index(check_value_t fn)
{
	int i;
	for (i = 0; i < NUM_TEST_TYPES; i++) {
		if (check_value[i] == fn)
			return i;
	}

	/* This should simply NEVER happen */
	ASSERT_FAIL();
	/* Return only to remove a warning */
	return -1;
}


static void sanity_checks(unsigned int file, unsigned int test_type,
			unsigned int type)
{
	if (file >= NUM_FILES) {
		CTEST_LOG("WARNING: argument file out of bounds");
		ASSERT_FAIL();
	} else if (test_type >= NUM_TEST_TYPES) {
		CTEST_LOG("WARNING: argument test_type out of bounds");
		ASSERT_FAIL();
	} else if (type >= NUM_TASK_TYPES) {
		CTEST_LOG("WARNING: argument type out of bounds");
		ASSERT_FAIL();
	}
}

void check_test_fields(struct fsm_parser_test_fields_data *data,
			unsigned int file,
			unsigned int test_type,
			unsigned int type)
{
	sprintf(data->file_name, "%s%s", TESTS_FOLDER, file_names[file]);
	sanity_checks(file, test_type, type);
	data->parsed = parsing_main(data->file_name, type);

	if (!data->parsed)
		ASSERT_FAIL();

	check_value[test_type](data, file, type);
}


