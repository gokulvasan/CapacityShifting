#include "ss_data.h"


void ss_inject_del(struct ss_inject *i)
{
	if (!i)
		return;
	i->plat->exit(i->plat);
}

int ss_inj_per_task(
	struct ss_inject *inj,
	struct task *task,
	struct hp_header *header,
	int type)
{
	int ret = -1;

	if (!inj || !task || !header)
		return ret;

	do {
		ret = inj->plat->tsk_inj(inj->plat, task, header, type);
		if(ret < 0)
			perror("platfrom task inj fails\n");	
	} while(0);
	return ret;
}

int ss_inj_interval(struct ss_inject *inj, int len, struct ss_intr_inj *intr)
{
	int ret = -1;

	if(!intr || !inj) {
		//fprintf(stderr, "ERROR\n");
		return ret;
	}
#if defined (DEBUG) && defined (SYS_HOST)
	return 0;
#endif
	do {
		ret = inj->plat->intr_inj(inj->plat, len, intr);
		if(ret < 0) 
			perror("platform interval inject fails\n");
	} while(0);

	return ret;
}

void ss_inject_init(struct ss_inject *i)
{
	/* This function expects i to be already allocated */
	if (!i)
		return;

	i->inj_interval = ss_inj_interval;
	i->inj_per_task = ss_inj_per_task;

	i->plat = platform_init();
}
