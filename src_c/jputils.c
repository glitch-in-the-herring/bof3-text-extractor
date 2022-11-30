#include "jpextractor.h"

word convert_little_endian(byte a[], int n, int bytes)
{
    word result = 0;
    for (int i = bytes - 1; i >= 0; i--)
    {
        result = a[n + i] << 8 * i | result;
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
            *section_size = convert_little_endian(toc_entry, 0, 4);
            return address;
        }

        tmp = convert_little_endian(toc_entry, 0, 4);
        if (tmp % 0x0800 != 0)
        {
            tmp = 0x0800 * ((tmp + 0x0800) / 0x0800);
        }
    }
    while (count--);
    return 0;
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

bool is_math_tbl(byte toc_header[])
{
    byte magic[8] = {0x4d, 0x41, 0x54, 0x48, 0x5f, 0x54, 0x42, 0x4c};
    for (int i = 8; i < 16; i++)
    {
        if(toc_header[i] != magic[i - 8])
        {
            return false;
        }
    }
    return true;
}

bool is_dialogue_section(byte toc_entry[])
{
    return toc_entry[4] == 0x00 && toc_entry[5] == 0x00 && toc_entry[6] == 0x01 && toc_entry[7] == 0x80;
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

bool is_alphanum(byte a)
{
    return (a >= 97 && a <= 122) || (a >= 48 && a <= 57);
}

char *is_position(byte a)
{
    static char buffer[20];
    byte position = a & 0x0f;
    byte style = (a & 0xf0) >> 4;

    switch (position)
    {
        case 0x00:
            strcpy(buffer, "[BOTTOMM");
            break;
        case 0x01:
            strcpy(buffer, "[MIDM");
            break;
        case 0x02:
            strcpy(buffer, "[TOPM");
            break;
        case 0x03:
            strcpy(buffer, "[TOPL");
            break;
        case 0x04:
            strcpy(buffer, "[TOPR");
            break;
        case 0x05:
            strcpy(buffer, "[BOTTOML");
            break;
        case 0x06:
            strcpy(buffer, "[BOTTOMR");
            break;
    }

    switch (style)
    {
        case 0x00:
            strcat(buffer, "] ");
            break;
        case 0x04:
            strcat(buffer, " SMALL] ");
            break;
        case 0x08:
            strcat(buffer, " NOBOX] ");
            break;
    }

    return buffer;
}

char *is_punct(byte a)
{
    switch (a)
    {
        case 0x2A:
            return "「";
        case 0x3E:
            return "‥";
        case 0x3F:
            return "？";
        case 0x40:
            return "！";
        case 0xFC:
            return "ー";
        case 0xFD:
            return "。";
        case 0xFE:
            return "、";
        default:
            return "";
    }
}

char *is_symbol(byte a)
{
    switch (a)
    {
        case 0x00:
            return "↑";
        case 0x01:
            return "↓";
        case 0x02:
            return "←";
        case 0x03:
            return "→";
        case 0x04:
            return "❤";
        case 0x05:
            return "♪";
        case 0x06:
            return "Ƶ";
        case 0x07:
            return "〜";
        case 0x08:
            return "◯";
        case 0x09:
            return "×";
        case 0x0a:
            return "△";
        case 0x0b:
            return "□";
        case 0x0c:
            return "✩";
        case 0x11:
            return "↖";
        case 0x12:
            return "↘";
        case 0x13:
            return "↗";
        case 0x14:
            return "↙";
        default:
            return "";
    }
}

char *is_color(byte a)
{
    switch (a)
    {
        case 0x01:
            return "PURPLE";
        case 0x02:
            return "RED";
        case 0x03:
            return "CYAN";
        case 0x04:
            return "YELLOW";
        case 0x05:
            return "PINK";
        case 0x06:
            return "GREEN";
        case 0x07:
            return "BLACK";
        default:
            return "";
    }    
}

char *is_effect(byte a)
{
    switch (a)
    {
        case 0x00:
            return "type=shake duration=short";
        case 0x01:
            return "type=shake duration=long";
        case 0x02:
            return "type=shake duration=indef";
        case 0x03:
            return "type=big1 duration=short";
        case 0x04:
            return "size=big2 duration=short";
        case 0x05:
            return "size=big3 duration=short";
        case 0x06:
            return "size=big1 duration=long";
        case 0x07:
            return "size=big2 duration=long";
        case 0x08:
            return "size=big3 duration=long";
        case 0x09:
            return "size=big1 duration=indef";
        case 0x0a:
            return "size=big2 duration=indef";
        case 0x0b:
            return "size=big3 duration=indef";
        case 0x0c:
            return "size=small1 duration=short";
        case 0x0d:
            return "size=small2 duration=short";
        case 0x0e:
            return "size=small3 duration=short";
        case 0x0f:
            return "size=small1 duration=long";
        case 0x10:
            return "size=small2 duration=long";
        case 0x11:
            return "size=small3 duration=long";
        case 0x12:
            return "size=small1 duration=indef";
        case 0x13:
            return "size=small2 duration=indef";
        case 0x14:
            return "size=small3 duration=indef";
        case 0x15:
            return "type=rise duration=long";
        case 0x16:
            return "type=rise duration=indef";
        case 0x17:
            return "type=jump1";
        case 0x18:
            return "type=jump2";
        case 0x19:
            return "type=jump3";
        default:
            return "";
    }    
}
