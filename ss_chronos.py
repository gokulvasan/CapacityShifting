from simso.core import Timer

class time_progress(object):
	def __init__(self, time):
		self.time = time
	
	@property
	def curr_time(self):
		pass
	@property
	def nxt_decn(self):
		return 0
	def update_curr_time(self):
		pass

	def reset_decn(self):
		pass
	def set_decn(self):
		pass
	@property
	def decn(self):
		pass

class time_progress_discrete(time_progress):
	
	def __init__(self, time):
		time_progress.__init__(self, time)	
		self.slot_count = 0
		self.slot_boundary = 1

	@property
	def curr_time(self):
		return self.slot_count
	@property
	def nxt_decn(self):
		return self.time

	@property
	def decn(self):
		return self.slot_boundary
	def reset_decn(self):
		self.slot_boundary = 0
	def set_decn(self):
		self.slot_boundary = 1
	
	def update_curr_time(self):
		self.slot_count += 1
	@property
	def curr_time(self):
		return self.slot_count

class chronos(Timer):
	def __init__(self, sim, proc_id, time_progress):
		self.time = time_progress
		self.cpu = proc_id
		self.sim = sim
		super(chronos, self).__init__( self.sim, chronos.decision,
					(self, ), self.time.nxt_decn,
					in_ms=True, one_shot=True, cpu=self.cpu)
		self.start()

	def decision(self, cpu=None):
		if cpu is None:
			proc_id = self.cpu

		print "Timer Activation"
		
		self.time.update_curr_time()
		self.time.set_decn()
		self.delay = self.time.nxt_decn

		self.start()
		proc_id.resched()

	@property
	def is_resched(self):
		if self.time.decn:
			return 1
		return 0

	@property
	def in_sched(self):
		if self.is_resched:
			self.time.reset_decn()
			return 1
		return 0

	def get_curr_time(self):
		return self.time.curr_time

