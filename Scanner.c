#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33} token_type;

void main(int argc, char *argv[])
{
    // File to be read is opened
    FILE* fpt = fopen(argv[1], "r");

    // Checks if there is an error opening file
    if (fpt == NULL)
    {
        printf("Could not open file\n");
        return;
    }
    // If no error was found, the file is read into a 2D int array
    else
    {

    }

}
