import socket
import sys
import os
import struct
import binascii
from interval import * 
from simso.configuration import Configuration

server_addr = "/tmp/ss_parser"
class local_table:
	def __init__(self, tsk_cnt, tsk_lst, intr_cnt, intr_lst):
		self.intr_cnt = intr_cnt
		self.tsk_cnt = tsk_cnt
		self.tsk = tsk_lst
		self.intr = intr_lst

class reciever:
	def __init__(self, serv_addr):
		self.__sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		try:
			self.__sock.connect(serv_addr)
		except socket.error, msg:
			return None 
			print >>sys.stderr, msg
		self.intr_cnt = 0
		self.tsk_cnt = 0
		self.tsk = []
		self.intr = []

	def __recv_hdr(self):
		hdr = self.__recv("I I")
		self.intr_cnt = hdr[0]
		self.tsk_cnt = hdr[1]
		print "intr{} task{}".format(self.intr_cnt, self.tsk_cnt)

	def __recv_tsk(self):
		i = 0
		tsk_hdr_fmt = "I I"
		tsk_const_data_fmt = 'I' * 6 
		tsk_var_data_fmt = "I"
		while(i < self.tsk_cnt):
			hdr_data = self.__recv(tsk_hdr_fmt)
			tsk_fmt = self.__createUnpackString(tsk_const_data_fmt,
						tsk_var_data_fmt,
						hdr_data[1])
			tsk_node = self.__recv(tsk_fmt)
			self.tsk.append(tsk_node)
			i = i + 1
		print "so recieved task {}".format(i)

	def __recv_intr(self):
		i = 0
		intr_fmt = "I I I i I"
		while(i < self.intr_cnt):
			intr_node = self.__recv(intr_fmt)
			self.intr.append(intr_node)
			i = i + 1

	def __recv(self, fmt):
		data = self.__sock.recv(256)
		r = struct.pack('I', len(data))
		self.__sock.send(r)

		s = struct.Struct(fmt)
		upk = s.unpack(data)
		return upk

	def __createUnpackString(self, const_data_fmt, var_fmt, count):
		return const_data_fmt + (var_fmt * count)

	def recieve_table(self):
		self.__recv_hdr()
		self.__recv_tsk()
		self.__recv_intr()

