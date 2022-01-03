#include "extractor.h"

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
        return 3;
    }
    
    if (!is_math_tbl(toc_header))
    {
        printf("Not an .EMI file!\n");
        fclose(area_file);
        return 4;
    }

    word file_count = convert_little_endian(toc_header, 0, 4);
    word address;
    word section_size;

    if ((address = find_dialogue_section(area_file, file_count, &section_size)) == 0)
    {
        printf("Dialogue section not found!\n");
        fclose(area_file);
        return 5;
    }
    fseek(area_file, address, SEEK_SET);

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL)
    {
        printf("Error opening output file\n");
        fclose(area_file);
        return 6;
    }    

    byte dialogue_section[section_size];
    if (fread(dialogue_section, 1, sizeof(dialogue_section), area_file) != sizeof(dialogue_section))
    {
        printf("Error reading file!\n");
        fclose(area_file);
        fclose(output_file);
        return 7;
    }

    word pointer_size = convert_little_endian(dialogue_section, 0, 2);
    word pointer;

    char punct;
    char last_color[8];
    char symbol[4];
    char box_position[20];
    int j;
    int opt_counter = 0;
    int in_opt = 0;

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

        while ((dialogue_section[j] != 0x00 || opt_counter > 0) && dialogue_section[j] != 0x16)
        {
            if (is_alphanum(dialogue_section[j]))
            {
                fprintf(output_file, "%c", dialogue_section[j]);
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
                        fprintf(output_file, "Ryu");
                        j++;                    
                        break;
                    case 0x01:
                        fprintf(output_file, "Nina");
                        j++;                    
                        break;
                    case 0x02:
                        fprintf(output_file, "Garr");
                        j++;                    
                        break;
                    case 0x03:
                        fprintf(output_file, "Teepo");
                        j++;                    
                        break;
                    case 0x04:
                        fprintf(output_file, "Rei");
                        j++;                    
                        break;
                    case 0x05:
                        fprintf(output_file, "Momo");
                        j++;                    
                        break;
                    case 0x06:
                        fprintf(output_file, "Peco");
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
                opt_counter = dialogue_section[j + 3] & 0xf;
                in_opt = 1;
                j += 3;
            }
            else if (strcmp(strcpy(symbol, is_symbol(dialogue_section[j])), "") != 0)
            {
                fprintf(output_file, "%s", symbol);
            }
            else if ((punct = is_punct(dialogue_section[j])) != 0x00)
            {
                fprintf(output_file, "%c", punct);
            }
            else if (dialogue_section[j] == 0x07)
            {
                fprintf(output_file, "[PLACEHOLDER]");
                j++;
            }
            else if (dialogue_section[j] == 0x00 && opt_counter > 0)
            {
                fprintf(output_file, "\n");
                opt_counter--;
            }

            if (in_opt == 1 && opt_counter == 0)
            {
                fprintf(output_file, "[/OPTION]");
                in_opt = 0;
                break;
            }

            j++;
        }

        fprintf(output_file, "\n==================================================== [%i (0x%04x)]\n", i, pointer);
    }

    fclose(area_file);
    fclose(output_file);
    return 0;
}
