
from simso.core import Scheduler
import ss_list
from ss_task import task_data, tsk_type
from ss_task import task_intr_association
from ss_state_machine import tsk_state

class deferred_intr_node:
	def __init__(self, deferred_update, interval_node):
		self.def_update = deferred_update
		self.interval = interval_node

	@property
	def id(self):
		return self.interval.intr_id
	@property
	def start(self):
		return self.interval.start
	@property
	def end(self):
		return self.interval.end
	@property
	def sc(self):
		return self.interval.sc
	def set_sc(self, sc):
		self.interval.sc = sc
	def set_start(self, start):
		self.interval.start = start
	def set_end(self,end):
		self.interval.end = end

	@property
	def update_val(self):
		return self.def_update.update_val
	@property
	def rec_val(self):
		return self.def_update.rec_val
	@property
	def lent_till(self):
		return self.def_update.lent_till
	@property
	def lender(self):
		return self.def_update.lender	
	def set_update_val(self, val):
		self.def_update.update_val = val
	def set_rec_Val(self, val):
		self.def_update.rec_val = val
	def set_lent_till(self, lent_till):
		self.def_update.lent_till = lent_till
	def set_lender(self, lender):
		self.def_update.lender = lender

class deferred_update:
	def __init__(self):
		self.update_val = 0
		self.rec_val = 0
		self.lent_till = None
		self.lender = None

class interval_node:
	def __init__(self, intr_id, start, end, sc):
		self.intr_id = intr_id
		self.start = start
		self.end = end
		self.sc = sc
	@property
	def id(self):
		return self.intr_id
	@property
	def start(self):
		return self.start
	@property
	def end(self):
		return self.end
	@property
	def sc(self):
		return self.sc
	def set_sc(self, val):
		self.sc = val
	def set_start(self, new_start):
		self.start = new_start
	def set_end(self, new_end):
		self.end = new_end

