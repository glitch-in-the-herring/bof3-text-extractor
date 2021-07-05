#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t byte;
typedef struct node
{
    byte chunk[512];
    struct node *next;
} node;

void free_node(node *n);
void copy_arrays(byte target[], byte source[], int n);
bool is_math_tbl(byte chunk[]);
bool is_first_prepadding(byte chunk[]);
bool is_second_prepadding(byte chunk[]);
bool is_final_chunk(byte chunk[]);
bool is_alpha(byte a);
char is_punct(byte a);
