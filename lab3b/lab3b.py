import csv

class inode:
	def __init__(self):
		self.n_inode = 0
		self.ref_list = []
		self.n_links = 0
		self.ptrs = []

class block:
	def __init__(self):
		self.n_block = 0
		self.ref_list = []



def main():
	ofd = open("lab3b_check.txt", "w")

	s_inodes_count = 0
	s_blocks_count = 0
	block_size = 0
	s_inodes_per_group = 0

	inode_bitmap = []
	block_bitmap = []

	free_inode = []
	free_block = []

	allocated_inode = {} #map<inode#, inode>
	allocated_block = {} #map<block#, block>

	indirect_map = {} #map<(block#, entry#), block ptr>
	directory_map = {} #map<child/entry inode#, parentdir inode#>

	with open("super.csv", "r") as file:
	    for row in csv.reader(file):
	      s_inodes_count = int(row[1])
	      s_blocks_count = int(row[2])
	      block_size = int(row[3])
	      s_inodes_per_group = int(row[6])

	with open("group.csv", "r") as file:
	    for row in csv.reader(file):
	      inode_bitmap.append(int(row[4], 16))
	      block_bitmap.append(int(row[5], 16))

	with open("bitmap.csv", "r") as file:
	    for row in csv.reader(file):
	    	if int(row[0], 16) in inode_bitmap:
	    		free_inode.append(int(row[1]))
	    	elif int(row[0], 16) in block_bitmap:
	    		free_block.append(int(row[1]))

	with open("indirect.csv", "r") as file:
	    for row in csv.reader(file):
	      indirect_map[(int(row[0], 16), int(row[1]))] = int(row[2], 16) #map<(block#, entry#), block ptr>

	with open("inode.csv", "r") as file:
	    for row in csv.reader(file):
			t_inode = inode()
			t_inode.n_inode = int(row[0])
			t_inode.n_links = int(row[5])
			for i in range(11, 26):
				if int(row[i], 16) != 0:
					t_inode.ptrs.append(int(row[i], 16))
			allocated_inode[int(row[0])] = t_inode #map<inode#, inode>

			#INVALID_BLOCK
			if int(row[10]) <= 12: #number of blocks <= 12
				for i in range(int(row[10])): #update_block
					t_n_block = int(row[i+11], 16)
					if (t_n_block == 0) or (t_n_block >= s_blocks_count):
						ofd.write("INVALID BLOCK < {0} > IN INODE < {1} > ENTRY < {2} >\n".format(t_n_block, int(row[0]), i))
					elif (t_n_block in allocated_block):
						allocated_block[t_n_block].ref_list.append((int(row[0]), None, i))
					else:
						t_block = block()
						t_block.n_block = t_n_block
						t_block.ref_list.append((int(row[0]), None, i))
						allocated_block[t_n_block] = t_block
			else:
				#examine indirect block
				for i in range(12):
					x = int(row[i+11], 16)
					if (x == 0) or (x >= s_blocks_count):
						ofd.write("INVALID BLOCK < {0} > IN INODE < {1} > ENTRY < {2} >\n".format(t_n_block, int(row[0]), i))
					else:
						if (x in indirect_map): #update_block
							n_entry = indirect_map[x]
							if (x in allocated_block):
								allocated_block[x].ref_list.append((int(row[0]), x, n_entry))
							else:
								t_block = block()
								t_block.n_block = x
								t_block.ref_list.append((int(row[0]), x, n_entry))
								allocated_block[x] = t_block
						#else:
							#ofd.write("INVALID BLOCK < {0} > IN INODE < {1} > ENTRY < {2} >\n".format(t_n_block, int(row[0]), i))
	
	with open("directory.csv", "r") as file:
		for row in csv.reader(file):
			p_inode = int(row[0])
			c_inode = int(row[4])
			n_entry = int(row[1])
			if (p_inode != c_inode) or (p_inode == 2):
				directory_map[c_inode] = p_inode
			if (c_inode in allocated_inode):
				allocated_inode[c_inode].ref_list.append((p_inode, n_entry))
			else:
				ofd.write("UNALLOCATED INODE < {0} > REFERENCED BY DIRECTORY < {1} > ENTRY < {2} >\n".format(c_inode, p_inode, n_entry))

			if (n_entry == 0) and (c_inode != p_inode):
				ofd.write("INCORRECT ENTRY IN < {0} > NAME < {1} > LINK TO < {2} > SHOULD BE < {3} >\n".format(p_inode, row[5], c_inode, p_inode))
			elif (n_entry == 1) and (c_inode != directory_map[p_inode]):
				ofd.write("INCORRECT ENTRY IN < {0} > NAME < {1} > LINK TO < {2} > SHOULD BE < {3} >\n".format(p_inode, row[5], c_inode, directory_map[p_inode]))

	for t_inode in allocated_inode:
		a = allocated_inode[t_inode]
		size = len(a.ref_list)
		if (a.n_inode > 10) and (size == 0):
			index = t_inode / s_inodes_per_group
			ofd.write("MISSING INODE < {0} > SHOULD BE IN FREE LIST < {1} >\n".format(a.n_inode, inode_bitmap[index]))
		elif (a.n_links != size):
			ofd.write("LINKCOUNT < {0} > IS < {1} > SHOULD BE < {2} >\n".format(t_inode, a.n_links, size))
	
	for x in range(11, s_inodes_count + 1):
		if (x not in free_inode) and (x not in allocated_inode):
			index = t_inode / s_inodes_per_group
			ofd.write("MISSING INODE < {0} > SHOULD BE IN FREE LIST < {1} >\n".format(x, inode_bitmap[index]))

	for x in allocated_block:
		t_block = allocated_block[x]
		size = len(t_block.ref_list)
		if (size > 1):
			ofd.write("MULTIPLY REFERENCED BLOCK < " + str(t_block.n_block) + " > BY")
			for i in range(size):
				ofd.write(" INODE < {0} > ENTRY < {1} >".format(t_block.ref_list[i][0], t_block.ref_list[i][2]))
			ofd.write('\n')

	for x in free_block:
		if x in allocated_block:
			if allocated_block[x].ref_list[0][1] == None:
				ofd.write("UNALLOCATED BLOCK < {0} > REFERENCED BY INODE < {1} > ENTRY < {2} >\n".format(x, allocated_block[x].ref_list[0][0], allocated_block[x].ref_list[0][2]))
			else:
				ofd.write("UNALLOCATED BLOCK < {0} > REFERENCED BY INODE < {1} > ENTRY < {2} > INDIRECT BLOCK < {3} >\n".format(x, allocated_block[x].ref_list[0][0], allocated_block[x].ref_list[0][2], allocated_block[x].ref_list[0][1]))    
            
if __name__ == "__main__":
	main()