class interval(object):
	def __init__(self):
		self.curr_interval=None
		self.intr_count=0
		self.intr_list = ss_list.locallist() 
		self.curr_point = None		#works like generator 

	def new_intr_append(self, intr_id, start, end, sc):
		"""
		This makes linked list of interval.
		"""	
		intr_node = interval_node(intr_id, start, end, sc)
		self.intr_list.append(intr_node)
		self.intr_count += 1

	def new_intr_insert(self, intr_id, start, end, sc, node):
		intr_node = interval_node(intr_id, start, end, sc)
		list_node = self.intr_list.insert(node, intr_node)
		self.intr_count += 1
		return 	list_node

	def goto_nxt_interval(self, data_type):
		"""
		This will move the curr_interval to nxt interval
		"""
		if self.curr_point == None:
			self.curr_point = self.intr_list.go_nxt(None)
		else:
			self.curr_point = self.intr_list.go_nxt(self.curr_point)
			if self.curr_point == self.intr_list.get_head():
				return None
		if data_type == None:
			return self.intr_list.get_data(self.curr_point)
		elif data_type == 1:
			return self.curr_point

	def goto_prev_interval(self, data_type):
		"""
		Moves interval to previous interval from curr_point
		data_type: could be used to return list data or interval itself
		"""
		if self.curr_point == self.intr_list.get_head():
			return None

		if self.curr_point == None:
			self.curr_point = self.intr_list.go_prev(None)
		else:
			self.curr_point = self.intr_list.go_prev(self.curr_point)

		if data_type == None:
			return self.intr_list.get_data(self.curr_point)
		elif data_type == 1:
			return self.curr_point

	def reset_iterator(self):
		"""
		Resets the iterator 
		"""
		self.curr_point = None

	def set_iterator(self, iter_point):
		self.curr_point = iter_point

	def get_curr_iterator(self):
		return self.curr_point

	def set_curr_interval(self, intr):
		"""
		Sets intr as current interval
		i/p : interval_node to set.
		"""
		self.curr_interval = intr

	def get_curr_interval(self):
		return self.intr_list.get_data(self.curr_interval)

	def get_intr_count(self):
		return self.intr_count

	def intr_reverse(self):
		self.intr_list.reverse()

	def intr_list(self):
		return self.intr_list
	
	def check_set_curr_interval(self, time_progressed):
		if time_progressed >= self.curr_interval.end:
			curr_intr = self.goto_nxt_interval(None)
			if None == curr_intr:
				return None
			self.set_curr_interval(curr_intr)
		return 0

	def update_intr(self, time_progressed, task): 
		"""
		Here time progress is in slot, but
		I should make this work generically, i.e.
		when notion of slot is removed then still it 
		should work in next level
		WHAT SHLD THIS DO:
			1. based on time progressed; 
				progress the current interval
			2. update_sc
		"""
		if 0 != self.check_set_curr_interval(time_progressed):
			return None

		if -1 == self.update_sc(task.data):
			return None
		return 1
	def update_sc(self, task_data):
		"""
		Original update_sc
		"""
		tmp = task_data.curr_intr
		sc = self.curr_interval.sc
		self.curr_interval.set_sc(sc-1)
		while 1:
			if tsk_type.soft_aperiodic.value == task_data.tsk_type:  #softAper
				break
			intr = tmp.get_data()
			intr.set_sc(intr.sc + 1)
			if intr.id == self.curr_interval.id:	#task belongs to same interval
				break
			if intr.sc >= 0:			#task intr is +ve just leave
				break
			tmp = self.intr_list.go_prev(tmp)	#else keep iterating backwards

	def aperiodic_test(self, Atask,time_progressed):
		curr_iter_point = self.get_curr_iterator()
		ret = tsk_state.not_guarantee.value

		wcet_time = Atask.wcet
		wcet_slot = wcet_time / quantum_notion
		dl_time = Atask.dl
		dl_slot = dl_time / quantum_notion

		if self.acceptance_test(wcet_slot, dl_slot):
			guarantee_task(Atask, wcet_slot, dl_slot, time_progressed)
			ret = tsk_state.guarantee.value

		self.set_iterator(curr_iter_point)
		return ret

	def acceptance_test(self,wcet_slot, dl_slot):

		intr = self.curr_interval
		sum = 0
		while 1:
			if intr.end < dl_slot:
				break
			sum += intr.sc
			intr = self.goto_nxt_interval(None)

		dl_sc = min(intr.sc, (dl_slot - intr.start) )
		sum += max(0, dl_sc)

		if sum >= wcet_slot:
			return 1
		else:
			return 0

	def split_intr(self, intr_right, split_point, time_progressed):
		"""
			# here do affilation in aperiodic task.i.e.
			# association 
		"""
		new_intr_id = self.intr_count + 1
		new_intr_end = split_point

		if intr_right == self.get_curr_interval():
			new_intr_start = time_progresed
		else:	
			new_intr_start = intr_right.start

		new_intr_sc = (split_point - new_intr_start) + 1
		
		intr_right.start = split_point + 1
		intr_right.sc = intr.sc - new_intr_sc
		new_intr_sc = new_intr_sc - min(0, intr_right.sc)
		
		curr_point = self.get_curr_iterator()
		insert_node = self.intr_list.go_prev(curr_point)
		
		intr_left = new_intr_insert(new_intr_id, 
				new_intr_start,
				new_intr_end,
				new_intr_sc,
				insert_node)
		return intr_left

	def guarantee_task(self, Atask, wcet_slot, dl_slot, time_progressed):
		
		"""
		This goes with the assumption that acceptance
		test has already moved curr_iterator to interval
		where Atask will fall.
		"""
		intr_left = None
		intr = self.get_curr_iterator()
		intr_right = self.intr_list.get_data(intr)
		if intr_right.end > dl_slot:
			intr_left = self.split_intr(intr_right,
						 dl_slot, time_progressed)
			intr_left_data = self.intr_list.get_data(intr_left)

			task_intr_asco = task_intr_association()
			task_intr_asco.append_node(1, intr_left, intr_left_data.id)
			tsk_data = task_data(tsk_intr_asco, 1, 1)
			Atask._task_info.data = tsk_data #This needs better abstraction
			self.set_curr_iterator(intr_left)

		delta = wcet_slot
		if intr_left != None:
			Intr_iterator = intr_left_data
		else:
			intr_iterator = intr_right

		while delta:
			if intr_iterator.sc > 0:
				if intr_iterator.sc >= delta:
					intr_iterator.sc = intr_iterator.sc - delta
					delta = 0
				else:
					delta = delta - intr_iterator.sc
					intr_iterator.sc = -delta
			else:
				intr_iterator.sc += -delta
			
			intr_iterator = goto_prev_interval(None)

	def print_intr_list(self):
		i = 0
		node = None
		while i < self.intr_count:
			node = self.intr_list.go_nxt(node)
			data = self.intr_list.get_data(node)
			self.__print_intr(data)
			i += 1

	def __print_intr(self, node):
		print "==========="
		print "intr_id{}".format(node.id)
		print "start {}".format(node.start)
		print "end {}".format(node.end)
		print "SC {}".format(node.sc)
		

