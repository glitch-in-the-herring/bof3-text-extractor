#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define HRSTART 0x5B
#define HREND 0xA9
#define KTSTART 0xAB
#define KTEND 0xFA
#define KJSTART 0x1200
#define KJEND 0x13B8

typedef uint8_t byte;
typedef struct node
{
    byte chunk[512];
    struct node *next;
} node;

void free_node(node *n);
void load_lookup_table(int n, char table[][n], FILE *source);
void copy_arrays(byte target[], byte source[], int n);
bool is_math_tbl(byte chunk[]);
bool is_first_prepadding(byte chunk[]);
bool is_second_prepadding(byte chunk[]);
bool is_final_chunk(byte chunk[]);
bool is_hiragana(byte a);
bool is_katakana(byte a);
bool is_kanji_start(byte a);
char *is_punct(byte a);
