
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

class state_node(object):
	def __init__(self, queue, not_state, state):
		self.lst = queue
		self.not_state = not_state
		self.state = state
	def enter(self, val):
		return self.lst.queue(val)
	def exit(self, val):
		return self.lst.dequeue(val)

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
		task = val.task
		data = task.data

		if (tsk_type.periodic.value  == data.tsk_type or
			tsk_type.firm_aperiodic_g.value == data.tsk_type):
			return tsk_state.guarantee.value
		elif tsk_type.firm_aperiodic.value == data.tsk_type:
			return tsk_state.unconcluded.value
		elif tsk_type.soft_aperiodic.value == data.tsk_type:
			return tsk_state.not_guarantee.value
 		else:
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
			return super(not_guaranteed, self).enter(val)
		return -1

class running(state_node):
	def __init__(self):
		super(running, self).__init__(curr_list(), 3, 16)
		print "running init"

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
		elif (self.__st[curr_state].not_state &
				self.__st[nxt_state].state):
			return 0
		return 1

	def __change_state(self, job, nxt_state):
		ret = -1
		tsk_data = job.task.data
		if self.__check_transition(tsk_data.curr_state, nxt_state):
			if tsk_data.curr_state >= 0:
				self.__st[tsk_data.curr_state].exit(job)
			ret =  self.__st[nxt_state].enter(job)
			tsk_data.curr_state = nxt_state
		return ret

	def add_job(self, job):
		tsk_data = job.task.data
		nxt_state =self.__change_state(job,
					 tsk_state.just_arrived.value)
		if nxt_state >= 0:
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

	def selection_function(self, prev_job):
		"""
		1. check prev_task type and move it accordingly
		2. try dequeuing periodics/ guarantee
		3. if not check on !guarantee
		4. move the tsk to curr_tsk state.
		"""
		if prev_job:
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
			return job
		return None

"""
s = state()
j = just_arrived(None, 0, 0) 
print j.enter(0)
"""
