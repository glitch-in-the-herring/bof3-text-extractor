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
        if (chunk[i] != magic[i - 8])
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

// The dialogue section always lines up with the
// chunks, that is to say, a dialogue section fits
// within an integer number of chunks.
// To accomplish this it must add paddings, which
// consists of repeating 0x5F. This can be used to
// check the end of the section.
bool is_final_chunk(byte chunk[])
{
    return chunk[511] == 0x5f && chunk[510] == 0x5f;
}

// The builtin function in ctype.h should in theory
// work just fine.
bool is_alpha(byte a)
{
    return (a >= 65 && a <= 90) || (a >= 97 && a <= 122);
}

// Converts codepoints used for encoding punctuations
// into actual punctuations.
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
        case 0x8f:
            return ':';
            break;
        default:
            return 0x00;
            break;
    }
}

char *is_color(byte a)
{
    switch (a)
    {
        case 0x01:
            return "PURPLE";
            break;
        case 0x02:
            return "RED";
            break;
        case 0x03:
            return "CYAN";
            break;
        case 0x04:
            return "YELLOW";
            break;
        case 0x05:
            return "PINK";
            break;
        case 0x06:
            return "GREEN";
            break;
        case 0x07:
            return "BLACK";
            break;
        default:
            return "";
            break;            
    }    
}

char *is_effect(byte a)
{
    switch (a)
    {
        case 0x00:
            return "type=shake duration=short";
            break;        
        case 0x01:
            return "type=shake duration=long";
            break;
        case 0x02:
            return "type=shake duration=indef";
            break;
        case 0x03:
            return "type=big1 duration=short";
            break;
        case 0x04:
            return "size=big2 duration=short";
            break;
        case 0x05:
            return "size=big3 duration=short";
            break;
        case 0x06:
            return "size=big1 duration=long";
            break;
        case 0x07:
            return "size=big2 duration=long";
            break;
        case 0x08:
            return "size=big3 duration=long";
            break;
        case 0x09:
            return "size=big1 duration=indef";
            break;
        case 0x0a:
            return "size=big2 duration=indef";
            break;
        case 0x0b:
            return "size=big3 duration=indef";
            break;
        case 0x0c:
            return "size=small1 duration=short";
            break;
        case 0x0d:
            return "size=small2 duration=short";
            break;
        case 0x0e:
            return "size=small3 duration=short";
            break; 
        case 0x0f:
            return "size=small1 duration=long";
            break;
        case 0x10:
            return "size=small2 duration=long";
            break;
        case 0x11:
            return "size=small3 duration=long";
            break;
        case 0x12:
            return "size=small1 duration=indef";
            break;
        case 0x13:
            return "size=small2 duration=indef";
            break;
        case 0x14:
            return "size=small3 duration=indef";
            break;
        case 0x15:
            return "type=rise duration=long";
            break;
        case 0x16:
            return "type=rise duration=indef";
            break;
        case 0x17:
            return "type=jump1";
            break;
        case 0x18:
            return "type=jump2";
            break;
        case 0x19:
            return "type=jump3";
            break;
        default:
            return "";
            break;
    }    
}