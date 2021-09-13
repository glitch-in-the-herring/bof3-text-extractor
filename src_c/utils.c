#include "extractor.h"

word convert_little_endian(byte a[], int n, int k)
{
    word result = 0;
    for (int i = n; i >= k; i--)
    {
        result = a[i] << 8 * i | result;
    }
    return result;
}

word find_dialogue_section(FILE *f, word count, word *section_size)
{
    byte toc_entry[16];
    word tmp = 0;
    word address = 0x0800;
    do
    {
        address += tmp;
        if (fread(toc_entry, 1, sizeof(toc_entry), f) != sizeof(toc_entry))
        {
            return -1;
        }

        if (is_dialogue_section(toc_entry))
        {
            *section_size = convert_little_endian(toc_entry, 3, 0);
            return address;
        }

        tmp = convert_little_endian(toc_entry, 3, 0);
        if (tmp % 0x0800 != 0)
        {
            tmp = 0x0800 * ((tmp + 0x0800) / 0x0800);
        }
    } 
    while (count--);
    return 0;
}

bool is_math_tbl(byte toc_header[])
{
    byte magic[8] = {0x4d, 0x41, 0x54, 0x48, 0x5f, 0x54, 0x42, 0x4c};
    for (int i = 8; i < 16; i++)
    {
        if (toc_header[i] != magic[i - 8])
        {
            return false;
        }
    }
    return true;
}

bool is_dialogue_section(byte toc_entry[])
{
    return toc_entry[8] == 0x00 && toc_entry[9] == 0x02 && toc_entry[10] != 0x16 && toc_entry[11] == 0x02;
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
        case 0x91:
            return ';';
            break;
        default:
            return 0x00;
            break;
    }
}

char *is_symbol(byte a)
{
    switch (a)
    {
        case 0x7b:
            return "↑";
            break;
        case 0x7c:
            return "↓";
            break; 
        case 0x7d:
            return "←";
            break;
        case 0x7e:
            return "→";
            break;
        case 0x7f:
            return "〜";
            break;
        case 0x80:
            return "◯";
            break;
        case 0x81:
            return "△";
            break;
        case 0x82:
            return "×";
            break;
        case 0x83:
            return "□";
            break;
        case 0x86:
            return "↖";
            break;
        case 0x87:
            return "↘";
            break; 
        case 0x88:
            return "↗";
            break;
        case 0x89:
            return "↙";
            break;
        case 0x93:
            return "%";
            break;
        case 0x8a:
            return "©";
            break;
        case 0x8d:
            return "&";
            break;
        default:
            return "";
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
