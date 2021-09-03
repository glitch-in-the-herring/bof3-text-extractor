#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t byte;
typedef uint32_t word;
typedef struct node
{
    byte chunk[512];
    struct node *next;
} node;

void free_node(node *n);
void copy_arrays(byte target[], byte source[], int n);
word convert_little_endian(byte a[], int n, int k);
word find_dialogue_section(FILE *f, word count, word *section_size);
bool is_math_tbl(byte chunk[]);
bool is_dialogue_section(byte toc_entry[]);
bool is_alpha(byte a);
char is_punct(byte a);
char *is_color(byte a);
char *is_effect(byte a);