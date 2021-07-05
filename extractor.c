#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef int8_t byte;

bool is_math_tbl(byte chunk[]);
bool is_first_prepadding(byte chunk[]);
bool is_second_prepadding(byte chunk[]);
bool is_final_chunk(byte chunk[]);

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./extractor input_file output_file\n");
        return 1;
    }

    FILE *area_file = fopen(argv[1], "r");
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
    bool final_chunk = false;
    bool first_chunk = true;

    while (fread(chunk, sizeof(chunk), 1, area_file) > 0 && !final_chunk)
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
        }
    }

    fclose(area_file);
    fclose(output_file);
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
    //TODO
}

bool is_second_prepadding(byte chunk[])
{
    //TODO
}

bool is_final_chunk(byte chunk[])
{
    //TODO
}
