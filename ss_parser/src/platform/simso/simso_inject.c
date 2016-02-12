#include "ss_data.h"
static int simso_data;

static int simso_parse_data(struct platform* self, enum task_type type, 
			struct hp_header *header, void *data) 
{
	return 0;
}

static int simso_configure(struct platform* self, enum task_type type,
			struct hp_header *header)
{
	return 0;
}

static int simso_run(struct platform* self) 
{
	return 0;
}

static int simso_exit(struct platform *self, int exit_code)
{
	return 0;
}

static int simso_intr_inj(struct platform *self, int len, 
			struct ss_intr_inj *intr)
{
	return 0;
}

static int simso_tsk_inj(struct platform *self, struct task *task,
			struct hp_header *hp, int type)
{
	return 0;
}

struct platform* platform_init() 
{
	struct platform *p;
	p = malloc(sizeof(*p));
	if(!p)
		return NULL;

	p->configure = simso_configure;
	p->parse_data = simso_parse_data;
	p->intr_inj = simso_intr_inj;
	p->tsk_inj = simso_tsk_inj;
	p->run = simso_run;
	p->exit = simso_exit;
	p->data = &simso_data;

	return p;
}
