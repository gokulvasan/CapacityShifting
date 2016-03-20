
class queue(object):
	def __init__(self):
		self.queue = []
	def queue(self, data):
		pass
	def dequeue(self, data):
		self.queue.remove(data)
		return 0
	def data(self):
		pass

class guaranteed_list(queue):
	def __init__(self):
		super(guaranteed_list, self).__init__()
	def queue(self, data):
		self.queue.append(data)

	def data(self):
		if self.queue:
			job = min(self.queue, key=lambda x: x.absoulute_deadline)
			if task != None:
				return job
		return None

class not_guaranteed_list(queue):
	def __init__(self):
		super(not_guaranteed_list, self).__init__()
	def queue(self, data):
		self.queue.append(data)
	def data(self):
		if self.queue:
			job = self.queue.pop(0)
			return job
		return None


class unconcluded_list(queue):
	def __init__(self):
		super(unconcluded_list, self).__init__()
	def queue(self, data):
		self.queue.append(data)
	def data(self):
		if self.queue:
			job = self.queue.pop(0)
			return job
		return None

class curr_list():
	def __init__(self):
		self.curr_task = None
	def queue(self, data):
		if self.curr_task:
			return -1
		self.curr_task = data
	def dequeue(self, data):
		if self.curr_task:
			return -1
		job = self.curr_task
		self.curr_task = None
		return job

"""
q = unconcluded_list()
"""

