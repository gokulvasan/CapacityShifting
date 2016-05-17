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
	def update_curr_time(self, time):
		pass

	def reset_decn(self):
		pass
	def set_decn(self):
		pass
	@property
	def decn(self):
		pass
class time_progress_continous(time_progress):
	
	def __init__(self, time):
		time_progress.__init__(self, time)
		self.time_progress = -1
	@property
	def curr_time(self):
		return self.time_progress
	@property
	def nxt_decn(self):
		return self.time()
	@property
	def decn(self):
		return 1 
	def reset_decn(self):
		pass
	def set_decn(self):
		pass
	def update_curr_time(self, time):
		self.time_progress = time
		#print " >>>>TIME: ", self.slot_count * self.time

class time_progress_discrete(time_progress):
	
	def __init__(self, time):
		time_progress.__init__(self, time)
		self.slot_count = -1
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

	def update_curr_time(self, time):
		self.slot_count += 1
		#print " >>>>TIME: ", self.slot_count * self.time
	@property
	def curr_time(self):
		return self.slot_count

class chronos(Timer):
	def __init__(self, sim, proc_id, time_progress):
		self.time = time_progress
		self.cpu = proc_id
		self.sim = sim
		super(chronos, self).__init__( self.sim, chronos.decision,
					(self, ), 1,
					in_ms=False, one_shot=True, cpu=self.cpu)
		self.decision()

	def decision(self, cpu=None):
		if cpu is None:
			proc_id = self.cpu
		self.time.update_curr_time(self.sim.now_ms())
		self.time.set_decn()
		nxt_decn = self.time.nxt_decn
		print "<<<<<<<<<<<<<<NXT DECN>>>>>>>>",nxt_decn
		self.delay = (nxt_decn * self.sim.cycles_per_ms)
		print "Timer Activation : nxt{} curr{}".format(self.delay, self.get_curr_time)
		print ">>>>SIM:",self.sim.now_ms()
		if self.delay:
			self.start()
			proc_id.resched()
		else:
			self.stop()

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
	@property
	def get_curr_time(self):
		return self.time.curr_time

