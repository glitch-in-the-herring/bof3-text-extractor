#include "jpextractor.h"

void free_node(node *n)
{
    node *temp;
    while (n != NULL)
    {
        temp = n->next;
        free(n);
        n = temp;
    }
}

void load_lookup_table(int n, char table[][n], FILE *source)
{
    int i = 0;
    int j = 0;
    char c;
    while((c = fgetc(source)) != EOF)
    {
        if (c != '\n')
        {
            table[i][j] = c;
            j++;
        }
        else
        {
            table[i][j] = '\0';
            j = 0;
            i++;
        }
    }
}

void copy_arrays(byte target[], byte source[], int n)
{
    for (int i = 0; i < n; i++)
    {
        target[i] = source[i];
    }
}

bool is_math_tbl(byte chunk[])
{
    byte magic[8] = {0x4d, 0x41, 0x54, 0x48, 0x5f, 0x54, 0x42, 0x4c};
    for (int i = 8; i < 16; i++)
    {
        if(chunk[i] != magic[i - 8])
        {
            return false;
        }
    }
    return true;
}

bool is_first_prepadding(byte chunk[])
{
    int a = 0;
    int b = 0;
    for (int i = 0; i < 512; i++)
    {
        if (chunk[i] == 0x00)
        {
            a++;
        }
        else if (chunk[i] == 0x5f)
        {
            b++;
        }
    }
    return a == 136 && b == 376;
}

bool is_second_prepadding(byte chunk[])
{
    int a = 0;
    for (int i = 0; i < 512; i++)
    {
        if (chunk[i] == 0x5f)
        {
            a++;
        }
    }
    return a == 512;
}

bool is_final_chunk(byte chunk[])
{
    return chunk[511] == 0x5f && chunk[510] == 0x5f;
}

bool is_hiragana(byte a)
{
    return a >= HRSTART && a <= HREND;
}

bool is_katakana(byte a)
{
    return a >= KTSTART && a <= KTEND;
}

bool is_kanji_start(byte a)
{
    return a == 0x12 || a == 0x13;
}

char *is_punct(byte a)
{
    switch (a)
    {
        case 0x2A:
            return "「";
            break;
        case 0x3E:
            return "‥";
            break;
        case 0x3F:
            return "？";
            break;
        case 0x40:
            return "！";
            break;
        case 0xFC:
            return "ー";
            break;
        case 0xFD:
            return "。";
            break;
        case 0xFE:
            return "、";
            break;
        default:
            return "";
            break;
    }
}
