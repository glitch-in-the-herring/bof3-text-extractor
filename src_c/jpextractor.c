#include "jpextractor.h"

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
        return 3;
    }
    else if (output_file == NULL)
    {
        printf("Error opening output file\n");
        fclose(area_file);
        return 4;
    }
    
    byte chunk[512];
    node *chunk_chain = NULL;
    node *temp = calloc(1, sizeof(node));
    if (temp == NULL)
    {
        printf("Failed to allocate memory");
        remove(argv[2]);
        return 5;        
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
        return 6;
    }    

    FILE *hiragana_source = fopen("hiragana.txt", "r");
    FILE *katakana_source = fopen("katakana.txt", "r");
    FILE *kanji_source = fopen("kanji.txt", "r");
    char hiragana_table[80][4];
    char katakana_table[81][4];
    char kanji_table[441][4];
    load_lookup_table(4, hiragana_table, hiragana_source);
    load_lookup_table(4, katakana_table, katakana_source);
    load_lookup_table(4, kanji_table, kanji_source);
    fclose(hiragana_source);
    fclose(katakana_source);
    fclose(kanji_source);    

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

    char punct[4];
    byte kanji_0, kanji_1;
    int kanji_bytes;
    for (int i = 0; i < 512 * chunk_count - 1; i++)
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
            }            
        }
        else if ((dialogue_section[i] == 0x0d || dialogue_section[i] == 0xff) && !is_kanji_start(dialogue_section[i - 1]))
        {
            fprintf(output_file, " ");
        }
        else if (((dialogue_section[i - 1] != 0x04 && dialogue_section[i - 1] != 0x0f) && dialogue_section[i] == 0x01) && !is_kanji_start(dialogue_section[i - 1]))
        {
            fprintf(output_file, "\n");
        }
        else if (dialogue_section[i] == 0x0b && !is_kanji_start(dialogue_section[i - 1]))
        {
            fprintf(output_file, "--");
        }
        else if (((dialogue_section[i] == 0x00 && (dialogue_section[i - 1] != 0x04 && dialogue_section[i - 1] != 0x0f)) || dialogue_section[i] == 0x20) && !is_kanji_start(dialogue_section[i - 1]))
        {
            fprintf(output_file, "\n\n--------------------\n");
        }
        else if ((dialogue_section[i] == 0x02 && (dialogue_section[i - 1] != 0x04 && dialogue_section[i - 1] != 0x0f)) && !is_kanji_start(dialogue_section[i - 1]))
        {
            fprintf(output_file, "\n⬇\n");
        }
        else if ((dialogue_section[i] == 0x0f && (dialogue_section[i + 1] <= 0x09 && dialogue_section[i + 1] >= 0x01)) && !is_kanji_start(dialogue_section[i - 1]))
        {
            fprintf(output_file, " ");
        }
        else if ((dialogue_section[i] == 0x04 && dialogue_section[i - 1] != 0x0f) && !is_kanji_start(dialogue_section[i - 1]))
        {
            switch (dialogue_section[i + 1])
            {
                case 0x00:
                    fprintf(output_file, "リュウ");
                    break;
                case 0x01:
                    fprintf(output_file, "ニーナ");
                    break;
                case 0x02:
                    fprintf(output_file, "ガーランド");
                    break;
                case 0x03:
                    fprintf(output_file, "テイーポ");
                    break;
                case 0x04:
                    fprintf(output_file, "レイ");
                    break;
                case 0x05:
                    fprintf(output_file, "モモ");
                    break;
                case 0x06:
                    fprintf(output_file, "ペコロス");
                    break;
            }
        }
        else if (dialogue_section[i] == 0x15 && dialogue_section[i + 1] == 0x07)
        {
            fprintf(output_file, "〜");
        }
        else if (strcmp(strcpy(punct, is_punct(dialogue_section[i])), "") != 0)
        {
            fprintf(output_file, "%s", punct);
        }
    }

    fclose(area_file);
    fclose(output_file);
}