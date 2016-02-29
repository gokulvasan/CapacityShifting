from simso.core import Scheduler

class SlotShifting(Scheduler):

	def init(self):
		self.ready_list = []
		self.unconcluded_lst = []
		self.not_guaranteed_lst = []
		self.interval = self.data
		self.curr_job = None
		self.slot_boundary = 0
		self.slot_count = 0

	def on_activate(self, job):
		""""
			1. Check the type of job
			2. if firm aperiodic move to unconcluded list
			3. if soft directly add to !guarantee list
			4. else to ready_list
		"""
		print "job got activated"
		self.ready_list.append(job)

	def on_deactivate(self, job):
		self.ready_list.remove(job)

	def schedule(self, cpu):
		"""
			1. interval and slot update
			2. run acceptance and guarantee
			3. run EDF on ready_list
		"""
	
