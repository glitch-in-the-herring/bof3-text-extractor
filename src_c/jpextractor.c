#include "jpextractor.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *area_file = fopen(argv[1], "rb");
    if (area_file == NULL)
    {
        printf("Error opening input file\n");
        return 2;
    }

    byte toc_header[16];
    if (fread(toc_header, 1, sizeof(toc_header), area_file) != sizeof(toc_header))
    {
        printf("Error reading file!\n");
        fclose(area_file);
        return 2;
    }

    if (!is_math_tbl(toc_header))
    {
        printf("Not an .EMI file!\n");
        fclose(area_file);
        return 3;
    }       

    word file_count = convert_little_endian(toc_header, 3, 0);
    word address;
    word section_size;

    if ((address = find_dialogue_section(area_file, file_count, &section_size)) == 0)
    {
        printf("Dialogue section not found!\n");
        fclose(area_file);
        return 4;
    }
    fseek(area_file, address, SEEK_SET);

    FILE *hiragana_source = fopen("hiragana.txt", "r");
    if (hiragana_source == NULL)
    {
        printf("Hiragana table not found!\n");
        fclose(area_file);
        return 6;
    }

    FILE *katakana_source = fopen("katakana.txt", "r");
    if (katakana_source == NULL)
    {
        printf("Katakana table not found!\n");
        fclose(hiragana_source);        
        fclose(area_file);
        return 7;
    }

    FILE *kanji_source = fopen("kanji.txt", "r");
    if (kanji_source == NULL)
    {
        printf("Kanji table not found!\n");
        fclose(hiragana_source);
        fclose(katakana_source);
        fclose(area_file);
        return 8;
    }    

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL)
    {
        printf("Error opening output file\n");
        fclose(hiragana_source);
        fclose(katakana_source);
        fclose(kanji_source);
        fclose(area_file);
        return 5;
    }

    byte dialogue_section[section_size];
    if (fread(dialogue_section, 1, sizeof(dialogue_section), area_file) != sizeof(dialogue_section))
    {
        printf("Error reading file!\n");
        fclose(hiragana_source);
        fclose(katakana_source);
        fclose(kanji_source);
        fclose(area_file);
        fclose(output_file);
        return 2;
    }

    char hiragana_table[80][4];
    char katakana_table[81][4];
    char kanji_table[441][4];
    load_lookup_table(4, hiragana_table, hiragana_source);
    load_lookup_table(4, katakana_table, katakana_source);
    load_lookup_table(4, kanji_table, kanji_source);
    fclose(hiragana_source);
    fclose(katakana_source);
    fclose(kanji_source); 

    byte kanji_0, kanji_1;
    int kanji_bytes;
    char punct[4];
    char last_color[8];
    for (int i = 0; i < section_size; i++)
    {
        if (is_hiragana(dialogue_section[i]))
        {
            fprintf(output_file, "%s", hiragana_table[dialogue_section[i] - HRSTART]);
        }
        else if (is_katakana(dialogue_section[i]))
        {
            fprintf(output_file, "%s", katakana_table[dialogue_section[i] - KTSTART]);
        }
        else if (is_kanji_start(dialogue_section[i]))
        {
            kanji_0 = dialogue_section[i];
            kanji_1 = dialogue_section[i + 1];
            kanji_bytes = (kanji_0 << 8) | kanji_1;
            fprintf(output_file, "%s", kanji_table[kanji_bytes - KJSTART]);
            i++;
        }
        else if (dialogue_section[i] == 0x0c)
        {
            switch (dialogue_section[i + 1])
            {
                case 0x00:
                    fprintf(output_file, "[BOX_BOTTOMM] ");
                    i++;
                    break;
                case 0x01:
                    fprintf(output_file, "[BOX_MIDM] ");
                    i++;
                    break;                 
                case 0x02:
                    fprintf(output_file, "[BOX_TOPM] ");
                    i++;
                    break;                    
                case 0x03:
                    fprintf(output_file, "[BOX_TOPL] ");
                    i++;
                    break;
                case 0x04:
                    fprintf(output_file, "[BOX_TOPR] ");
                    i++;
                    break;
                case 0x05:
                    fprintf(output_file, "[BOX_BOTTOML] ");
                    i++;
                    break;
                case 0x06:
                    fprintf(output_file, "[BOX_BOTTOMR] ");
                    i++;
                    break;
                default:
                    break;
            }            
        }
        else if (dialogue_section[i] == 0xff)
        {
            fprintf(output_file, " ");
        }
        else if (dialogue_section[i] == 0x0d)
        {
            fprintf(output_file, "[EFFECT]");
        }        
        else if (dialogue_section[i] == 0x01)
        {
            fprintf(output_file, "\n");
        }
        else if (dialogue_section[i] == 0x0b)
        {
            fprintf(output_file, "--");
        }
        else if (dialogue_section[i] == 0x00 || dialogue_section[i] == 0x20)
        {
            fprintf(output_file, "\n\n--------------------\n");
        }
        else if (dialogue_section[i] == 0x16)
        {
            fprintf(output_file, "\n\n--------------------\n");
            i++;
        }
        else if (dialogue_section[i] == 0x02)
        {
            fprintf(output_file, "\n        ▼        \n");
        }
        else if (dialogue_section[i] == 0x0f && dialogue_section[i - 1] == 0x0e)
        {
            fprintf(output_file, "[/EFFECT %s]", is_effect(dialogue_section[i + 1]));
            i += 2;
        }
        else if (dialogue_section[i] == 0x04)
        {
            switch (dialogue_section[i + 1])
            {
                case 0x00:
                    fprintf(output_file, "リュウ");
                    i++;
                    break;
                case 0x01:
                    fprintf(output_file, "ニーナ");
                    i++;
                    break;
                case 0x02:
                    fprintf(output_file, "ガーランド");
                    i++;
                    break;
                case 0x03:
                    fprintf(output_file, "ティーポ");
                    i++;
                    break;
                case 0x04:
                    fprintf(output_file, "レイ");
                    i++;
                    break;
                case 0x05:
                    fprintf(output_file, "モモ");
                    i++;
                    break;
                case 0x06:
                    fprintf(output_file, "ペコロス");
                    i++;
                    break;
                default:
                    break;
            }
        }
        else if (dialogue_section[i] == 0x05)
        {
            strcpy(last_color, is_color(dialogue_section[i + 1]));
            fprintf(output_file, "[%s]", last_color);
            i++;
        }
        else if (dialogue_section[i] == 0x06)
        {
            fprintf(output_file, "[/%s]", last_color);
        }
       else if (dialogue_section[i] == 0x14)
        {
            fprintf(output_file, "\n[OPTIONS]\n");
            i += 2;
        }
        else if (dialogue_section[i] == 0x15 && dialogue_section[i + 1] == 0x07)
        {
            fprintf(output_file, "〜");
            i++;
        }
        else if (strcmp(strcpy(punct, is_punct(dialogue_section[i])), "") != 0)
        {
            fprintf(output_file, "%s", punct);
        }
    }

    fclose(area_file);
    fclose(output_file);
}
