#include <stdio.h>

#ifndef _FILE_OPERATIONS_H_
#define _FILE_OPERATIONS_H_

/**
 * @brief realiza a leitura da imagem ext2 do host
 * @param address endereço da imagem ext2
 * @return FILE* ponteiro para o arquivo aberto
 * @throws const char*
 */
FILE *get_file(const char *address);

#endif