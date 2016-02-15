from simso.core.Task import GenericTask

"""
Class for a interval node of interval list
"""
class task_intr_node():
	def __init__(self, job_no, intrvl):
		self.j_no = job_no
		self.interval = intrvl
	@property
	def job_no(self):
		return self.j_no

	@property
	def interval(self):
		return self.interval	#holds container_of interval data which is embedded in a list
					#enables easy iteration.
	def set_job_no(self, j_no):
		self.j_no = j_no

	def set_interval(self, interval):
		self.interval = interval

"""
Class used for creating interval list of a task.
"""
class task_intr_association():
	def __init__(self):
		self.intr_list = []
	def append_data_node(self, j_no, interval):
		"""
		creating a composition of interval_list
		"""
		node = task_intr_node(j_no, interval)
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
 
	def set_current_job(self, j_no):
		if None == self.intr_list:
			print "error: intr_list is empty"
			return -1

		l = self.intr_list.get_node(self.curr_index)
		if l.j_no > j_no:
			print "Error: wrong job interation"
			return -1
		elif l.j_no == j_no:
			return 0
		else:
			self.curr_index = self.curr_index + 1
			print 'current index {}'.format(self.curr_index)
			if self.curr_index >= self.intr_count:
				return 1
			return 0
	@property
	def curr_intr(self):
		l = self.intr_list.get_node(self.curr_index)
		return l.interval
	@property
	def tsk_type(self):
		return self.tsk_type

	def set_tsk_data(self, data, task):
		task._task_info.data = data

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
