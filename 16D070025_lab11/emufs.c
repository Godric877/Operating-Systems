#include "emufs.h"

/**************************** File system helper functions ****************************/

//	Function to encrypt a block of data 
int encrypt(char* input, char* encrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		encrypted[i] = ~input[i];
	}
}

//	Function to decrypt a block of data 
int decrypt(char* input, char* decrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		decrypted[i] = ~input[i];
	}
}

//	The following helper functions are used to read and write 
//	superblock and metadata block. 
//	Change the function definitions with required arguments
struct superblock_t* readSuperblock(struct mount_t* mountpoint)
{
	/*
		* Read 0th block from the device into a blocksize buffer
		* Create superblock_t variable and fill it using reader buffer
		* Return the superblock_t variable
	*/
	int device_fd = mountpoint->device_fd;
	char buffer[512];
	int ret = readblock(device_fd,0,buffer);
	if(ret == 1)
	{
		struct superblock_t* superblock = (struct superblock_t*)malloc(sizeof(struct superblock_t));
		memcpy(superblock, buffer, sizeof(struct superblock_t));
		return superblock;
	}
	else
	{
		printf("ERROR: Unable to read super block on device. \n");
		return NULL;
	}
}

int writeSuperblock(struct mount_t* mountpoint,struct superblock_t* superblock)
{
	/*
		* Read the 0th block from device into a buffer
		* Write the superblock into the buffer
		* Write back the buffer into block 0
	*/
	int device_fd = mountpoint->device_fd;
	if(superblock != NULL)
	{
		char buffer[512];
		int ret1 = readblock(device_fd,0,buffer);
		if(ret1 == 1)
		{
			memcpy(buffer,superblock,sizeof(struct superblock_t));
			int ret2 = writeblock(device_fd,0,buffer);
			if(ret2 == 1)
			{
				return 1;
			}
			else
			{
				printf("ERROR: Unable to write super block on device. \n");
				return -1;
			}
		}
		else
		{
			printf("ERROR: Unable to write super block on device. \n");
			return -1;
		}
	}
	else
	{
		printf("ERROR: Unable to write super block on device. \n");
		return -1;
	}
}

struct metadata_t* readMetadata(struct mount_t* mountpoint)
{
	// Same as readSuperBlock(), but it is stored on block 1
	// Need to decrypt if emufs-encrypted is used
	int device_fd = mountpoint->device_fd;
	char fs_number = mountpoint->fs_number;
	char buffer[512];
	int ret = readblock(device_fd,1,buffer);
	if(ret == 1)
	{
		struct metadata_t* metadata = (struct metadata_t*)malloc(sizeof(struct metadata_t));
		if(fs_number == 1)
		{
			decrypt(buffer,buffer);
			memcpy(metadata, buffer, sizeof(struct metadata_t));
			return metadata;
		}
		else if(fs_number == -1)
		{
			printf("ERROR: File System does not exist. \n");
			return NULL;
		}
		else if(fs_number == 0)
		{
			memcpy(metadata, buffer, sizeof(struct metadata_t));
			return metadata;
		}
		else
		{
			printf("ERROR: Unknown File System. \n");
			return NULL;
		}
	}
	else
	{
		printf("ERROR: Unable to read metadata block on device. \n");
		return NULL;
	}  
}

int writeMetadata(struct mount_t* mountpoint,struct metadata_t* metadata)
{
	// Same as writeSuperblock(), but it is stored on block 1
	// Need to decrypt/encrypt if emufs-encrypted is used
	int device_fd = mountpoint->device_fd;
	int fs_number = mountpoint->fs_number;
	if(metadata != NULL)
	{
		char buffer[512];
		int ret1 = readblock(device_fd,1,buffer);
		if(ret1 == 1)
		{
			if(fs_number == 1)
			{
				decrypt(buffer,buffer);
				memcpy(buffer,metadata,sizeof(struct metadata_t));
				encrypt(buffer,buffer);	
			}
			else if(fs_number == 0)
			{
				memcpy(buffer,metadata,sizeof(struct metadata_t));
			}
			else if(fs_number == -1)
			{
				printf("ERROR: File System does not exist. \n");
				return -1;
			}
			else
			{
				printf("ERROR: Unknown File System. \n");
				return -1;
			}

			int ret2 = writeblock(device_fd,1,buffer);
			if(ret2 == 1)
			{
				return 1;
			}
			else
			{
				printf("ERROR: Unable to write metadata block on device. \n");
				return -1;
			}
		}
		else
		{
			printf("ERROR: Unable to write metadata block on device. \n");
			return -1;
		}
	}
	else
	{
		printf("ERROR: Unable to write metadata block on device. \n");
		return -1;
	}
}

