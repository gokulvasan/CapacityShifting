#include "common.h"

#define MAX_FILE_NAME_LEN 1024
#define MAX_NUM_BINS 10

CTEST_DATA(fsm_parser_test_fields) {
	struct ss_data* parsed;
	char file_name[MAX_FILE_NAME_LEN];
};

void check_test_fields(struct fsm_parser_test_fields_data *data,
			unsigned int file,
			unsigned int test_type,
			unsigned int type);

