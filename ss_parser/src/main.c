#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "fsm_parser.h"
#include "ss_data.h"

#define OPTSTR "phsn:r"
int main (int argc, char *argv[])
{
	/* Reads command line, as well as .conf file */
	struct ss_data *inject_data;
	struct hp_header *header;

	struct ss_inject ss_inject, *inj = &ss_inject;
	struct ss_tsaheylu ss_tsa, *tsa = &ss_tsa;

	int ret = -1;
	enum task_type task= TYPE_END;
	int opt;
	int no_release = 0;
	int release = 0;
	void *platform_data = NULL;

	/* Initializes injection data structures */
	ss_inject_init(inj);
	if(!ss_tsaheylu_init(tsa, inj)) {
		fprintf(stderr, "Error: tsaheylu init failed\n");
		return -1;
	}
	do {
		while((opt = getopt(argc, argv, OPTSTR)) != -1) {
			switch(opt) {
			case 'p':
				task = TYPE_PERIODIC;
			break;
			case 'h':
				task = TYPE_FIRM_APERIODIC;
			break;
			case 's':
				task = TYPE_SOFT_APERIODIC;
			break;
			case 'n':
				printf("NO release\n");
				no_release = 1;
			break;
			case 'r':
				inj->plat->run(inj->plat);
				release = 1;
			break;
			default:
				fprintf(stderr, "Error: wrong args\n");
			break;
			}
		}
		if(('?' == opt) || (release))
			break;

		if(no_release && release) {
			fprintf(stderr, "contradicting Arguments\n");
			break;
		}

		printf("optind %d\n", optind);
		inject_data = parsing_main(argv[optind], task);
		if(!inject_data) {
			fprintf(stderr,
				"ERROR:%s: Parsing failed\n",
				__FUNCTION__);
			break;
		}
#if 0
		/* This need to become platform configuration init*/
		inj->set_res_id(inj, 1234);

		if(inj->res_create(inj, res_len, 0, task, header) ) {
			fprintf(stderr, "Error: %s: Reservation create failed\n",
			__FUNCTION__);
			break;
		}
#else
		 inj->plat->parse_data(inj->plat, task, inject_data->res_data->data, platform_data);
		 inj->plat->configure(inj->plat, task, inject_data->res_data->data);
#endif
		/*TSAHEYLU */
		if(tsa->job_to_task(tsa, &inject_data->task, header, task)) {
			inj->plat->exit(inj->plat);
			fprintf(stderr, "ERROR %s: task bonding failed\n",
				__FUNCTION__);
			break;
		}
		if(task == TYPE_PERIODIC &&
			tsa->intr_task_bond(
				tsa,
				inject_data->task,
				inject_data->interval)) {

			fprintf(stderr,
				"ERROR: %s: interval bonding failed\n",
				 __FUNCTION__);
			break;
		}

		if(!no_release) {
			inj->plat->run(inj->plat);
		}
		printf("INJECTION_SUCCESS\n");
		ret = 0;

	} while(0);

	return ret;
}

