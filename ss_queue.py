
class sslist(object):
	def __init__(self):
		self.q = []
	def queue(self, data):
		self.q.append(data)
		return 0
	def dequeue(self, data):
		if data in self.q:
			self.q.remove(data)
		else:
			print "data is not in list"
		return 0
	
	def data(self):
		pass

class guaranteed_list(sslist):
	def __init__(self):
		super(guaranteed_list, self).__init__()
	
	def data(self):
		if self.q:
			job = min(self.q,
				key=lambda x: x.absolute_deadline)
			if job != None:
				return job
		return None

class not_guaranteed_list(sslist):
	def __init__(self):
		super(not_guaranteed_list, self).__init__()
	
	def data(self):
		if self.q:
			job = self.q.pop(0)
			return job
		return None

class unconcluded_list(sslist):
	def __init__(self):
		super(unconcluded_list, self).__init__()
	
	def data(self):
		if self.q:
			# ERROR: here it should dequeue
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

	def data(self):
		return self.curr_job

	def dequeue(self, data):
		"""if ( None == self.curr_job or
			data != self.curr_job) :
			return -1
		"""
		self.curr_job = None
		return 0

"""
q = guaranteed_list()
q.queue(1)
q.dequeue(1)
"""
