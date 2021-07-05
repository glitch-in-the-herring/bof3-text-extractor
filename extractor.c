#include "extractor.h"

int main (int argc, char *argv[])
{
    if (argc < 3)
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
        printf("Failed to allocate memory!");
        return 5;
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
            return 4;
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
                    printf("Failed to allocate memory!");
                    return 5;
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

    fclose(area_file);
    fclose(output_file);
    free_node(chunk_chain);
}

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
