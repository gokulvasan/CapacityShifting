
class sslist(object):
	def __init__(self):
		self.q = []
	def queue(self, data):
		self.q.append(data)
		return 0
	def dequeue(self, data):
		self.q.remove(data)
		return 0
	@property
	def data(self):
		pass

class guaranteed_list(sslist):
	def __init__(self):
		super(guaranteed_list, self).__init__()
	@property
	def data(self):
		if self.q:
			job = min(self.q,
				key=lambda x: x.absoulute_deadline)
			if job != None:
				return job
		return None

class not_guaranteed_list(sslist):
	def __init__(self):
		super(not_guaranteed_list, self).__init__()
	@property
	def data(self):
		if self.q:
			job = self.q.pop(0)
			return job
		return None

class unconcluded_list(sslist):
	def __init__(self):
		super(unconcluded_list, self).__init__()
	@property
	def data(self):
		if self.q:
			job = self.q.pop(0)
			return job
		return None

class curr_list():
	def __init__(self):
		self.curr_job = None
	def queue(self, data):
		if self.curr_job:
			return -1
		self.curr_job = data
		return 0
	def dequeue(self, data):
		if ( None == self.curr_task or
			data != self.curr_tsk) :
			return -1
		self.curr_task = None
		return 0

"""
q = guaranteed_list()
q.queue(1)
q.dequeue(1)
"""
