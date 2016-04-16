from simso.core.Task import GenericTask
from enum import Enum

class tsk_type(Enum):
	periodic = 0
	firm_aperiodic = 1
	soft_aperiodic = 2
	firm_aperiodic_g = 3

"""
Class for a interval node of interval list
"""
class task_intr_node():
	def __init__(self, job_no, intrvl, intr_id):
		self.j_no = job_no
		self.interval = intrvl
		self.intr_id = intr_id
	@property
	def job_no(self):
		return self.j_no
	@property
	def interval(self):
		return self.interval #holds container_of interval
				     #data which is embedded in a lst
				     #enables easy iteration.
	@property
	def intr_id(self):
		return self.intr_id

	def set_job_no(self, j_no):
		self.j_no = j_no

	def set_interval(self, interval):
		self.interval = interval
	
	def set_intr_id(self, intr_id):
		self.intr_id = intr_id

"""
Class used for creating interval list of a task.
"""
class task_intr_association():
	def __init__(self):
		self.intr_list = []
	def append_node(self, j_no, interval, intr_id):
		"""
		creating a composition of interval_list
		"""
		node = task_intr_node(j_no, interval, intr_id)
		self.intr_list.append(node)

	@property
	def intr_count(self):
		return len(self.intr_list)

	def get_node(self,index):
		if None == self.intr_list:
			return None

		if index == None:
			return self.intr_list[0]
		else:
			return self.intr_list[index]

"""
Class used for acessing task data and setting task data in
case of aperiodics.
This class inherits task_intr_list and task_intr_node 
provides a right approach to access the data
"""
class task_data:
	def __init__(self, intr_list, length, tsk_type):
		self.tsk_type = tsk_type
		self.curr_index = 0
		"""
		Creating aggreagation here, though it is
		not necessary, it creates good abstraction.
		"""
		self.intr_list = intr_list
		self.intr_count = length
		self.state = -1	
	@property
	def curr_intr(self):
		if self.curr_index >= self.intr_count:
			return None 
		l = self.intr_list.get_node(self.curr_index)
		if l != None:
			return l.interval
		return None
	@property
	def curr_state(self):
		return self.state
	@property
	def tsk_type(self):
		return self.tsk_type

	# this needs to be called on each job activation. 
	def set_current_job(self, j_no):

		if None == self.intr_list:
			print "error: intr_list is empty"
			return -1

		if self.curr_index >= self.intr_count:
			return 0

		print "job number {}".format(j_no)
		l = self.intr_list.get_node(self.curr_index)
		if l.j_no > j_no:
			print "Error: wrng job iter:l.jno{} jno{}".format(l.j_no, j_no)
			return -1
		elif l.j_no == j_no:
			return 1
		else:
			self.curr_index = self.curr_index + 1
			print 'curridx {}'.format(self.curr_index)
			if self.curr_index >= self.intr_count:
				return 0
		return 1
	# This needs a better abstraction later
	def set_tsk_data(self, data, task):
		task._task_info.data = data

	def print_data(self):
		print "********TASK*******"
		print "tsk_type {}".format(self.tsk_type)
		print "curr_index{}".format(self.curr_index)
		print "intr_count {}".format(self.intr_count)
		i = 0
		while i < self.intr_count:
			print "----------"
			j = self.intr_list.get_node(i)
			print "j_no {}".format(j.j_no)
			print "interval {}".format(j.interval)
			print "intr id {}".format(j.intr_id)
			i += 1
		print "-----------"
		print "intr list {}".format(self.intr_list)	
"""
#test
t = task_intr_list()
t.append_data_node(0,1)
t.append_data_node(1,2)
t.append_data_node(2,3)

k = task_data(t, t.intr_count,0)
k.set_current_job(1)
print 'current interval {}.'.format(k.curr_intr)
print 'task type {}.'.format(k.tsk_type)
"""
