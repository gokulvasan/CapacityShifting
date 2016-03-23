from simso.core import Scheduler, Timer
from ss_state_machine import state

class SlotShifting(Scheduler):

	def init(self):
		#self.ready_list = []
		#self.unconcluded_lst = []
		#self.not_guaranteed_lst = []
		self.interval = self.data
		self.curr_job = None
		self.slot_boundary = 0
		self.slot_count = 0
		self.slot_timer = Timer(
			self.sim, SlotShifting.decision, (self, ), 1,
			cpu=self.processors[0], in_ms=True, one_shot=False)
		self.slot_timer.start()
		print " IN SCHEDULER"
		self.interval.print_def_interval()
		self.state = state()

	def decision(self, cpu=None):

		if cpu is None:
			cpu = self.processors[0]
		cpu.resched()
		print "Timer Activation"
		self.slot_boundary = 1

	def on_activate(self, job):
		""""
			1. Check the type of job
			2. if firm aperiodic move to unconcluded list
			3. if soft directly add to !guarantee list
			4. else to ready_list
		"""
		print "job got activated"
		# self.ready_list.append(job)
		self.state.add_job(job)

	def on_deactivate(self, job):
		self.ready_list.remove(job)

	def schedule(self, cpu):
		"""
			1. interval and slot update
			2. run acceptance and guarantee
			3. run EDF on ready_list
		"""
		print "Schedule"
		if self.slot_boundary == 1:
			self.slot_boundary = 0
		else:
			return
		 