/**************************** File system API ****************************/

int create_file_system(struct mount_t *mount_point, int fs_number)
{
	/*
	   	* Read the superblock.
	    * Set file system number on superblock
		* Clear the bitmaps.  values on the bitmap will be either '0', or '1', or'x'. 
		* Create metadata block in disk
		* Write superblock and metadata block back to disk.

		* Return value: -1,		ERROR
						 1, 	success
	*/
	struct superblock_t* superblock = readSuperblock(mount_point);
	if(superblock != NULL)
	{
		superblock->fs_number = fs_number;
		int disk_size = superblock->disk_size;
		char bitmap[MAX_BLOCKS];
		bitmap[0] = '1';
		bitmap[1] = '1';
		for(int i=2;i<disk_size;i++) bitmap[i] = '0';
		for(int i=disk_size;i<MAX_BLOCKS;i++) bitmap[i] = 'x';
		strcpy(superblock->bitmap,bitmap);


		struct metadata_t* metadata = (struct metadata_t*)malloc(sizeof(struct metadata_t));
		if(metadata != NULL)
		{	
			for(int i=0;i<MAX_FILES;i++) (metadata->inodes[i]).status = 0;
			mount_point->fs_number = fs_number;
			int ret1 = writeSuperblock(mount_point,superblock);
			if(ret1 == 1)
			{
				int ret2 = writeMetadata(mount_point,metadata);
				if(ret2 == 1) 
				{
					printf("[%s] File system created.\n", superblock->device_name);
					return 1;
				}
				else return -1;
			}
		}
		else
		{
			return -1;
		}

	}
	else
	{
		return -1;
	}
}


struct file_t* eopen(struct mount_t* mount_point, char* filename)
{
	/* 
		* If file exist, get the inode number. inode number is the index of inode in the metadata.
		* If file does not exist, 
			* find free inode.
			* allocate the free inode as USED
			* if free id not found, print the ERROR and return -1
		* Create the file hander (struct file_t)
		* Initialize offset in the file hander
		* Return file handler.

		* Return NULL on ERROR.
	*/
	struct metadata_t* metadata = readMetadata(mount_point);
	if(metadata != NULL)
	{
		int file_exists = 0;
		for(int i=0;i<MAX_FILES;i++)
		{
			if( strcmp(filename,(metadata->inodes[i]).name) == 0 )
			{
				file_exists = 1;
				struct file_t* file_handler = (struct file_t*)malloc(sizeof(struct file_t));
				file_handler->inode_number = i;
				file_handler->mount_point = mount_point;
				file_handler->offset = 0;
				(metadata->inodes[i]).modtime = time(NULL);
				printf("File '%s' Opened\n", filename);
				return file_handler;
			} 	
		}

		int free_id_found = 0;
		if(file_exists == 0)
		{
			for(int i=0;i<MAX_FILES;i++)
			{
				if( (metadata->inodes[i]).status == 0 )
				{
					free_id_found = 1;

					(metadata->inodes[i]).status = 1;
					strcpy((metadata->inodes[i]).name,filename);
					(metadata->inodes[i]).file_size = 0;
					for(int j=0;j<4;j++) (metadata->inodes[i]).blocks[j] = -1;
					(metadata->inodes[i]).modtime = time(NULL);

					if(writeMetadata(mount_point,metadata) == 1)
					{
						struct file_t* file_handler = (struct file_t*)malloc(sizeof(struct file_t));
						file_handler->inode_number = i;
						file_handler->mount_point = mount_point;
						file_handler->offset = 0;
						printf("File '%s' Opened\n", filename);
						return file_handler;
					}
					else
					{
						printf("ERROR: Unable to create file\n");
						return NULL;
					}					
				}
			}
		}

		if(free_id_found == 0)
		{
			printf("ERROR: Unable to create file\n");
			return NULL;
		}
	}
	else
	{
		printf("ERROR: Unable to read metadata block\n");
		return NULL;
	}
}

