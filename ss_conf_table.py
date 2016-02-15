import socket
import sys
import os
import struct
import binascii

server_addr = "/tmp/ss_parser"

class reciever:
	def __init__(self, serv_addr):
		self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		try:
			self.__sock.connect(serv_addr)
		except socket.error, msg:
			print >>sys.stderr, msg
			sys.exit(1) 
		self.__intr_count = 0
		self.__task_count = 0
		self.__tsk = []
		self.__intr = []

	def recv_hdr(self):
		hdr = self.__recv("I I")
		self.__intr_count = hdr[0]
		self.__intr_count = hdr[1]

	def recv_tsk(self):
		i = 0
		tsk_hdr_fmt = "I I"
		tsk_const_data_fmt = 'I' * 6
		tsk_var_data_fmt = "I"
		while(i < self.__task_count):
			hdr_data = self.__recv(tsk_hdr_fmt)
			tsk_fmt = self.__createUnpackString(tsk_const_data_fmt, 
								tsk_var_data_fmt, hdr_data[1])
			tsk_node = self.recv(tsk_fmt)
			self.__tsk.append(tsk_node)
			i = i + 1

	def recv_intr(self):
		i = 0
		intr_fmt = "I I I i I"
		while(i < self.__intr_count):
			intr_node = self.__recv(intr_fmt)
			self.__intr.append(intr_node)
			i = i + 1

	def create_tsk(self):
		pass	
	def create_intr(self):
		pass
	def __recv(self, fmt):
		data = self.__sock.recv(256)
		r = struct.pack('I', len(data))
		self.__sock.send(r)

		s = struct.Struct(fmt)
		upk = s.unpack(data)
		return upk

	def __task_intr_asco(self):
		pass
	def __createUnpackString(self, const_data_fmt, var_fmt, count):
		return const_data_fmt + (var_fmt * count)
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

