#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>

#include "types.h"
#include "fs.h"


#define BLOCK_SIZE (512)

int nblocks = 995;
int ninodes = 200;
int size = 1024;


uint
i2b(uint inum)
{
  return (inum / IPB) + 2;
}


int
main(int argc, char *argv[])
{
  int r,i,n,fsfd;
  char *addr;
  struct dinode *dip;
  struct superblock *sb;
  struct dirent *rde;

  if(argc < 2){
    fprintf(stderr, "Usage: sample fs.img ...\n");
    exit(1);
  }


  fsfd = open(argv[1], O_RDONLY);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  /* Dont hard code the size of file. Use fstat to get the size */
  addr = mmap(NULL, 524248, PROT_READ, MAP_PRIVATE, fsfd, 0);
  if (addr == MAP_FAILED){
	perror("mmap failed");
	exit(1);
  }
  /* read the super block */
  sb = (struct superblock *) (addr + 1 * BLOCK_SIZE);
  printf("fs size %d, no. of blocks %d, no. of inodes %d \n", sb->size, sb->nblocks, sb->ninodes);

  /* read the inodes */
  dip = (struct dinode *) (addr + i2b((uint)0)*BLOCK_SIZE); 
  printf("begin addr %p, begin inode %p , offset %d \n", addr, dip, (char *)dip -addr);
  printf("Root inode  size %d links %d \n", dip[ROOTINO].size, dip[ROOTINO].nlink);

  /* get the address of root dir */
  rde = (struct dirent *) (addr + (dip[ROOTINO].addrs[0])*BLOCK_SIZE);

  /* print the entries in the first block of root dir */
  n = dip[ROOTINO].size/sizeof(struct dirent);
  for (i = 0; i < n; i++,rde++)
 	printf(" inum %d, name %s \n", rde->inum, rde->name);

  exit(0);

}

