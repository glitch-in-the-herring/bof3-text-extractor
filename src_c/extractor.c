#include "extractor.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./extractor input_file output_file\n");
        return 1;
    }

    FILE *area_file = fopen(argv[1], "rb");
    FILE *output_file = fopen(argv[2], "w");

    if (area_file == NULL)
    {
        printf("Error opening input file");
        return 2;
    }
    else if (output_file == NULL)
    {
        printf("Error opening output file");
        return 3;
    }

    byte chunk[512];
    node *chunk_chain;
    node *temp = calloc(1, sizeof(node));
    if (temp == NULL)
    {
        printf("Failed to allocate memory");
        return 4;
    }
    chunk_chain = temp;

    int chunk_count = 0;
    bool final_chunk = false;
    bool first_chunk = true;
    bool paddings_found[2] = {false, false};

    while (fread(chunk, 1, sizeof(chunk), area_file) > 0 && !final_chunk)
    {
        if (first_chunk && !is_math_tbl(chunk))
        {
            printf("Not a valid .EMI file!");
            fclose(area_file);
            fclose(output_file);
            return 5;
        }
        else
        {
            first_chunk = false;
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
                    printf("Failed to allocate memory");
                    return 4;
                }
                copy_arrays(temp->chunk, chunk, 512);
                temp = temp->next;
                final_chunk = is_final_chunk(chunk);
            }
        }
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
    for (int i = 0; i < 512 * chunk_count - 1; i++)
    {
        if (dialogue_section[i] == 0x0c)
        {
            switch (dialogue_section[i + 1])
            {
                case 0x03:
                    fprintf(output_file, "[BOX_TOPL] ");
                    break;
                case 0x04:
                    fprintf(output_file, "[BOX_TOPR] ");
                    break;
                case 0x05:
                    fprintf(output_file, "[BOX_BOTTOML] ");
                    break;
                case 0x06:
                    fprintf(output_file, "[BOX_BOTTOMR] ");
                    break;
            }
        }
        else if (dialogue_section[i] == 0x0d || dialogue_section[i] == 0xff)
        {
            fprintf(output_file, " ");
        }
        else if ((dialogue_section[i - 1] != 0x04 && dialogue_section[i - 1] != 0x0f) && dialogue_section[i] == 0x01)
        {
            fprintf(output_file, "\n");
        }
        else if (dialogue_section[i] == 0x0b)
        {
            fprintf(output_file, "--");
        }
        else if ((dialogue_section[i] == 0x00 && (dialogue_section[i - 1] != 0x04 && dialogue_section[i - 1] != 0x0f)) || dialogue_section[i] == 0x20)
        {
            fprintf(output_file, "\n\n--------------------\n");
        }
        else if (dialogue_section[i] == 0x02 && (dialogue_section[i - 1] != 0x04 && dialogue_section[i - 1] != 0x0f))
        {
            fprintf(output_file, "\nV\n");
        }
        else if (dialogue_section[i] == 0x0f && (dialogue_section[i + 1] <= 0x09 && dialogue_section[i + 1] >= 0x01))
        {
            fprintf(output_file, " ");
        }
        else if (dialogue_section[i] == 0x04 && dialogue_section[i - 1] != 0x0f)
        {
            switch (dialogue_section[i + 1])
            {
                case 0x00:
                    fprintf(output_file, "Ryu");
                    break;
                case 0x01:
                    fprintf(output_file, "Nina");
                    break;
                case 0x02:
                    fprintf(output_file, "Garr");
                    break;
                case 0x03:
                    fprintf(output_file, "Teepo");
                    break;
                case 0x04:
                    fprintf(output_file, "Rei");
                    break;
                case 0x05:
                    fprintf(output_file, "Momo");
                    break;
                case 0x06:
                    fprintf(output_file, "Peco");
                    break;
            }
        }
        else if ((punct = is_punct(dialogue_section[i])) != 0x00)
        {
            fprintf(output_file, "%c", punct);
        }
        else if (is_alpha(dialogue_section[i]))
        {
            fprintf(output_file, "%c", dialogue_section[i]);
        }
    }

    fclose(area_file);
    fclose(output_file);
    return 0;
}
