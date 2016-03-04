
from simso.core import Scheduler
import ss_list
from ss_task import task_data
from ss_task import task_intr_association

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
		self.intr_list.insert(node, intr_node)
		self.intr_count += 1
	
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
	def intr_reverse(self):
		self.intr_list.reverse()

	def split_intr(self, intr, split_point):
		pass

	def Acceptance_test(self, Atask):
		pass

	def Guarantee_algo(self, Atask):
		pass
	
	def intr_list(self):
		return self.intr_list
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
	def print_def_interval(self):
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
		print "update_val {}".format(node.update_val)
		print "rec_Val {}".format(node.rec_val)
		print "lent_till {}".format(node.lent_till)
		print "lender {}".format(node.lender)

"""
i = interval()
i.new_intr_append(1, 1, 1, 1)
"""

