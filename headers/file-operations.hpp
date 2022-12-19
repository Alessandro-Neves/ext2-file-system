#include <stdio.h>

#ifndef _FILE_OPERATIONS_H_
#define _FILE_OPERATIONS_H_

/* realiza a leitura de um arquivo na máquina do usuário */
FILE *get_file(const char *address);

/* realiza a movimentação 'shift' de uma determinado porção de bytes na imagem */
void shift_bytes(FILE* ext2_image, uint32_t position, unsigned int qtd_bytes, unsigned int offset);

#endif