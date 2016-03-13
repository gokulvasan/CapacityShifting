

class list_node:
	def __init__(self, data, nxt, prev):
		self.data = data
		self.nxt = nxt
		self.prev = prev
	def get_nxt(self):
		return self.nxt
	def get_prev(self):
		return self.prev
	def get_data(self):
		return self.data
	def set_prev(self, prev):
		self.prev = prev
	def set_nxt(self, nxt):
		self.nxt = nxt
	def set_data(self, data):
		self.data = data

class locallist:
	def __init__(self):
		print "Creating a new list"
		self.head=None

	def append(self, data):
		node = list_node(data, None, None)
		if self.head == None:
			print "List seems empty"
			self.head = node
			node.set_nxt(node)
			node.set_prev(node)
		else:
			print "Adding new data"
			node.set_nxt(self.head)
			head_prev = self.head.get_prev()
			self.head.set_prev(node)
			node.set_prev(head_prev)
			head_prev.set_nxt(node)
			#print "*****************"
			#print node.get_data()
			#print node.get_prev().get_data()
			#print node.get_nxt().get_data()	
			#print "****************"
	def insert(self, node, data):
		if node == None:
			print "Error: node data is empty"
			return None
		new = list_node(data, node.get_nxt(), node)
		node_nxt = node.get_nxt()
		node_nxt.set_prev(new)
		node.set_nxt(new)
		#print "******insert***********"
		#print new.get_data()
		#print new.get_prev().get_data()
		#print new.get_nxt().get_data()	
		#print "****************"
		return new

	def reverse(self):
		if self.head == None:
			print "Error: reverse on Empty List"
			return
		cur = self.head
		fut = None
		nxt = None
		prv = None
		while(fut != self.head):
			fut = cur.get_nxt()
			nxt = cur.get_nxt()
			prv = cur.get_prev()
			cur.set_nxt(prv)
			cur.set_prev(nxt)
			cur = fut
		self.head = self.head.get_nxt()

	def get_data(self, node):
		if self.head == None:
			print "Error: Empty List"
			return None
		if node == None:
			return self.head.get_data()
		return node.get_data()

	def go_nxt(self, node):
		if node == None:
			if self.head != None:
				return self.head
			else:
				print("Error: Empty List")
				return None
		return node.get_nxt()
	
	def go_prev(self, node):
		if node == self.head:
			return None
		if node == None:
			print("Error: Empty List")
		return node.get_prev()

	def get_head(self):
		return self.head

	def print_list(self):
		cur = self.head
		while 1:
			print "data {}".format(cur.get_data())
			#print "prev {}".format(cur.get_prev().get_data())
			#print "nxt {}".format(cur.get_nxt().get_data())
			cur = cur.get_nxt()
			if cur == self.head:
				break

"""
i = locallist()
i.append(1)
i.append(2)
i.append(3)
i.append(4)
n = i.go_nxt(None)
print i.get_data(n)
n = i.go_nxt(n)
print i.get_data(n)
n = i.go_nxt(n)
print i.get_data(n)
n = i.go_nxt(n)
print i.get_data(n)
i.insert(n,5)
n = i.go_nxt(n)
print i.get_data(n)
i.append(6)
print "****************"
i.print_list()
print "****************"
i.reverse()
i.print_list()
print "-----again reverse----"
i.reverse()
i.print_list()

# print i.get_data(None)
print "moving prev"
n = i.go_prev(n)
print i.get_data(n)
n = i.go_prev(n)
print i.get_data(n)
n = i.go_prev(n)
print i.get_data(n)
n = i.go_prev(n)
print i.get_data(n)
n = i.go_prev(n)
print i.get_data(n)
n = i.go_prev(n)
print i.get_data(n)
"""
