#include <cstdint>

#ifndef _EXT2_H_
#define _EXT2_H_

/*
 * Estrutura do inode ixt2 - https://www.nongnu.org/ext2-doc/ext2.html#inode-table
 */
struct ext2_inode
{
  uint16_t i_mode;          /* File mode */
  uint16_t i_uid;           /* Low 16 bits of Owner Uid */
  uint32_t i_size;          /* Size in bytes */
  uint32_t i_atime;         /* Access time */
  uint32_t i_ctime;         /* Creation time */
  uint32_t i_mtime;         /* Modification time */
  uint32_t i_dtime;         /* Deletion Time */
  uint16_t i_gid;           /* Low 16 bits of Group Id */
  uint16_t i_links_count;   /* Links count */
  uint32_t i_blocks;        /* Blocks count IN DISK SECTORS*/
  uint32_t i_flags;         /* File flags */
  uint32_t osd1;            /* OS dependent 1 */
  uint32_t i_block[15];     /* Pointers to blocks */
  uint32_t i_generation;    /* File version (for NFS) */
  uint32_t i_file_acl;      /* File ACL */
  uint32_t i_dir_acl;       /* Directory ACL */
  uint32_t i_faddr;         /* Fragment address */
  uint32_t i_osd2[3];
};

#endif