from simso.core import Scheduler, Timer
from ss_state_machine import state
from ss_chronos import *
class SlotShifting(Scheduler):

	def init(self):
		self.interval = self.data
		self.curr_job = None
		print " IN SCHEDULER"
		self.interval.print_def_interval()
		self.state = state()
		self.time  = chronos(self.sim,  self.processors[0], time_progress_discrete(5))

	def on_activate(self, job):
		""""
			1. Check the type of job
			2. if firm aperiodic move to unconcluded list
			3. if soft directly add to !guarantee list
			4. else to ready_list
		"""
		print "job got activated"
		self.state.add_job(job)

	def on_deactivate(self, job):
		print "job deactivated"
		self.state.rmv_job(job)

	def schedule(self, cpu):
		"""
			1. interval and slot update
			2. run acceptance and guarantee
			3. run EDF on ready_list
		"""
		print "Schedule Curr Time {}".format(self.time.get_curr_time())
		if self.time.in_sched == 0:
			return None
		if self.curr_job:
			tsk = self.curr_job.task
		else:
			tsk = None
		self.interval.update_intr(self.time.get_curr_time, tsk)

		while  self.state.transit_unconcluded_tsk(self.interval.aperiodic_test):
			print "Acceptance test"

		self.curr_job = self.state.selection_function(self.curr_job)

		return (self.curr_job, self.processors[0])		
