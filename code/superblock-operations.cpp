#include <iostream>
#include "../headers/superblock-operations.hpp"
#include "../headers/colors.hpp"

using namespace std;


Ext2_Superblock* read_ext2_superblock(FILE* ext2_image) {

    /* aloca memória para armazenar toda a estrutura do superbloco */
    Ext2_Superblock *superblock = (Ext2_Superblock *)malloc(sizeof(Ext2_Superblock));

    /* posicionar o ponteiro no início do superbloco (bite 1024) */
    fseek(ext2_image, 1024, SEEK_SET);

    /* copiar 1024 unidade de 1byte a partir da posicão atual (bite 1024)*/
    /* copia á area do supebloco para &superbloco */
    fread(superblock, 1, 1024, ext2_image);

    return superblock;
}

void print_superblock(Ext2_Superblock* superblock){
    cout << string(BLUE) << endl << "[ Superblock infos ]" << string(DEFAULT) << endl;
    cout << "s_inodes_count:\t\t"         << superblock->s_inodes_count               << endl;
    cout << "s_blocks_count:\t\t"         << superblock->s_blocks_count               << endl;  
    cout << "s_r_blocks_count:\t"         << superblock->s_r_blocks_count             << endl;
    cout << "s_free_blocks_count:\t"      << superblock->s_free_blocks_count          << endl;
    cout << "s_free_inodes_count:\t"      << superblock->s_free_inodes_count          << endl;
    cout << "s_first_data_block:\t"       << superblock->s_first_data_block           << endl;
    cout << "s_log_block_size:\t"         << superblock->s_log_block_size             << endl;
    cout << "s_log_frag_size:\t"          << superblock->s_log_frag_size              << endl;
    cout << "s_blocks_per_group:\t"       << superblock->s_blocks_per_group           << endl;
    cout << "s_frags_per_group:\t"        << superblock->s_frags_per_group            << endl;
    cout << "s_inodes_per_group:\t"       << superblock->s_inodes_per_group           << endl;
    cout << "s_mtime:\t\t"                << superblock->s_mtime                      << endl;
    cout << "s_wtime:\t\t"                << superblock->s_wtime                      << endl;
    cout << "s_mnt_count:\t\t"            << superblock->s_mnt_count                  << endl;
    cout << "s_max_mnt_count:\t"          << superblock->s_max_mnt_count              << endl;
    cout << "s_magic:\t\t"                << superblock->s_magic                      << endl;
    cout << "s_state:\t\t"                << superblock->s_state                      << endl;
    cout << "s_errors:\t\t"               << superblock->s_errors                     << endl;
}