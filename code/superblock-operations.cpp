#include <iostream>
#include "../headers/superblock-operations.hpp"
#include "../headers/colors.hpp"
#include "../headers/util-operations.hpp"

using namespace std;

Ext2_Superblock *read_ext2_superblock(FILE *ext2_image)
{

    /* aloca memória para armazenar toda a estrutura do superbloco */
    Ext2_Superblock *superblock = (Ext2_Superblock *)malloc(sizeof(Ext2_Superblock));

    /* posicionar o ponteiro no início do superbloco (bite 1024) */
    fseek(ext2_image, 1024, SEEK_SET);

    /* copiar 1024 unidade de 1byte a partir da posicão atual (bite 1024)*/
    /* copia á area do supebloco para &superbloco */
    fread(superblock, 1, sizeof(Ext2_Superblock), ext2_image);

    return superblock;
}

void write_ext2_superblock(Ext2_Superblock *superblock, FILE *ext2_image)
{
    fseek(ext2_image, 1024, SEEK_SET);
    fwrite(superblock, 1, sizeof(Ext2_Superblock), ext2_image);
}

void format_str(const char *str)
{
    std::string s(str);

    for (char c : s){
        if ((c > 47 && c < 58) || (c > 64 && c < 91) || (c > 96 && c <123))
            cout << c;
        else cout << "-";
    }
}

void print_superblock(Ext2_Superblock *superblock)
{
    cout << "inodes count:  " << (unsigned)superblock->s_inodes_count << endl;
    cout << "blocks count:  " << (unsigned)superblock->s_blocks_count << endl;
    cout << "reserved blocks count:  " << (unsigned)superblock->s_r_blocks_count << endl;
    cout << "free blocks count:  " << (unsigned)superblock->s_free_blocks_count << endl;
    cout << "free inodes count:  " << (unsigned)superblock->s_free_inodes_count << endl;
    cout << "first data block:  " << (unsigned)superblock->s_first_data_block << endl;
    cout << "block size:  " << (unsigned)superblock->s_log_block_size << endl;
    cout << "frag size:  " << superblock->s_log_frag_size << endl;
    cout << "blocks per group:  " << (unsigned)superblock->s_blocks_per_group << endl;
    cout << "frags per group:  " << (unsigned)superblock->s_frags_per_group << endl;
    cout << "inodes per group:  " << (unsigned)superblock->s_inodes_per_group << endl;
    cout << "mount time:  " << (unsigned)superblock->s_mtime << endl;
    cout << "write time:  " << (unsigned)superblock->s_wtime << endl;
    cout << "mount count:  " << (unsigned)superblock->s_mnt_count << endl;
    cout << "max mount count:  " << (unsigned)superblock->s_max_mnt_count << endl;
    cout << "magic signature:  " << std::hex << std::uppercase << superblock->s_magic << std::dec << endl;
    cout << "file system state:  " << (unsigned)superblock->s_state << endl;
    cout << "errors:  " << (unsigned)superblock->s_errors << endl;
    cout << "minor revision level:  " << (unsigned)superblock->s_minor_rev_level << endl;
    cout << "time of last check:  ";

    print_time_from_unix((unsigned)superblock->s_lastcheck); // print em DD/MM/AAAA HH:MM

    cout << "max check interval:  " << (unsigned)superblock->s_checkinterval << endl;
    cout << "creator OS:  " << (unsigned)superblock->s_creator_os << endl;
    cout << "revision level:  " << (unsigned)superblock->s_rev_level << endl;
    cout << "default uid reserved blocks:  " << (unsigned)superblock->s_def_resuid << endl;
    cout << "default gid reserved blocks:  " << (unsigned)superblock->s_def_resgid << endl;
    cout << "first non-reserved inode:  " << (unsigned)superblock->s_first_ino << endl;
    cout << "inode size:  " << (unsigned)superblock->s_inode_size << endl;
    cout << "block group number:  " << (unsigned)superblock->s_block_group_nr << endl;
    cout << "compatible feature set:  " << (unsigned)superblock->s_feature_compat << endl;
    cout << "incompatible feature set:  " << (unsigned)superblock->s_feature_incompat << endl;
    cout << "read only compatible feature set:  " << (unsigned)superblock->s_feature_ro_compat << endl;
    cout << "volume UUID:  "; format_str((const char*) superblock->s_uuid); cout << endl;
    cout << "volume name:  " << superblock->s_volume_name << endl;
    cout << "volume last mounted:  " << superblock->s_last_mounted << endl;
    cout << "algorithm usage bitmap:  " << (unsigned)superblock->s_algorithm_usage_bitmap << endl;
    cout << "blocks to try to preallocate:  " << (unsigned)superblock->s_prealloc_blocks << endl;
    cout << "blocks preallocate dir:  " << (unsigned)superblock->s_prealloc_dir_blocks << endl;
}