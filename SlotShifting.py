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
		quantum = self.sim.cycles_per_ms * 5 
		self.time  = chronos(self.sim,  self.processors[0], time_progress_discrete(quantum))

	def on_activate(self, job):
		""""
			1. Check the type of job
			2. if firm aperiodic move to unconcluded list
			3. if soft directly add to !guarantee list
			4. else to ready_list
			5. iterate job's list
		"""
		#print "job got activated {}".format(job.name)
		if job.task.data.set_current_job(job.task._job_count) > 0:
			#print "Active job in list"
			if self.state.add_job(job) < 0:
				print "Adding job Failed"
		else:
			print "job cannot be added as certainity ended"

	def on_terminated(self, job):
		#print "job terminated {}".format(job.name)
		self.state.rmv_job(job)

	def schedule(self, cpu):
		"""
			1. interval and slot update
			2. run acceptance and guarantee
			3. run EDF on ready_list
		"""
		if self.time.in_sched == 0:
			return None

		while  self.state.transit_unconcluded_tsk(self.interval.aperiodic_test):
			print "Acceptance test"

		self.curr_job = self.state.selection_function()
		if self.curr_job:
			print("selected job : %s %.3f ms" % (self.curr_job.name, self.curr_job.computation_time))
			tsk = self.curr_job.task
		else:
			print "selected job is None"
			tsk = None

		self.interval.update_intr(self.time.get_curr_time, tsk)

		return (self.curr_job, self.processors[0])		
