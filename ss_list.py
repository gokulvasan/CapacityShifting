

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
			node.set_nxt(self.head)
			node.set_prev(self.head)
		else:
			print "Adding new data"
			node.set_nxt(self.head)
			node.set_prev(self.head.get_prev)
			prev = self.head.get_prev()
			prev.set_nxt(node)
			self.head.set_prev(node)
			

	def insert(self, node, data):
		if node == None:
			print "Error: node data is empty"
			return None
		node.set_nxt( list_node(data, node, node.get_nxt()) )

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
		if node == None:
			if self.head != None:
				return self.head
			else:
				print("Error: Empty List")
		return node.get_prev()
	def get_head(self):
		return self.head
"""
i = locallist()
i.append(1)
i.append(2)
i.append(3)
n = i.go_nxt(None)
print i.get_data(n)
n = i.go_nxt(n)
print i.get_data(n)
i.insert(n,5)
n = i.go_nxt(n)
print i.get_data(n)
# print i.get_data(None)
n = i.go_nxt(n)
print i.get_data(n)
"""
