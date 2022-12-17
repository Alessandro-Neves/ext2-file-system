// Função para inverter a ordem dos bits de um char
char reverse_bits(char c) {
  char inverted = 0;
  for (int i = 0; i < 8; i++) {
    inverted = (inverted << 1) | (c & 1);
    c = c >> 1;
  }
  return inverted;
}

// Função para setar um determinado bit do char para 0 ou para 1
char set_bit(char c, int index, bool value) {
  if (value) return c | (1 << index);
  else return c & ~(1 << index);
}
