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

    word file_count = convert_little_endian(toc_header, 0, 4);
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

    word pointer_size = convert_little_endian(dialogue_section, 0, 2);
    word pointer;

    byte kanji_0, kanji_1;
    int kanji_bytes;
    char punct[4];
    char last_color[8];
    char box_position[20];
    int j;

    for (int i = 0; i < pointer_size; i++)
    {
        pointer = convert_little_endian(dialogue_section, i * 2, 2);
        j = pointer;
        if (pointer == pointer_size && i != 0)
        {
            continue;
        }
        else if (pointer == section_size)
        {
            break;
        }

        while (dialogue_section[j] != 0x00 && dialogue_section[j] != 0x16)
        {
            if (is_hiragana(dialogue_section[j]))
            {
                fprintf(output_file, "%s", hiragana_table[dialogue_section[j] - HRSTART]);
            }
            else if (is_katakana(dialogue_section[j]))
            {
                fprintf(output_file, "%s", katakana_table[dialogue_section[j] - KTSTART]);
            }
            else if (is_kanji_start(dialogue_section[j]))
            {
                kanji_0 = dialogue_section[j];
                kanji_1 = dialogue_section[j + 1];
                kanji_bytes = (kanji_0 << 8) | kanji_1;
                fprintf(output_file, "%s", kanji_table[kanji_bytes - KJSTART]);
                j++;
            }
            else if (dialogue_section[j] == 0x0c)
            {
                strcpy(box_position, is_position(dialogue_section[j + 1]));
                fprintf(output_file, "%s", box_position);
                j++;
            }
            else if (dialogue_section[j] == 0xff)
            {
                fprintf(output_file, " ");
            }
            else if (dialogue_section[j] == 0x0d)
            {
                fprintf(output_file, "[EFFECT]");
            }        
            else if (dialogue_section[j] == 0x01)
            {
                fprintf(output_file, "\n");
            }
            else if (dialogue_section[j] == 0x0b)
            {
                fprintf(output_file, "--");
            }
            else if (dialogue_section[j] == 0x02)
            {
                fprintf(output_file, "\n        ▼        \n");
            }
            else if (dialogue_section[j] == 0x0f && dialogue_section[j - 1] == 0x0e)
            {
                fprintf(output_file, "[/EFFECT %s]", is_effect(dialogue_section[j + 1]));
                j += 2;
            }
            else if (dialogue_section[j] == 0x04)
            {
                switch (dialogue_section[j + 1])
                {
                    case 0x00:
                        fprintf(output_file, "リュウ");
                        j++;
                        break;
                    case 0x01:
                        fprintf(output_file, "ニーナ");
                        j++;
                        break;
                    case 0x02:
                        fprintf(output_file, "ガーランド");
                        j++;
                        break;
                    case 0x03:
                        fprintf(output_file, "ティーポ");
                        j++;
                        break;
                    case 0x04:
                        fprintf(output_file, "レイ");
                        j++;
                        break;
                    case 0x05:
                        fprintf(output_file, "モモ");
                        j++;
                        break;
                    case 0x06:
                        fprintf(output_file, "ペコロス");
                        j++;
                        break;
                    default:
                        break;
                }
            }
            else if (dialogue_section[j] == 0x05)
            {
                strcpy(last_color, is_color(dialogue_section[j + 1]));
                fprintf(output_file, "[%s]", last_color);
                j++;
            }
            else if (dialogue_section[j] == 0x06)
            {
                fprintf(output_file, "[/%s]", last_color);
            }
           else if (dialogue_section[j] == 0x14)
            {
                fprintf(output_file, "\n[OPTIONS]\n");
                j += 2;
            }
            else if (dialogue_section[j] == 0x15)
            {
                strcpy(punct, is_symbol(dialogue_section[j + 1]));        
                fprintf(output_file, "%s", punct);
                j++;
            }
            else if (is_alphanum(dialogue_section[j]))
            {
                fprintf(output_file, "%c", dialogue_section[j]);
            }
            else if (strcmp(strcpy(punct, is_punct(dialogue_section[j])), "") != 0)
            {
                fprintf(output_file, "%s", punct);
            }

            j++;
        }

        fprintf(output_file, "\n==================================================== [%i (0x%04x)]\n", i, pointer);
    }

    fclose(area_file);
    fclose(output_file);
}
