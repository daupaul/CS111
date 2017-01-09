#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>

int ifd = 0;

struct directory_inode{
	int my_inode;
	int my_n_block;
	uint32_t my_i_block[15];
};

bool isPowerOfTwo(int n) {
    if(n == 1)
        return true;
    if(n == 0)
        return false;
    if(n % 2 != 0)
        return false;
    return isPowerOfTwo(n/2);
}

int helper(uint32_t* ptr, int value, FILE* output)
{
	if(ptr == 0)
		return 0;
	int counter = 0;
	int upper = 256;
	int n;
	for(n = 0; n < upper; n++)
	{
		uint32_t data = ptr[n];
		if(data == 0)
			break;
		fprintf(output, "%x,%d,%x\n", value, n , data);
		counter++;
	}
	return counter;
}

int main(int argc, char **argv) {
	if(argc > 2) //only one input is allowed
	{
		fprintf(stderr, "Error! More than one disk file./n");
		exit(1);
	}

	ifd = open(argv[1], O_RDONLY); //open input file
	if(ifd < 0)
	{
		fprintf(stderr, "Error! Unable to open file./n");
		exit(1);
	}

	//super.csv
	char* super_block = malloc(1024);
	pread(ifd, super_block, 1024, 1024);

	uint16_t* s_magic = (uint16_t*) (super_block + 56);
	if(*s_magic != 0xEF53) //check magic number
	{
		fprintf(stderr, "Superblock - invalid magic: 0x%x", *s_magic);
		exit(1);
	}
	uint32_t* s_inodes_count = (uint32_t*) (super_block);
	uint32_t* s_blocks_count = (uint32_t*) (super_block + 4);
	uint32_t* s_log_block_size = (uint32_t*)(super_block + 24);
	int block_size = 1024 << *s_log_block_size;
	if(!isPowerOfTwo(block_size) || !(block_size <= 64000 && block_size >= 512)) //check if block size is power of two between 512-64K
	{
		fprintf(stderr, "Superblock - invalid block size: %d", block_size);
		exit(1);
	}
	uint32_t* s_log_frag_size = (uint32_t*)(super_block + 28);
	int frag_size;
	if(*s_log_frag_size >= 0)
		frag_size = 1024 << *s_log_frag_size;
	else
		frag_size = 1024 >> -(*s_log_frag_size);
	uint32_t* s_blocks_per_group = (uint32_t*)(super_block + 32);
	uint32_t* s_inodes_per_group = (uint32_t*)(super_block + 40);
	uint32_t* s_frags_per_group = (uint32_t*)(super_block + 36);
	uint32_t* s_first_data_block = (uint32_t*)(super_block + 20);
	int ofd = creat("super.csv", 0666);
	dprintf(ofd, "%x,%d,%d,%d,%d,%d,%d,%d,%d\n", *s_magic, *s_inodes_count, *s_blocks_count, block_size, frag_size, *s_blocks_per_group, *s_inodes_per_group, *s_frags_per_group, *s_first_data_block);

	if((*s_blocks_count) % (*s_blocks_per_group)) //check if blocks per group evenly divide into total blocks
	{
		fprintf(stderr, "Superblock - %d blocks, %d blocks/group", *s_blocks_count, *s_blocks_per_group);
		exit(1);
	}
	int n_group = (*s_blocks_count) / (*s_blocks_per_group);
	int size = 32 * n_group;

	//group.csv
	char* group_descriptor = malloc(size);
	if(block_size == 1024)
		pread(ifd, group_descriptor, size, block_size * 2);
	else
		pread(ifd, group_descriptor, size, block_size);
	ofd = creat("group.csv", 0666);
	int block_bitmap[n_group];
	int inode_bitmap[n_group];
	int inode_table[n_group];
	int i;
	for(i = 0; i < n_group; i++)
	{
		uint16_t* bg_free_blocks_count = (uint16_t*)(group_descriptor + 12);
		uint16_t* bg_free_inodes_count = (uint16_t*)(group_descriptor + 14);
		uint16_t* bg_used_dirs_count = (uint16_t*)(group_descriptor + 16);
		uint32_t* bg_inode_bitmap = (uint32_t*)(group_descriptor + 4);
		uint32_t* bg_block_bitmap = (uint32_t*)(group_descriptor);
		uint32_t* bg_inode_table = (uint32_t*)(group_descriptor + 8);
		dprintf(ofd, "%d,%d,%d,%d,%x,%x,%x\n", *s_blocks_per_group, *bg_free_blocks_count, *bg_free_inodes_count, *bg_used_dirs_count, *bg_inode_bitmap, *bg_block_bitmap, *bg_inode_table);
		
		group_descriptor += 32;

		//save the value for future use
		block_bitmap[i] = (*bg_block_bitmap);
		inode_bitmap[i] = (*bg_inode_bitmap);
		inode_table[i] = (*bg_inode_table);
	}

	//bitmap.csv
	FILE* bitmap = fopen("bitmap.csv", "w+");
	int allocated_inode[(*s_inodes_count)];
	int s_inode = 0;
	int j;
	int k;
	for(i = 0; i < n_group; i++) //for each group
	{
		char* b_map = malloc(block_size);
		pread(ifd, b_map, block_size, block_bitmap[i]*block_size);
		for(j = 0; j < (*s_blocks_per_group)/8; j++) //for each byte in block 
		{
			for(k = 0; k < 8; k++) //for each bit in block
			{
				if(!(b_map[j] & (1 << k))) //if block is free
				{
					int temp = i*(*s_blocks_per_group) + j*8 + k + 1;
					fprintf(bitmap, "%x,%d\n", block_bitmap[i], temp);
				}
			}
		}

		char* i_map = malloc(block_size);
		pread(ifd, i_map, block_size, inode_bitmap[i]*block_size);
		for(j = 0; j < (*s_inodes_per_group)/8; j++) //for each byte in inode
		{
			for(k = 0; k < 8; k++) //for each bit in inode
			{
				int temp = i*(*s_inodes_per_group) + j*8 + k + 1;
				if((i_map[j] & (1 << k)) == 0) //if inode is free
				{
					fprintf(bitmap, "%x,%d\n", inode_bitmap[i], temp);
				}
				else
				{
					//save the value for future use
					allocated_inode[s_inode] = temp;
					s_inode++;
				}
			}
		}
	}

	//inode.csv
	FILE* inode = fopen("inode.csv", "w+");
	char* inode_table_temp = malloc(128);
	struct directory_inode my_directory[s_inode];
	int s_dir = 0;
	for(i = 0; i < s_inode; i++)
	{
		int number = (allocated_inode[i] - 1) / (*s_inodes_per_group); //calculate which group this inode belong
		int index = (allocated_inode[i] - 1) % (*s_inodes_per_group);
		pread(ifd, inode_table_temp, 128, inode_table[number]*block_size + index*128);

		char file_type = '?';
		uint16_t* i_mode = (uint16_t*) inode_table_temp;
		uint16_t* i_uid = (uint16_t*) (inode_table_temp + 2);
		uint16_t* i_gid = (uint16_t*) (inode_table_temp + 24);
		uint16_t* i_link_count = (uint16_t*) (inode_table_temp + 26);
		uint32_t* i_ctime = (uint32_t*) (inode_table_temp + 12);
	    uint32_t* i_mtime = (uint32_t*) (inode_table_temp + 16);
	    uint32_t* i_atime = (uint32_t*) (inode_table_temp + 8);
	    uint32_t* i_size = (uint32_t*) (inode_table_temp + 4);
	    uint32_t* i_blocks = (uint32_t*) (inode_table_temp + 28);
	    uint32_t* i_block = (uint32_t*) (inode_table_temp + 40);
	    if(0x4000 & (*i_mode))
	    	file_type = 'd';
	    else if(0x8000 & (*i_mode))
	    	file_type = 'f';
	    else if(0xA000 & (*i_mode))
	    	file_type = 's';
	    
	    if(file_type == 'd')
		{
			my_directory[s_dir].my_inode = allocated_inode[i];
			my_directory[s_dir].my_n_block = (*i_blocks)*512/block_size;
			for(j = 0; j < 15; j++)
				my_directory[s_dir].my_i_block[j] = *(i_block + j);
			s_dir++;
		}

	    fprintf(inode, "%d,%c,%o,%d,%d,%d,%x,%x,%x,%d,%d", allocated_inode[i], file_type, *i_mode, *i_uid, *i_gid, *i_link_count,*i_ctime, *i_mtime, *i_atime, *i_size, (*i_blocks)*512/block_size);
		for(j = 0; j < 15; j++)
		{
			fprintf(inode, ",%x", *i_block);
			i_block++;
			if(j == 14)
				fprintf(inode, "\n");
		}

	}

	//directory.csv
	FILE* directory = fopen("directory.csv", "w+");
    char* directory_entry = malloc(block_size);
    for(i = 0; i < s_dir; i++)
    {
        int counter = 0;
        for(j = 0; j < my_directory[i].my_n_block; j++)
        {
            uint16_t check = 0;
            while(check < block_size)
            {
                pread(ifd, directory_entry, block_size, my_directory[i].my_i_block[j]*block_size + check);
                uint32_t* inode = (uint32_t*) (directory_entry);
                uint16_t* rec_len = (uint16_t*) (directory_entry + 4);
                uint8_t* name_len = (uint8_t*) (directory_entry + 6);
                check += *rec_len;
                char name[(*name_len) + 1];
                for(k = 0; k < (*name_len); k++)
                    name[k] = *(directory_entry + 8 + k);
                name[k] = '\0';
                if(*inode != 0)
                    fprintf(directory, "%d,%d,%d,%d,%d,\"%s\"\n", my_directory[i].my_inode, counter, *rec_len, *name_len, *inode, name);
                counter++;
                if(*rec_len < 8 || *rec_len > 1024) //sanity check
                {	
                	fprintf(stderr, "Bad dirent: len = %d\n", *rec_len);
                	break;
                }
                if(*name_len > *rec_len)
                {
                	fprintf(stderr, "Bad dirent: len = %d, namelen = %d\n", *rec_len, *name_len);
                	break;
                }
            }
        }
    }

	//indirect.csv
	FILE* indirect = fopen("indirect.csv", "w+");
	uint32_t* indirect_block = malloc(block_size);
	uint32_t* d_indirect_block = malloc(block_size);
	uint32_t* t_indirect_block = malloc(block_size);
	for(i = 0; i < s_inode; i++)
	{
		int number = (allocated_inode[i] - 1) / (*s_inodes_per_group); //calculate which group this inode belong
		int index = (allocated_inode[i] - 1) % (*s_inodes_per_group);
		pread(ifd, inode_table_temp, 128, inode_table[number]*block_size + index*128);

		uint32_t* i_blocks = (uint32_t*) (inode_table_temp + 28);
	    uint32_t* i_block = (uint32_t*) (inode_table_temp + 40);
	    int n_block = *i_blocks * 512 / block_size;

	    if(n_block > 12)
	    {
	    	if(i_block[12] != 0) //single indirect block pointer
	    	{
	    		pread(ifd, indirect_block, block_size, i_block[12]*block_size);
	    		if(indirect_block != 0)
	    			helper(indirect_block, i_block[12], indirect);
	    	}
	    	if(i_block[13] != 0) //double indirect block pointer
	    	{
	    		pread(ifd, d_indirect_block, block_size, i_block[13]*block_size);
	    		if(d_indirect_block != 0)
	    		{
		    		int temp = helper(d_indirect_block, i_block[13], indirect);
		    		for(j = 0; j < temp; j++)
		    		{
		    			pread(ifd, indirect_block, block_size, d_indirect_block[j]*block_size);
		    			if(indirect_block != 0)
		    				helper(indirect_block, d_indirect_block[j], indirect);
		    		}
		    	}
	    	}
	    	if(i_block[14] != 0) //triple indirect block pointer
	    	{
	    		pread(ifd, t_indirect_block, block_size, i_block[14]*block_size);
	    		if(t_indirect_block != 0)
	    		{
	    			int a = helper(t_indirect_block, i_block[14], indirect);
	    			for(j = 0; j < a; j++)
	    			{
	    				pread(ifd, d_indirect_block, block_size, t_indirect_block[j]*block_size);
	    				if(d_indirect_block != 0)
			    		{
				    		int b = helper(d_indirect_block, i_block[14], indirect);
				    		for(k = 0; k < b; k++)
				    		{
				    			pread(ifd, indirect_block, block_size, d_indirect_block[k]*block_size);
				    			if(indirect_block != 0)
				    				helper(indirect_block, d_indirect_block[k], indirect);
				    		}
				    	}
	    			}
	    		}
	    	}
	    }
	}
	return 0;
}