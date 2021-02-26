// gen_rand.cpp : Defines the entry point for the console application.
//

// Remembering to define _CRT_RAND_S prior
// to inclusion statement.
#define _CRT_RAND_S

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	// This program illustrates how to generate random
	// integer or floating point numbers in a specified range.

    unsigned int    i;
    unsigned int    number = 0;
	unsigned int	numbytes = 0;
    int				ret;
	FILE			*pFile = NULL;

	// Read in the amount of bytes to generate
	if (argc != 2)
	{
         /* Print out usage */
         printf("-----------------------------------------------\n");
         printf("Usage: gen_rand [num_bytes]\n");
         return 0;
	}

	else
	{
		numbytes = strtol( argv[1], NULL, 10);
		// pad up to 4 byte boundry
		if ((numbytes % sizeof(number) != 0))
		{
			numbytes = ((numbytes + sizeof(number)) & ~0x3);
		}
	}

	// Go ahead and open a file
	pFile = fopen("rand_seed.dat", "w+");
	if (pFile == NULL)
    {
		printf("Could not create file\n");
		return 1;
	}
   
    for (i = 0; i < numbytes; i+=sizeof(number))
    {
        ret = rand_s(&number);
        if (ret != 0)
        {
            printf("The rand_s function failed!\n");
			return 1;
        }
		fwrite((void*)&number, sizeof(number), 1, pFile);
    }

	return 0;
}