int ewrite(struct file_t* file, char* data, int size)
{
	// You do not need to implement partial writes in case file exceeds 4 blocks
	// or no free block is available in the disk. 

	// Return value: 	-1,  ERROR
	//					Number of bytes written
	struct superblock_t* superblock = readSuperblock(file->mount_point);
	if(superblock != NULL)
	{
		struct metadata_t* metadata = readMetadata(file->mount_point);
		if(metadata != NULL)
		{
			int offset = file->offset;
			int init_block = offset/BLOCKSIZE;
			int fin_block = (offset+size)/BLOCKSIZE;
			int inode_num = file->inode_number;
			int bytes_written = 0;
			int initial_file_size = (metadata->inodes[inode_num]).file_size;
			struct mount_t* mount_point = file->mount_point;
			char buffer[512];

			int blocks_required = 0;
			for(int i=init_block;i<fin_block;i++)
				if( (metadata->inodes[inode_num]).blocks[i] == -1 ) 
					blocks_required++;

			int num_free_blocks = 0;
			for(int i=0;i<MAX_BLOCKS;i++)
				if( (superblock->bitmap)[i] == '0' )
					num_free_blocks++;

			if(offset + size > BLOCKSIZE*MAX_FILE_SIZE || offset > initial_file_size)
			{
				printf("ERROR: Maximum File Size Exceeded\n");
				return -1;
			}	

			// Check for file system type
			if( mount_point->fs_number == -1 || mount_point->fs_number > 1 )
			{
				if(mount_point->fs_number == -1)
				{
					printf("ERROR: File System does not exist\n");
					return -1;
				}
				else if(mount_point->fs_number > 1)
				{
					printf("ERROR: Unknown File System\n");
					return -1;
				}
			}

			if( num_free_blocks >= blocks_required )
			{
				for(int i=init_block;i<fin_block;i++)
				{
					memcpy(buffer,data+bytes_written,BLOCKSIZE);

					// Allocate block if not already allocated
					if( (metadata->inodes[inode_num]).blocks[i] == -1 )
					{
						int free_block = -1;
						for(int i=2;i<MAX_BLOCKS;i++)
						{
							if( (superblock->bitmap)[i] == '0' )
							{
								free_block = i;
								break;
							}
						}
						if(free_block == -1) 
						{
							printf("ERROR: No free block available\n");
							return -1;
						}
						(metadata->inodes[inode_num]).blocks[i] = free_block;
						(superblock->bitmap)[free_block] = 1;
						(metadata->inodes[inode_num]).file_size += BLOCKSIZE;
					}

					// Encryption if required
					if( mount_point->fs_number == 1 )
					{
						encrypt(buffer,buffer);
					}

					// Write data on disk
					int ret = writeblock(mount_point->device_fd,(metadata->inodes[inode_num]).blocks[i],buffer);
					if(ret == 1)
					{
						bytes_written += BLOCKSIZE;
					}
					else
					{
						printf("ERROR: Unable to write to file\n");
						return -1;
					}
				}

				// Update file size & offset after writing has finished 
				file->offset = offset + bytes_written;

				int ret1 = writeSuperblock(mount_point,superblock);
				if(ret1 == 1)
				{
					int ret2 = writeMetadata(mount_point,metadata);
					if(ret2 == 1) 	return bytes_written;
					else 
					{
						printf("ERROR: Unable to write super block\n");
						return -1;
					}
				}
				else
				{
					printf("ERROR: Unable to write metadata block\n");
					return -1;
				}
			}
			else
			{
				printf("ERROR: Out of diskspace\n");
				return -1;
			}
		}
		else
		{
			printf("ERROR: Unable to find metadata\n");
			return -1;
		}
	}
	else
	{
		printf("ERROR: Unable to find superblock\n");
		return -1;
	}
}