"""
	1. Create Relation Window
	2. method to update the values : rec_val, update_val
	3. iteration function that does the deferred update.
	Basically I will try to use composition method to add 
	additional data to existing data.
"""
class deferred_interval(interval):
	def __init__(self):
		super(deferred_interval, self).__init__()
		print "creating deferred interval"
		
	def update_sc(self, time_progressed , task):
		"""
		Simply update slots, O(1) 
		"""
		task_data = task.data
		intr_task = task_data.curr_intr
		if None == intr_task:
			print "Error: task intr association failure"
			return -1
		intr_task_data = intr_task.get_data()
		if  self.curr_interval == intr_task_data:
			return 0

		if intr_task_data.sc < 0:
			intr_task_data.set_update_val(intr_task_data.update_val + 1)
 
		intr_task_data.set_sc(intr_task_data.sc + 1)
		if self.curr_interval.lender == intr_task_data.lender:
			return 0

		self.curr_interval.set_sc(self.curr_interval.sc - 1)
		return 0
	def deferred_update(self, curr_intr):
		"""
			1. iterate backwards and update all SC till we reach
			the interval which will be the current interval.
			2. along update REC val
			3. return SC of the current interval.
		"""
		if None == curr_intr.lent_till:
			return None
		if None == curr_intr.lender:
			print "Error: lender is empty, lent-till exists"
			return -1

		ith_intr = curr_intr.lent_till
		ith_intr_data = ith_intr.get_data()
		self.set_iterator(ith_intr)
		rec_val = 0
		while 1:
			update_val += ith_intr_data.update_val
			ith_intr_data.set_update_val(0)
			ith_intr_data = goto_prev_interval(None)
			ith_intr_data.sc = (ith_intr_data.sc - rec_val) 
			if ith_intr_data == curr_data:
				break
			ith_intr_data.sc += update_val
			rec_val += max(0, ith_intr_data.sc)
		return 1
 
	def check_set_curr_interval(self, time_progressed):
		"""
		this should be a atomic function which does the following
		in sequence:
			2. do deferred update
			3. set the current interval.
		"""
		if time_progressed >= self.curr_interval.end:
			if -1 == self.deferred_update(self.curr_interval):
				return -1
			curr_intr = self.goto_nxt_interval(None)
			if None == curr_intr:
				return None

			self.set_curr_interval(curr_intr)
		return 0

	def print_def_interval(self):
		i = 0
		node = None
		while i < self.intr_count:
			node = self.intr_list.go_nxt(node)
			data = self.intr_list.get_data(node)
			self.__print_intr(data)
			i += 1

	def __print_intr(self, node):
		print "======{}=====".format(node)
		print "intr_id{}".format(node.id)
		print "start {}".format(node.start)
		print "end {}".format(node.end)
		print "SC {}".format(node.sc)
		print "update_val {}".format(node.update_val)
		print "rec_Val {}".format(node.rec_val)
		if node.lent_till != None:
			l_till = node.lent_till
			data = l_till.get_data()
			print "lent_till {}".format(data.id)
		else:
			print "lent_till {}".format(None)
		if node.lender != None:
			lender = node.lender
			data = lender.get_data()
			print "lender {}".format(data.id)
		else:
			print "lender {}".format(None)
"""
i = interval()
i.new_intr_append(1, 1, 1, 1)
"""

