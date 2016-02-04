
class task_data_node:
	def __init(self, job_no, interval ):
		self.job_no = job_no
		self.interval = interval

class task_data:
	def __init__(self):
		self.curr_index = 0
		self.list = None

	def set_current_job(self, job_no):
		l = self.list[self.curr_idex]
		if l.job_no < job_no
			print "Error: wrong job interation"
			return 0
		else if l.job_no == job_no
			return 0
		else 
			self.curr_index = self.curr_index++
			if self.curr_index >= self.list.len()
				return 1
			return 0
	def curr_intr(self)
		l = self.list[self.curr_index]
		return l.interval
	def set_tsk_data(self, data, task)
		
