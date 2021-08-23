#include "jpextractor.h"

int main(int argc, char argv[])
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
        remove(argv[2]);
        return 2;
    }
    else if (output_file == NULL)
    {
        printf("Error opening output file\n");
        remove(argv[2]);
        return 3;
    }

}