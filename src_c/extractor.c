#include "extractor.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *area_file = fopen(argv[1], "rb");
    FILE *output_file = fopen(argv[2], "w");
    if (area_file == NULL)
    {
        printf("Error opening input file\n");
        fclose(output_file);
        remove(argv[2]);
        return 2;
    }
    else if (output_file == NULL)
    {
        printf("Error opening output file\n");
        fclose(area_file);
        return 3;
    }

    byte chunk[512];
    node *chunk_chain = NULL;
    node *temp = calloc(1, sizeof(node));
    if (temp == NULL)
    {
        printf("Failed to allocate memory\n");
        fclose(area_file);
        fclose(output_file);
        remove(argv[2]);
        return 4;
    }
    chunk_chain = temp;

    int chunk_count = 0;
    bool final_chunk = false;
    bool first_chunk = true;
    bool paddings_found[2] = {false, false};

    fread(chunk, 1, sizeof(chunk), area_file);
    if (!is_math_tbl(chunk))
    {
        printf("Not a valid .EMI file!\n");
        fclose(area_file);
        fclose(output_file);
        free_node(chunk_chain);
        remove(argv[2]);
        return 5;
    }

    while (fread(chunk, 1, sizeof(chunk), area_file) > 0 && !final_chunk)
    {
        if (!(paddings_found[0] && paddings_found[1]))
        {
            if (!paddings_found[1] && is_first_prepadding(chunk))
            {
                paddings_found[0] = true;
            }         
            else if (paddings_found[0] && is_second_prepadding(chunk))
            {
                paddings_found[1] = true;
            }
        }
        else
        {
            chunk_count++;
            temp->next = calloc(1, sizeof(node));
            if (temp->next == NULL)
            {
                printf("Failed to allocate memory\n");
                fclose(area_file);
                fclose(output_file);
                free_node(chunk_chain);
                remove(argv[2]);
                return 4;
            }
            copy_arrays(temp->chunk, chunk, 512);
            temp = temp->next;
            // Check if this is the final chunk.
            final_chunk = is_final_chunk(chunk);
        }
    }

    if (!(paddings_found[0] && paddings_found[1]))
    {
        printf("No dialogue section found in this .EMI file!\n");
        fclose(area_file);
        fclose(output_file);
        free_node(chunk_chain);
        remove(argv[2]);
        return 6;
    }

    byte dialogue_section[512 * chunk_count];
    int i = 0;
    for (node *n = chunk_chain; n->next != NULL; n = n->next)
    {
        for (int j = 0; j < 512; j++)
        {
            dialogue_section[512 * i + j] = n->chunk[j];
        }
        i++;
    }
    free_node(chunk_chain);

    char punct;
    char last_color[8];
    for (int i = 0; i < 512 * chunk_count - 1; i++)
    {
        if (is_alpha(dialogue_section[i]))
        {
            fprintf(output_file, "%c", dialogue_section[i]);
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
        else if (dialogue_section[i] == 0x00 || dialogue_section[i - 1] == 0x16 || dialogue_section[i] == 0x20)
        {
            fprintf(output_file, "\n\n--------------------\n");
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
                    fprintf(output_file, "Ryu");
                    i++;                    
                    break;
                case 0x01:
                    fprintf(output_file, "Nina");
                    i++;                    
                    break;
                case 0x02:
                    fprintf(output_file, "Garr");
                    i++;                    
                    break;
                case 0x03:
                    fprintf(output_file, "Teepo");
                    i++;                    
                    break;
                case 0x04:
                    fprintf(output_file, "Rei");
                    i++;                    
                    break;
                case 0x05:
                    fprintf(output_file, "Momo");
                    i++;                    
                    break;
                case 0x06:
                    fprintf(output_file, "Peco");
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
        else if ((punct = is_punct(dialogue_section[i])) != 0x00)
        {
            fprintf(output_file, "%c", punct);
        }  
    }

    fclose(area_file);
    fclose(output_file);
    return 0;
}
