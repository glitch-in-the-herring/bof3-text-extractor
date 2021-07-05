#include "extractor.h"

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

bool is_alpha(byte a)
{
    return (a >= 65 && a <= 90) || (a >= 97 && a <= 122);
}

char is_punct(byte a)
{
    switch (a)
    {
        case 0x5d:
            return '!';
            break;
        case 0x90:
            return '"';
            break;
        case 0x8e:
            return '\'';
            break;
        case 0x3c:
            return ',';
            break;
        case 0x3d:
            return '-';
            break;
        case 0x3e:
            return '.';
            break;
        case 0x5c:
            return '?';
            break;
        default:
            return 0x00;
            break;
    }
}
