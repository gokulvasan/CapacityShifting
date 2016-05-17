from simso.core import Scheduler, Timer
from ss_state_machine import state
from ss_chronos import *
class SlotShifting(Scheduler):

	def init(self):
		self.interval = self.data
		self.curr_job = None
		print " IN SCHEDULER"
		#self.interval.print_def_interval()
		self.state = state()
		self.quantum = 5
		self.start = 0 

	def on_activate(self, job):
		""""
			1. Check the type of job
			2. if firm aperiodic move to unconcluded list
			3. if soft directly add to !guarantee list
			4. else to ready_list
			5. iterate job's list
		"""
		if self.start == 0:
			self.start = 1
			self.time  = chronos(self.sim,  
				self.processors[0],
				time_progress_continous(self.interval.nxt_decn))
				#time_progress_discrete(self.quantum))

		#print "job got activated {}".format(job.name)
		if job.task.data.set_current_job(job.task._job_count) > 0:
			print ">>>>>>>>>>>>>>>Activate job in list", job.name
			if self.state.add_job(job) < 0:
				print "Adding job Failed"
		else:
			print "job cannot be added as certainity ended"
		self.processors[0].resched()
	def on_terminated(self, job):
		print "::::::::::::job terminated {} c: {}".format(job.name, job.computation_time)
		self.state.rmv_job(job)
		self.processors[0].resched()

	def schedule(self, cpu):
		"""
			1. interval and slot update
			2. run acceptance and guarantee
			3. run EDF on ready_list
		"""
		if self.time.in_sched == 0:
			return self.curr_job
		print ">>>>>SCHED ", self.sim.now_ms()
		if self.curr_job:
			print("prev selected job : %s %.3f ms" % (self.curr_job.name, self.curr_job.computation_time))
			tsk = self.curr_job.task
		else:
			print "prev selected job is None"
			tsk = None
		#self.time.get_curr_time
		self.interval.update_intr(self.sim.now_ms(), tsk)

		while  self.state.transit_unconcluded_tsk(self.interval.aperiodic_test):
			print "Acceptance test"

		self.curr_job = self.state.selection_function()

		return (self.curr_job, self.processors[0])		
