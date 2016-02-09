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
	// pid_t wpid;
	// int status;
	// int count;
	lt_t res_len;
	enum task_type task= TYPE_END;
	// int sleep_cnt = 0;
	int opt;
	int no_release = 0;
	int release = 0;

	/* Initializes injection data structures */
	ss_inject_init(inj);
	ss_tsaheylu_init(tsa, inj);

	do {
		/*if(argc <= 2 || argc > 3) {
			fprintf(stderr, "ERROR:argc\n");
			break;
		} */

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
				inj->wait_till_release(inj);
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
		if(TYPE_PERIODIC == task) {
			header = inject_data->res_data->data;
			if(!header->slot_quantum) {
				fprintf(stderr, "ERROR:%s: Slot quantum is 0\n",
			 	__FUNCTION__);
				break;
			}
			res_len = header->slot_count * header->slot_quantum;
			if(res_len <= 0) {
				fprintf(stderr, "Error %s: reservation len should be <0\n",
				__FUNCTION__);
				break;
			}
		}
		inj->set_res_id(inj, 1234);

		if(inj->res_create(inj, res_len, 0, task, header) ) {
			fprintf(stderr, "Error: %s: Reservation create failed\n",
			__FUNCTION__);
			break;
		}
		if(tsa->job_to_task(tsa, &inject_data->task, header, task)) {
			//TODO: kill all child here.
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
			inj->wait_till_release(inj);
		}
		printf("INJECTION_SUCCESS\n");
		ret = 0;

	} while(0);

	return ret;
}

