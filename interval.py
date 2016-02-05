
from simso.core import Scheduler
from list import locallist
from ss_task import task_data

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
		self.sc += val
	def set_start(self, new_start):
		self.start = new_start
	def set_end(self, new_end):
		self.end = new_end

class interval(object):
	def __init__(self):
		self.curr_interval=None
		self.intr_count=0
		self.intr_list = locallist() 
		self.curr_point = None

	def add_interval(self, intr_id, start, end, sc):
		"""
		This makes linked list of interval.
		"""	
		intr_node = interval_node(intr_id, start, end, sc)
		self.intr_list.append(intr_node)
		self.intr_count += 1

	def goto_nxt_interval(self, data_type):
		"""
		This will move the curr_interval to nxt interval
		"""
		if self.curr_point == None:
			self.curr_point = self.intr_list.get_nxt(None)
		else:
			self.curr_point = self.intr_list.get_nxt(self.curr_point)
			if self.curr_point == self.intr_list.get_head():
				return None
		return self.intr_list.get_data(self.curr_point)

	def goto_prev_interval(self, data_type):
		"""
		Moves interval to previous interval from curr_point
		data_type: could be used to return list data or interval itself
		"""
		if self.curr_point == None:
			self.curr_point = self.intr_list.get_prev(None)
		else:
			self.curr_point = self.intr_list.get_prev(self.curr_point)
			if self.curr_point == self.intr_list.get_head():
				return None
		return self.intr_list.get_data(self.curr_point)
	
	@property
	def reset_iterator(self):
		"""
		Resets the iterator 
		"""
		self.curr_point = None

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

	def update_sc(self, task_data):
		"""
		Original update_sc
		"""
		tmp = task_data.curr_intr
		sc = self.curr_interval.sc
		self.curr_interval.set_sc(sc-1)
		while 1:
			if 2 == task_data.tsk_type: 		#softAper
				break
			intr = tmp.get_data()
			intr.set_sc(intr.sc()+1)
			if intr.id == self.curr_interval.id:	#task belongs to same interval
				break
			if intr.sc >= 0:			#task interval is +ve just leave
				break
			tmp = self.intr_list.go_prev(tmp)	#else keep iterating backwards

	def split_intr(self, intr, split_point):
		pass

	def Acceptance_test(self, Atask):
		pass

	def Guarantee_algo(self, Atask):
		pass

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

	"""
	This should be moved to association class.
	"""
	def create_relation_window(self):
		"""
		This will create realtion window
		1. Append deferred interval data
		2. create references of relation window
		"""
		if self.intr_list.get_data(None) == None:
			print "No intervals still defined"
		print self.intr_count
		pass

	def update_sc(self, curr_intr, task):
		"""
		Simply update slots, O(1) 
		"""
		pass

	def set_curr_interval(self):
		"""
		This is where defereed update happens
		"""
		pass
"""
i = deferred_interval()
i.add_interval(1, 1, 1, 1)
i.add_interval(2,2,3,1)

i.create_relation_window()
"""
