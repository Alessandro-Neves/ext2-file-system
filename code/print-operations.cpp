#include <iostream>

#include "../headers/colors.hpp"
#include "../headers/print-operations.hpp"

using namespace std;

/**
 * @brief imprime os dados do superbloco na saída
 * @param superblock estrutura com dados do superbloco
 */
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