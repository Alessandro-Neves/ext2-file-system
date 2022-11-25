#include "../headers/blocks-group-operations.hpp"

Ext2_Blocks_Group_Descriptor *read_ext2_blocks_group_descriptor(FILE *ext2_image)
{

  /* aloca espaço na memória suficiente para armazenar a estrutura do Ext2_Blocks_Group_Descriptor */
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor = (Ext2_Blocks_Group_Descriptor *)malloc(sizeof(Ext2_Blocks_Group_Descriptor));

  /* posicionar o ponteiro no início do superbloco (bite 2048) */
  fseek(ext2_image, 2048, SEEK_SET);

  /* copiar 288 unidade de 1byte a partir da posicão atual (bite 2048)*/
  /* copia á area do supebloco para &superbloco */
  fread(blocks_group_descriptor, 1, 288, ext2_image);

  /*  */
  return blocks_group_descriptor;
}

void print_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* blocks_group_descriptor) {
  cout << string(BLUE) << endl << "[ Blocks group descriptor infos ]" << string(DEFAULT) << endl;
  cout << "bg_block_bitmap:\t\t"          << blocks_group_descriptor->bg_block_bitmap              << endl;
  cout << "bg_inode_bitmap:\t\t"          << blocks_group_descriptor->bg_inode_bitmap              << endl;
  cout << "bg_inode_table:\t\t\t"         << blocks_group_descriptor->bg_inode_table               << endl;
  cout << "bg_free_blocks_count:\t\t"     << blocks_group_descriptor->bg_free_blocks_count         << endl;
  cout << "bg_free_inodes_count:\t\t"     << blocks_group_descriptor->bg_free_inodes_count         << endl;
  cout << "bg_used_dirs_count:\t\t"       << blocks_group_descriptor->bg_used_dirs_count           << endl;
  cout << "bg_pad:\t\t\t\t"               << blocks_group_descriptor->bg_pad                       << endl;
}