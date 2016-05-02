from simso.generator.task_generator import gen_ripoll 
from operator import itemgetter

def _gcd(*numbers):
    """Return the greatest common divisor of the given integers"""
    from fractions import gcd
    return reduce(gcd, numbers)

def _lcm(numbers):
    """Return lowest common multiple."""
    def lcm(a, b):
        return (a * b) // _gcd(a, b)
    return reduce(lcm, numbers, 1)

def get_hyperperiod(task_set):
	"""
	Compute and return the hyperperiod of the tasks.
	"""
	return _lcm([x[2] for x in task_set])

def __calc_sc(tmp_lst):
	c = 0
	for j in tmp_lst:
		c += j[4]
	return c

def __gen_intr(job, job_set, tsk_lst, intr_lst, intr_id):
	tmp_lst = []
	j = job
	while 1:
		tmp_lst.append(j)
		if job_set == None:
			break
		if job[3] == job_set[0][3]:
			 j = job_set.pop[0]
		else:
			break

	intr_est = min(job_set, key=lambda x:x[4])
	if intr_lst:
		intr_start = max(intr_est, intr_lst[intr_id - 1][2])
	intr_end = tmp_lst[0][2]
	intr_sc = intr_end - intr_start
	intr_sc = intr_sc - __calc_sc(tmp_lst)
	return (intr_id, intr_start, intr_end, intr_sc)
def __correct_intr(intr_lst):
	pass
def calculate_sc(intr_lst):
	pass

def gen_slotshifting_intervals(job_set, tsk_lst):
	intr_lst = []
	intr_id = 0
	for j in job_set:
		job = job_set.remove(j)
		intr = __gen_intr(job, job_set, tsk_lst, intr_lst, intr_id)
		intr_lst.append(intr)
		intr_id += 1
	return intr_lst

def gen_jobs(task, hp, i, job_lst):
	"""
	task: task that gets converted into jobs
	hp : hyper period
	job_list: job list to which current task->job gets appended
	"""
	tsk = []
	tsk_job = []
	job_cnt = hp/task[2]
	jid  = 1
	while cnt < job_cnt:
		j_id = jid
		if j_id > 1:
			c = task[0] * jid
			d = task[1] * jid
			p = task[2] * jid
		else:
			c = task[0]
			d = task[1]
			p = 0 
		t = i
		tsk_job.append((t, j_id, c, d, p))
		jid += 1
	job_lst.extend(tsk_job)	
	
	tsk.append(i)  # id
	tsk.append(0)  # est
	tsk.append(task[0]) #wcet
	tsk.append(task[1]) #dl
	tsk.append(task[2]) #period
	tsk.append(0) #intr_cnt
	return tsk

def gen_slotshifting_table(nsets, compute, deadline, period, target_util):
	"""
	Args:
	- `nsets`: Number of tasksets to generate.
	- `compute`: Maximum computation time of a task.
	- `deadline`: Maximum slack time.
	- `period`: Maximum delay after the deadline.
	- `target_util`: Total utilization to reach.
	"""
	i = 0
	job_lst = []
	tsks_lst = []
	tsk_set = gen_ripoll(nsets, compute, deadline, period, target_util)
	hp = get_hyperperiod(tsk_set[0])
	tsk_cnt = len(tsk_set[0])
	while i < tsk_cnt:
		tsk = gen_jobs(tsk_set[0][i], hp, i, job_lst)
		tsks_lst.append(tsk)
		i += 1
	sorted(job_lst, key=itemgetter(2))
	intr_lst = gen_slotshifting_intervals(job_lst)

