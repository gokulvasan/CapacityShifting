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

def __calc_wcet(tmp_lst):
	c = 0
	for j in tmp_lst:
		c += j[2]
	return c

def __asoc_task(tsk_lst, job_lst, intr_id):
	for j in job_lst:
		tsk_id = j[0]
		tsk_lst[tsk_id][5] += 1
		tsk_lst[tsk_id].append(intr_id)

def __gen_intr(job, job_set, tsk_lst, intr_lst, intr_id):
	tmp_lst = []
	j = job
	intr_cnt = 0
	while 1:
		tmp_lst.append(j)
		tsk_id = j[0]
		#tsk_lst[tsk_id][5] += 1
		#tsk_lst[tsk_id].append(intr_id)
		#print "TASK:", tsk_lst

		if len(job_set) == 0:
			break
		if job[3] == job_set[0][3]:
			j = job_set.pop(0)
		else:
			break

	print "intr_id", intr_id, "job_lst", tmp_lst
	if tmp_lst:
		intr_est = min(tmp_lst, key=lambda x:x[4])
		intr_est = intr_est[4]
	else:
		intr_est = 0

	if intr_lst:
		intr_before_end = intr_lst[intr_id - 1][2]
		intr_start = max(intr_est, intr_before_end)
		if intr_before_end != intr_start:
			ibtw_id = intr_id
			ibtw_start = intr_before_end
			ibtw_end = intr_start
			ibtw_sc = ibtw_end - ibtw_start
			intr_lst.append([ibtw_id, ibtw_start, ibtw_end, ibtw_sc])
			intr_id += 1
			intr_cnt += 1
	else:
		intr_start = 0
	
	intr_end = tmp_lst[0][3]
	print "intr start and end", intr_start, intr_end
	intr_sc = intr_end - intr_start
	print "intr_sc: ", intr_sc, __calc_wcet(tmp_lst)
	intr_sc = intr_sc - __calc_wcet(tmp_lst)
	__asoc_task(tsk_lst, tmp_lst, intr_id)
	print "SO -----------intr:", intr_id, intr_start, intr_end, intr_sc
	intr_lst.append([intr_id, intr_start, intr_end, intr_sc])
	intr_cnt += 1
	return intr_cnt

def __correct_intr(intr_lst):
	i_curr = 0
	i_nxt  = 1
	length = len(intr_lst)
	while i_curr < length:
		intr_lst[i_curr][0] = i_curr
		if i_nxt < len(intr_lst):
			if intr_lst[i_curr][2] != intr_lst[i_nxt][1]:
				intr_lst.insert( i_curr, [i_curr+1,
				intr_lst[i_curr][1],
				intr_lst[i_nxt][2], 
				intr_lst[i_nxt][2] - intr_lst[i_curr][1]] )
		i_curr += 1
		i_nxt +=1

def __calculate_sc(intr_lst):
	i = len(intr_lst) - 2
	while i >= 0:
		intr_lst[i][3] = intr_lst[i][3] + min(0, intr_lst[i+1][3])
		i -= 1

def gen_slotshifting_intervals(job_set, tsk_lst):
	intr_lst = []
	intr_id = 0
	while 1:
		if len(job_set):
			job = job_set.pop(0)
			print "job", job
			intr_cnt = __gen_intr(job, job_set, 
				tsk_lst, intr_lst, intr_id)
		else:
			break
		#intr_lst.append(intr)
		intr_id += intr_cnt
	print "Before Correction", intr_lst
	#__correct_intr(intr_lst)
	print "Before SC correction", intr_lst
	__calculate_sc(intr_lst)	
	print "intr", intr_lst

	print "-------------"
	print "tsks:", tsk_lst	
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
	print job_cnt
	while jid <= job_cnt:
		# j_id = jid
		if jid > 1:
			c = task[0]
			d = task[1] * jid
			est = nxt_est #task[2] * jid
		else:
			c = task[0]
			d = task[1]
			est = 0
		nxt_est = task[2]*jid
		tid = i
		tsk_job.append([tid, jid, c, d, est])
		jid += 1
	print "tsk_job: ", tsk_job
	job_lst.extend(tsk_job)	

	# create task based on requirement	
	tsk.append(i)  # 0: id
	tsk.append(0)  # 1: est
	tsk.append(task[0]) #2: wcet
	tsk.append(task[1]) #3: dl
	tsk.append(task[2]) #4: period
	tsk.append(0) #5:intr_cnt
	return tsk

def gen_slotshift_table(nsets, compute, deadline, period, target_util):
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
	#tsk_set = gen_ripoll(nsets, compute, deadline, period, target_util)
	# c, d, p
	tsk_set = [[(3,4,10), (2,5,6), (4,12,12)]]
	#print "task_set : {}".format(tsk_set)
	hp = 12 # get_hyperperiod(tsk_set[0])
	print "hyper period {}".format(hp)

	tsk_cnt = len(tsk_set[0])
	while i < tsk_cnt:
		tsk = gen_jobs(tsk_set[0][i], hp, i, job_lst)
		tsks_lst.append(tsk)
		i += 1
	job_lst.sort(key=itemgetter(3))
	print "job_lst {}".format(job_lst)
	#print "tsk_lst {}",format(tsks_lst)
	intr_lst = gen_slotshifting_intervals(job_lst, tsks_lst)
	print "len task", len(tsks_lst)
	print "tasks: ", tsks_lst
	print "intr len", len(intr_lst)
	print "intr: ", intr_lst
	return[len(tsks_lst), tsks_lst, len(intr_lst), intr_lst]

"""
TEST CODE
"""
t = gen_slotshift_table(1, 1,2 , 2, .5)

