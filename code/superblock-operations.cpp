#include <iostream>
#include "../headers/superblock-operations.hpp"
#include "../headers/colors.hpp"
#include "../headers/util-operations.hpp"

using namespace std;

Ext2_Superblock* read_ext2_superblock(FILE* ext2_image) {

    /* aloca memória para armazenar toda a estrutura do superbloco */
    Ext2_Superblock *superblock = (Ext2_Superblock *)malloc(sizeof(Ext2_Superblock));

    /* posicionar o ponteiro no início do superbloco (bite 1024) */
    fseek(ext2_image, 1024, SEEK_SET);

    /* copiar 1024 unidade de 1byte a partir da posicão atual (bite 1024)*/
    /* copia á area do supebloco para &superbloco */
    fread(superblock, 1, sizeof(Ext2_Superblock), ext2_image);

    return superblock;
}

void print_superblock(Ext2_Superblock* superblock){
    cout << "inodes count:\t\t"                     << (unsigned) superblock->s_inodes_count               << endl;
    cout << "blocks count:\t\t"                     << (unsigned) superblock->s_blocks_count               << endl;  
    cout << "reserved blocks count:\t"              << (unsigned) superblock->s_r_blocks_count             << endl;
    cout << "free blocks count:\t"                  << (unsigned) superblock->s_free_blocks_count          << endl;
    cout << "free inodes count:\t"                  << (unsigned) superblock->s_free_inodes_count          << endl;
    cout << "first data block:\t"                   << (unsigned) superblock->s_first_data_block           << endl;
    cout << "block size:\t"                         << (unsigned) superblock->s_log_block_size             << endl;
    cout << "frag size:\t"                          << superblock->s_log_frag_size                         << endl;
    cout << "blocks per group:\t"                   << (unsigned) superblock->s_blocks_per_group           << endl;
    cout << "frags per group:\t"                    << (unsigned) superblock->s_frags_per_group            << endl;
    cout << "inodes per group:\t"                   << (unsigned) superblock->s_inodes_per_group           << endl;
    cout << "mount time:\t\t"                       << (unsigned) superblock->s_mtime                      << endl;
    cout << "write time:\t\t"                       << (unsigned) superblock->s_wtime                      << endl;
    cout << "mount count:\t\t"                      << (unsigned) superblock->s_mnt_count                  << endl;
    cout << "max mount count:\t"                    << (unsigned) superblock->s_max_mnt_count              << endl;
    cout << "magic signature:\t\t"                  << std::hex << std::uppercase << superblock->s_magic   << std::dec << endl;
    cout << "file system state :\t\t"               << (unsigned) superblock->s_state                      << endl;
    cout << "errors:\t\t"                           << (unsigned) superblock->s_errors                     << endl;
    cout << "minor revision level:\t\t"             << (unsigned) superblock->s_minor_rev_level            << endl;
    cout << "time of last check:\t\t";               
    print_time_from_unix((unsigned) superblock->s_lastcheck); // print em DD/MM/AAAA HH:MM
    
    cout << "max check interval:\t\t"               << (unsigned) superblock->s_checkinterval              << endl;
    cout << "creator OS:\t\t"                       << (unsigned) superblock->s_creator_os                 << endl;
    cout << "revision level:\t\t"                   << (unsigned) superblock->s_rev_level                  << endl;
    cout << "default uid reserved blocks:\t\t"      << (unsigned) superblock->s_def_resuid                 << endl;
    cout << "default gid reserved blocks:\t\t"      << (unsigned) superblock->s_def_resgid                 << endl;
    cout << "first non-reserved inode:\t\t"         << (unsigned) superblock->s_first_ino                  << endl;
    cout << "inode size:\t\t"                       << (unsigned) superblock->s_inode_size                 << endl;
    cout << "block group number:\t\t"               << (unsigned) superblock->s_block_group_nr             << endl;
    cout << "compatible feature set:\t\t"           << (unsigned) superblock->s_feature_compat             << endl;
    cout << "incompatible feature set:\t\t"         << (unsigned) superblock->s_feature_incompat           << endl;
    cout << "read only compatible feature set:\t\t" << (unsigned) superblock->s_feature_ro_compat          << endl;
    cout << "volume UUID:\t\t"                      << superblock->s_uuid                       << endl;
    cout << "volume name:\t\t"                      << superblock->s_volume_name                << endl;
    cout << "volume last mounted:\t\t"              << superblock->s_last_mounted               << endl;
    cout << "algorithm usage bitmap:\t\t"           << (unsigned) superblock->s_algorithm_usage_bitmap     << endl;
    cout << "blocks to try to preallocate :\t\t"    << (unsigned) superblock->s_prealloc_blocks            << endl;
    cout << "blocks preallocate dir:\t\t"           << (unsigned) superblock->s_prealloc_dir_blocks        << endl;

    cout << "s_inode_size:\t\t"           << (unsigned) superblock->s_inode_size                 << endl;
}