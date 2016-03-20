
from ss_queue import *
from enum import Enum

class tsk_state(Enum):
	just_arrived = 0
	unconcluded = 1
	not_guarantee = 2
	guarantee = 3

class state_node(object):
	def __init__(self, queue, not_state, state):
		self.queue = queue
		self.not_state = not_state
		self.state = state
	def enter(self, val):
		self.queue.queue(val)
	def exit(self, val):
		self.queue.dequeue(val)

class just_arrived(state_node):
	def __init__(self):
		super(just_arrived, self).__init__(None, 0, 1)
		job = None
		print "just arrived init"
	def enter(self, val):
		return 0
	def test(self):
		print "JA Works"
	def exit(self, val):
		task = val.task
		data = task.data
		
		if 0 == data.tsk_type: 
			return tsk_state.guarantee
		elif 1 == data.tsk_type:
			return tsk_state.unconcluded
		elif 2 == data.tsk_type:
			return tsk_state.not_guarantee
 		else: 
			return -1

class unconcluded(state_node):
	def __init__(self):
		queue = unconcluded_list()
		super(unconcluded, self).__init__(queue, 1, 2)
		print "unconcluded init"
	def enter(self, val):
		pass				
	def exit(self,val):	
		pass

class not_guaranteed(state_node):
	def __init__(self):
		queue = not_guaranteed_list()
		super(not_guaranteed, self).__init__(queue, 11, 4)
		print "!guaranteed init"

class guaranteed(state_node):
	def __init__(self):
		queue = guaranteed_list()
		super(guaranteed, self).__init__(queue, 7, 8)
		print "guaranteed init"

class state:
	def __init__(self):
		self.__st=[]
		self.__st.insert(tsk_state.just_arrived.value, just_arrived())
		self.ulist = unconcluded() 
		self.__st.insert(tsk_state.unconcluded.value, self.ulist)
		self.nglist = not_guaranteed()
		self.__st.insert(tsk_state.not_guarantee.value, self.nglist)
		self.glist = guaranteed()
		self.__st.insert(tsk_state.guarantee.value, self.glist)
	
	def __check_transition(self, curr_state, nxt_state):
		
		if curr_state == -1:
			return 1
		elif (self.__st[curr_state].not_state & 
				self.__st[nxt_state].state):
			return 1

		return 0
	
	def __change_state(self, tsk, nxt_state):
		curr_state = tsk.data.curr_state
		if self.__check_transition(curr_state, nxt_state):
			tsk.data.curr_state = nxt_state
			if curr_state >= 0:
				self.__st[curr_state].exit()
			return self.__st[nxt_state].enter()
		return -1
	def add_task(self, tsk):
		nxt_state =self.__change_state(tsk, tsk_state.just_arrived.value)
		return self.__change_state(tsk, nxt_state)

	def transit_unconcluded_tsk(self, transition):
		"""
		1. get uncocnluded task.
		2. perform test
		3. return count on success
		"""
	def select_nxt_tsk(self, prev_tsk):
		pass	

s = state()
"""
j = just_arrived(None, 0, 0) 
print j.enter(0)
"""