int eread(struct file_t* file, char* data, int size)
{
	// NO partial READS.

	// Return value: 	-1,  ERROR
	//					Number of bytes read
	// printf("Herkrjlkadsjflksdhflksdf\n");
	struct superblock_t* superblock = readSuperblock(file->mount_point);
	if(superblock != NULL)
	{
		struct metadata_t* metadata = readMetadata(file->mount_point);
		if(metadata != NULL)
		{
			int offset = file->offset;
			int init_block = offset/BLOCKSIZE;
			int fin_block = (offset+size)/BLOCKSIZE;
			int inode_num = file->inode_number;
			int bytes_read = 0;
			int file_size = (metadata->inodes[inode_num]).file_size;
			struct mount_t* mount_point = file->mount_point;
			char buffer[512];

			if(offset + size > BLOCKSIZE*MAX_FILE_SIZE || offset > file_size)
			{
				printf("ERROR: Maximum file size exceeded\n");
				return -1;
			}

			// Check for file system type
			if( mount_point->fs_number == -1 || mount_point->fs_number > 1 )
			{
				if(mount_point->fs_number == -1)
				{
					printf("ERROR: File System does not exist\n");
					return -1;
				}
				else if(mount_point->fs_number > 1)
				{
					printf("ERROR: Unknown File System\n");
					return -1;
				}
			}

			int blocks_allocated = 1;
			for(int i=init_block;i<fin_block;i++)
			{
				if( (metadata->inodes[inode_num]).blocks[i] == -1 )
				{
					blocks_allocated = 0;
					break;
				}
			}

			if(blocks_allocated == 1)
			{
				for(int i=init_block;i<fin_block;i++)
				{
					// Write data on disk
					int ret = readblock(mount_point->device_fd,(metadata->inodes[inode_num]).blocks[i],buffer);
					if(ret == 1)
					{
						// Decryption if required
						if( mount_point->fs_number == 1 )
						{
							decrypt(buffer,buffer);
						}
						memcpy(data+bytes_read,buffer,BLOCKSIZE);
						bytes_read += BLOCKSIZE;
					}
					else
					{
						printf("ERROR: Unable to read from file\n");
						return -1;
					}
				}

					// Update offset after writing has finished 
					file->offset = offset + bytes_read;
					// return bytes_read;

					int ret1 = writeSuperblock(mount_point,superblock);
					if(ret1 == 1)
					{
						int ret2 = writeMetadata(mount_point,metadata);
						if(ret2 == 1) return bytes_read;
						else 
						{
							printf("ERROR: Unable to write super block\n");
							return -1;
						}
					}
					else
					{
						printf("ERROR: Unable to write metadata block\n");
						return -1;
					}
			}
			else
			{
				printf("ERROR: Invalid offset\n");
				return -1;
			}
		}
		else
		{
			printf("ERROR: Unable to find metadata\n");
			return -1;
		}
	}
	else
	{
		printf("ERROR: Unable to find superblock\n");
		return -1;
	}
}

void eclose(struct file_t* file)
{
	// free the memory allocated for the file handler 'file'
	free(file);
	printf("File Closed\n");
}

int eseek(struct file_t *file, int offset)
{
	// Change the offset in file hanlder 'file'
	struct metadata_t* metadata = readMetadata(file->mount_point);
	if(metadata != NULL)
	{
		int inode_num = file->inode_number;
		if(offset > metadata->inodes[inode_num].file_size)
		{
			printf("ERROR: Offset greater than file size\n");
			return -1;
		}
		file->offset = offset;
		return 1;
	}
	else
	{
		printf("ERROR: Unable to read metadata block\n");
		return -1;
	}
}

void fsdump(struct mount_t* mount_point)
{
	struct metadata_t* metadata = readMetadata(mount_point);
	if(metadata != NULL)
	{
		printf("\n[%s] fsdump \n", mount_point->device_name);
		printf("%-10s %6s \t[%s] \t%s\n", "  NAME", "SIZE", "BLOCKS", "LAST MODIFIED");
		
		for(int i=0;i<MAX_FILES;i++)
		{
			if(metadata->inodes[i].status == 1)
			{
				printf("%-10s %6d \t[%d %d %d %d] \t%s", 
					metadata->inodes[i].name, 
					metadata->inodes[i].file_size,
					(metadata->inodes[i]).blocks[0],
					(metadata->inodes[i]).blocks[1],
					(metadata->inodes[i]).blocks[2],
					(metadata->inodes[i]).blocks[3],
					asctime(localtime(&((metadata->inodes[i]).modtime))));
			}	
		}
	}
	else
	{
		printf("Unable to read metadata block\n");
	}
}
