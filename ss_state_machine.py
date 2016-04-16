
from enum import Enum
from ss_queue import *
from simso.configuration import Configuration
from ss_task import tsk_type

class tsk_state(Enum):
	just_arrived = 0
	unconcluded = 1
	not_guarantee = 2
	guarantee = 3
	running = 4
	exit = -1
class state_node(object):
	def __init__(self, queue, not_state, state):
		self.queue = queue
		self.not_state = not_state
		self.state = state
	def enter(self, val):
		return self.queue.queue(val)
	def exit(self, val):
		return self.queue.dequeue(val)

class just_arrived(state_node):
	def __init__(self):
		super(just_arrived, self).__init__(None, 0, 1)
		job = None
		print "just arrived init"
	def exit(self, val):
		return 0
	def test(self):
		print "JA Works"
	def enter(self, val):
		data = val.task.data
		#print "tsk type {}".format(data.tsk_type)
		if (tsk_type.periodic.value  == data.tsk_type or
			tsk_type.firm_aperiodic_g.value == data.tsk_type):
			#print "job is guarantee"
			return tsk_state.guarantee.value
		elif tsk_type.firm_aperiodic.value == data.tsk_type:
			return tsk_state.unconcluded.value
		elif tsk_type.soft_aperiodic.value == data.tsk_type:
			return tsk_state.not_guarantee.value
 		else:
			print "error: in task type"
			return -1

class unconcluded(state_node):
	def __init__(self):
		super(unconcluded, self).__init__(unconcluded_list(), 1, 2)
		print "unconcluded init"
	def enter(self, val):
		tsktype = val.task.data.tsk_type
		if (tsk_type.firm_aperiodic.value == tsktype):
			return super(unconcluded, self).enter(val)
		return -1

class not_guaranteed(state_node):
	def __init__(self):
		super(not_guaranteed, self).__init__(not_guaranteed_list(),
								 11, 4)
		print "!guaranteed init"
	def enter(self, val):
		tsktype = val.task.data.tsk_type
		if (tsk_type.soft_aperiodic.value == tsktype):
			return super(not_guaranteed, self).enter(val)
		return -1

class guaranteed(state_node):
	def __init__(self):
		super(guaranteed, self).__init__(guaranteed_list(), 7, 8)
		print "guaranteed init"
	def enter(self, val):
		tsktype = val.task.data.tsk_type
		if (tsk_type.periodic.value == tsktype or
			tsk_type.firm_aperiodic_g.value == tsktype):
			return super(guaranteed, self).enter(val)
		return -1

class running(state_node):
	def __init__(self):
		super(running, self).__init__(curr_list(), 3, 16)
		print "running init"
	"""def check(self, job):
		if self.exit(job) < 0:
			return 0
		# print " job is current"
		return 1
	"""
"""
Only 3 functions are exposed to outer world.
	1. add_job
	2. transit_unconcluded_task
	3. selection_function.
"""
class state:
	def __init__(self):
		self.__st=[]
		self.__st.insert(tsk_state.just_arrived.value, just_arrived())
		self.__st.insert(tsk_state.unconcluded.value, unconcluded())
		self.__st.insert(tsk_state.not_guarantee.value, not_guaranteed())
		self.__st.insert(tsk_state.guarantee.value, guaranteed())
		self.__st.insert(tsk_state.running.value, running())

	def __check_transition(self, curr_state, nxt_state):
		if curr_state == -1:
			return 1

		if (self.__st[curr_state].not_state &
				self.__st[nxt_state].state):
			return 0
		return 1

	def __change_state(self, job, nxt_state):
		ret = -1
		tsk_data = job.task.data
		#print "CURR STATE{} NXT STATE {}".format(tsk_data.curr_state, nxt_state)
		if self.__check_transition(tsk_data.curr_state, nxt_state):
			if tsk_data.curr_state >= 0:
				if self.__st[tsk_data.curr_state].exit(job) < 0:
					print "curr state failed {}".format(tsk_data.curr_state)
			ret =  self.__st[nxt_state].enter(job)
			tsk_data.curr_state = nxt_state
		return ret

	def rmv_job(self, job):
		data = job.data
		data.curr_state = tsk_state.exit.value 
		curr_job = self.__st[tsk_state.running.value].queue.data
		if curr_job == job:
			#print " removing current job"
			self.__st[tsk_state.running.value].exit(job)
			return
		if tsk_type.soft_aperiodic.value == data.tsk_type:
			self.__st[tsk_state.not_guarantee.value].exit(job)
		else:
			self.__st[tsk_state.guarantee.value].exit(job)

	def add_job(self, job):
		tsk_data = job.task.data
		print "tsk type {}".format(job.data.tsk_type)
		nxt_state =self.__change_state(job,
					 tsk_state.just_arrived.value)
		if nxt_state >= 0:
			# print "Adding {} to state {}".format(job.name, nxt_state)
			return self.__change_state(job, nxt_state)
		return -1

	def transit_unconcluded_tsk(self, transition):
		"""
		1. get uncocnluded task.
		2. perform test using transition function
		3. change tsk type : to soft_aperiodic or firm_aperiodic_g
		4. return count on success
		"""
		queue = self.__st[tsk_state.unconcluded.value].queue
		job = queue.data
		if job:
			tsk = job.task
			state = transition(tsk)
			return self.__change_state(job, state)
		return 0

	def selection_function(self):
		"""
		1. check prev_task type and move it accordingly
		2. try dequeuing periodics/ guarantee
		3. if not check on !guarantee
		4. move the tsk to curr_tsk state.
		"""
		prev_job = self.__st[tsk_state.running.value].queue.data 
		if prev_job:
			#print "prev job {}".format(prev_job.name)
			prev_tsk_type = prev_job.task.data.tsk_type
			if (tsk_type.periodic.value == prev_tsk_type or
			tsk_type.firm_aperiodic_g.value == prev_tsk_type):
				self.__change_state(prev_job,
					tsk_state.guarantee.value)
			elif( tsk_type.soft_aperiodic.value == prev_tsk_type):
				self.__change_state(prev_job,
					 tsk_state.not_guarantee.value)
			else:
				return -1

		job = self.__st[tsk_state.guarantee.value].queue.data
		if None == job:
			job = self.__st[tsk_state.not_guarantee.value].queue.data
		if None != job:
			self.__change_state(job, tsk_state.running.value)
			#print " Selected job {}".format(job.name)
		return job

"""
s = state()
j = just_arrived(None, 0, 0) 
print j.enter(0)
"""
