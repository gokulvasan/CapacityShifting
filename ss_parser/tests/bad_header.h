#include "common.h"

#define MAX_FILE_NAME_LEN 1024

CTEST_DATA(fsm_parser_bad_header) {
	struct ss_data* parsed_periodic;
	struct ss_data* parsed_fa;
	struct ss_data* parsed_sa;
	struct ss_data* parsed_end;
	char file_name[MAX_FILE_NAME_LEN];
};

void check_bad_header(struct fsm_parser_bad_header_data *data,
			unsigned int file,
			unsigned int suffix);

