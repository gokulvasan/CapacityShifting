#include "bad_header.h"

/* These definitions are here to trigger warnings in case the number of headers
 * is changed, so that I remember to change these numbers to the appropriate
 * ones. They are also used in the sanity checks done before executing the
 * tests. */
#define NUM_FILES 5
#define NUM_HEADERS 6

static char *file_names[NUM_FILES] = {
	"data_separator_removed",
	"exclamation_removed",
	"car_return_removed",
	"line_feed_removed",
	"new_line_removed"
};

static char *header_names[NUM_HEADERS] = {
	"_hp_start",
	"_hp_end",
	"_job_start",
	"_job_end",
	"_interval_start",
	"_interval_end"
};

#define TESTS_FOLDER "test_files/fsm_parser_bad_header/"
static void sanity_checks(unsigned int file, unsigned int suffix)
{
	if (file > NUM_FILES) {
		CTEST_LOG("WARNING: argument file out of bounds");
		ASSERT_FAIL();
	} else if (suffix > NUM_HEADERS) {
		CTEST_LOG("WARNING: argument suffix out of bounds");
		ASSERT_FAIL();
	}
}

void check_bad_header(struct fsm_parser_bad_header_data *data,
			unsigned int file,
			unsigned int suffix)
{
	sanity_checks(file, suffix);

	sprintf(data->file_name, "%s%s%s",
		TESTS_FOLDER, file_names[file], header_names[suffix]);

	data->parsed_periodic = parsing_main(data->file_name, TYPE_PERIODIC);
	data->parsed_fa = parsing_main(data->file_name, TYPE_FIRM_APERIODIC);
	data->parsed_sa = parsing_main(data->file_name, TYPE_SOFT_APERIODIC);
	data->parsed_end = parsing_main(data->file_name, TYPE_END);

	ASSERT_NULL(data->parsed_periodic);
	ASSERT_NULL(data->parsed_fa);
	ASSERT_NULL(data->parsed_sa);
	ASSERT_NULL(data->parsed_end);
}