"""
1. create intervals.
2. create tasks with reference of intervals.
"""
class association:
	def __init__(self, reciever):
		self.rcvr = reciever
		self.intr_idx = 0
		self.tsk_idx = 0
	"""
	converts recived data into task
	Approach: 
		1. make task_intr_association
		2. make task_data
		3. make task
	"""
	@staticmethod
	def create_tsk(self, intr_list, conf):
		if self.tsk_idx >= self.rcvr.tsk_cnt:
			#print "task count: {}".format(self.tsk_idx)
			return None
		tsk = self.rcvr.tsk[self.tsk_idx]
		self.tsk_idx += 1

		tskid = tsk[0]
		tskest = tsk[1]
		tskwcet = tsk[2] * 5
		tskdl = tsk[3] * 5
		tskperiod = tsk[4] * 5
		tskintrcnt = tsk[5]
		print " ====== TASK id{} est{} wcet{} dl{} prd{} =====".format(tskid, tskest, tskwcet, tskdl, tskperiod)
		tsk_intr_asco = task_intr_association()
		i = self.__task_intr_asco(intr_list, tsk_intr_asco,
						tskintrcnt, tsk[6:])
		if i != 0:
			return -1
		tsk_data = task_data(tsk_intr_asco, tskintrcnt, 0)
		tsk_data.print_data()
		conf.add_task(name="TP"+str(tskid), identifier=tskid,
				task_type="Periodic",period=tskperiod, 
				activation_date=tskest,wcet=tskwcet, 
				deadline=tskdl, data = tsk_data
				)
		return 0

	def __get_intrfrm_id(self, intr, intr_id):
		intr_list = intr.intr_list
		i  = intr_list.go_nxt(None)
		curr_cnt = 0
		intr_cnt = intr.get_intr_count()
		while curr_cnt < intr_cnt:
			data = intr_list.get_data(i)
			#print "curr intr {} Expected {}".format( data.id, intr_id)
			if data.id == intr_id:
				return i
			i = intr_list.go_nxt(i)
			curr_cnt += 1
		print "Error: No interval with {} found".format(intr_id)
		return None

	def __task_intr_asco(self, intr, tsk_intr_lst, tskintrcnt, intr_asco):
		i = 1
		for a in intr_asco:
			intr_node = self.__get_intrfrm_id(intr, a)
			if intr_node == None:
				print "Error: Abort System"
				return -1
			tsk_intr_lst.append_node(i, intr_node, a)
			i += 1
		return 0

	@staticmethod
	def __create_deferred_intr_list(self, interval):
		i = 0
		interval.reset_iterator()	
		while i < interval.intr_count:
			deferred_node = deferred_update()
			list_node = interval.goto_nxt_interval(1)
			interval_node = list_node.get_data() 
			data = deferred_intr_node(
				deferred_node, interval_node)
			list_node.set_data(data)
			i += 1

	def ___create_relation(self, interval, intr_count, lent_node):
		i = 0
		while (i + intr_count) < interval.intr_count:
			node = interval.goto_nxt_interval(1)
			data = node.get_data()
			#print "==========intr id {}".format(data.id)
			if data.sc >= 0:
				print "found a intr >= 0"
				break
			else:
				lent_till = node
				i += 1
		print "=======================LENDER:{} LENT TILL:{} count{}".format(lent_node.get_data().id, lent_till.get_data().id, i+intr_count), i
		if (i + intr_count) >= interval.intr_count:
			data = lent_till.get_data()
		else:
			data = interval.goto_prev_interval(None)

		while 1:
			data.set_lent_till(lent_till)
			d = data.lent_till
			#dt = d.get_data() 
			#print "lent_till {}".format(dt.id)
			data.set_lender(lent_node)
			if data.sc >= 0:
				break
			data = interval.goto_prev_interval(None)
			if data == None:
				print "moved to head"
				return
		interval.set_iterator(lent_till)
		return i

	def __create_interval_relation(self, interval):
		i = 0
		interval.reset_iterator()
		print "INTR COUNT: ", interval.intr_count
		while i < interval.intr_count:
			node = interval.goto_nxt_interval(1)
			data = node.get_data()
			i += 1
			#print "curr intr iteration:", data.id, i
			#if i >= interval.intr_count:
			#	break
			if data.sc < 0:
				print "Error: relation window creation", data.id
				return -1
 
			node1 = interval.goto_nxt_interval(1)
			if node1 == None:
				print " Next Node is empty"
				return 0
			data1 = node1.get_data()
			print "NODE 1: ", data1.id
			if data1.sc < 0:
				#interval.set_iterator(node1)
				node1 = interval.goto_prev_interval(1)
				if node1 == None:
					return 0
				i += self.___create_relation(
					interval, i, node)
				print " RELATION WINDOW CREATED"
			else:
				node1 = interval.goto_prev_interval(1)
		interval.reset_iterator()
		return 0

	@staticmethod
	def create_relation_window(self, interval):
		"""
		This will create realtion window
			1. create deferred_intr_node for each interval
			2. then create relation window
			3. complexity: n^2 + R, but considered offline phase
		"""
		self.__create_deferred_intr_list(self, interval)
		if self.__create_interval_relation(interval) < 0:
			exit(1)
		# here check for the correctness
		print "============printing deferred interval============="
		interval.print_def_interval()
		print "==========setting CURRENT INTR"
		curr_intr = interval.goto_nxt_interval(1)
		interval.set_curr_interval(curr_intr.get_data())
		interval.set_iterator(curr_intr)	
	"""
	Creates deferred_interval object 
	"""
	@staticmethod
	def create_def_intr_list(self):
		i = deferred_interval()
		while self.intr_idx < self.rcvr.intr_cnt:
			intr = self.rcvr.intr[self.intr_idx]
			i.new_intr_append(intr[0],intr[1],intr[2],intr[3])
			self.intr_idx += 1
		i.print_intr_list()
		#print "=============NOW REVERSING==========="
		#i.intr_reverse()
		i.print_intr_list()

		return i

"""
class client:
	def __init__(self):
		sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		print "connecting to server %s" % server_addr
		try:
			sock.connect(server_addr)
			print "Connection is a Success"
		except socket.error, msg:
			print >>sys.stderr, msg
			sys.exit(1)
		data = sock.recv(256)
		d = struct.pack("I", len(data))
		sock.send(d)

		s = struct.Struct('I I')
		unpacked_data = s.unpack(data)
		del s
		print " recieved some data :", unpacked_data
		print "recieved data : ", unpacked_data[0]
		i = 0
		
		# Checking jobs
		while (i < unpacked_data[1]):
			data = sock.recv(200)
			s = struct.Struct('I I')
			u = s.unpack(data)
			print "recieved intr data {}, {}".format(len(data), u)
			j = len(data)
			d = struct.pack("I", j)
			sock.send(d)
			data = sock.recv(200)
			print "recieved intr data {}".format(len(data))
			j = len(data)
			d = struct.pack("I", j)
			sock.send(d)
			sg = self.__createUnpackString('I'*6, 'I', u[1])
			s = struct.Struct(sg)
			u = s.unpack(data)
			print u
			i = i + 1
		i = 0
		while (i < unpacked_data[0]):
			data = sock.recv(200)
			print "recieved intr data {}".format(len(data))
			d = struct.pack("I", len(data))
			sock.send(d)
			i = i + 1
	def __createUnpackString(self, string, fmt, count):
		return string + (fmt * count)

"""
"""
r = reciever(server_addr)
r.recieve_table()
a = association(r)
intr = a.create_intr_list(a)
intr.print_intr_list()

i = 0
c = Configuration()
while 1:
	j = a.create_tsk(a, intr, c)
	if j != 0:
		break
	
	print "=================task created==========="
	i += 1

print "total tasks {}".format(i)
"""
