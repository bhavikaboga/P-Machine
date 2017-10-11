//Homework 1
//Programmers: Bhavika Boga, Michael Vigil

#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct instruction {
    int op; // Refers to opcode
    int r; // Refers to a register
    int l; // Refers to the lexicographical level or a register in arithmetic or logic instructions
    int m; // Refers a number/program address/data address/register in arithmetic or logic instruction
} instruction;

typedef struct CPU {
    int PC; // Refers to program counter
    int BP; // Refers to base pointer
    int SP; // Refers to the stack pointer
    int RF[8]; // Refers to register file with registers from R0 to R7
    instruction IR; // Refers to the instruction register

} CPU;

// Function declarations
void createCPU(int stack[], int instructs[][4], CPU *cpu);
void readInput(char *argv[], int instructs[][4]);
char *opcodeToName (int op);
int findBase(int stack[], int l, int base);
void print(CPU *cpu, int levels[], int stack[]);

int main(int argc, char *argv[])
{
    int instructs[MAX_CODE_LENGTH][4]; // Stores code from file
    int levels[MAX_LEXI_LEVELS]; // Stores base pointers
    int stack[MAX_STACK_HEIGHT]; // Creates stack
    int levelCounter = 0; // Counts how many base pointers are saved
    char *opName; // Variable to store name of an opCode
    CPU cpu; // Creates CPU

    // Initializing loop counters
    int i = 0;
    int j = 0;


    // Initializes everything needed
    createCPU( stack, instructs, &cpu);

    // Initializes array that keeps track of base pointers as new activation records get created
    for( i = 0; i < MAX_LEXI_LEVELS; i++ )
        levels[ i ] = 0;

    // Instructions get read into a 2D array
    readInput(argv, instructs);
    i = 0;
    printf("Factorial OP Printout:\n\n");
    // Prints a numbered list of the code in assembly language format
    while (!(instructs[i][0] == 0 && instructs[i][1] == 0 && instructs[i][2] == 0 && instructs[i][3] == 0))
    {
        // Converts op code to name
        opName = opcodeToName( instructs[i][0]);
        printf("%d   %s  %d   %d   %d\n", i , opName, instructs[i][1], instructs[i][2], instructs[i][3]);
        i++;
    }

    printf("\n\nFactorial Stack Trace:\n");
    printf("\nInitial Values\t   PC\t   BP\t   SP\tStack\n" );

    while( !(cpu.PC == 0 && cpu.BP == 0 && cpu.SP == 0))
    {

        // Instruction is fetched from code and stored in instruction register (Fetch Cycle)
        cpu.IR.op = instructs[cpu.PC][0];
        cpu.IR.r = instructs[cpu.PC][1];
        cpu.IR.l = instructs[cpu.PC][2];
        cpu.IR.m = instructs[cpu.PC][3];

        // Prints instructions like it did before
        opName = opcodeToName(cpu.IR.op);
        printf ("%d  %s %d %d %d ", cpu.PC, opName, instructs[cpu.PC][1], instructs[cpu.PC][2], instructs[cpu.PC][3]);

        // PC now points to the next instruction (Fetch Cycle)
        cpu.PC++;

        switch(cpu.IR.op)
        {
            // Loads constant value M into Register R
            case 1:
                cpu.RF[cpu.IR.r] = cpu.IR.m;
                break;

            // Returns from a subroutine
            case 2:
                // Restores called environment
                cpu.SP = cpu.BP - 1;
                cpu.BP = stack[cpu.SP + 3];
                cpu.PC = stack[cpu.SP + 4];
                // Keeps track of Activation Record levels
                levels[levelCounter] = 0;
                levelCounter--;
                break;

            // Loads value into a selected register from stack location at offset m from l levels down
            case 3:
                cpu.RF[cpu.IR.r] = stack[findBase(stack, cpu.IR.l, cpu.BP) + cpu.IR.m];
                break;

            // Stores value from a selected register into stack location at offset m from l levels down
            case 4:
                stack[findBase(stack, cpu.IR.l, cpu.BP) + cpu.IR.m] = cpu.RF[cpu.IR.r];
                break;

            // Call procedure at code index M, generates new AR and pc <- m
            case 5:
                stack[cpu.SP + 1] = 0; // Space to return value
                stack[cpu.SP + 2] = findBase(stack, cpu.IR.l, cpu.BP); // Static link
                stack[cpu.SP + 3] = cpu.BP; // Dynamic link
                stack[cpu.SP + 4] = cpu.PC; // Return address
                cpu.BP = cpu.SP + 1;
                cpu.PC = cpu.IR.m;
                //Keeps track of activation record levels
                levels[levelCounter] = cpu.SP + 1;
                levelCounter++;
                break;

            // Allocates m locals by incrementing stack pointer by m
            case 6:
                cpu.SP += cpu.IR.m;
                break;

            // Jumps to m instruction
            case 7:
                cpu.PC = cpu.IR.m;
                break;

            //Cpu pc set to int m iff RF[r] is 0
            case 8:
                if (cpu.RF[cpu.IR.r] == 0)
                {
                    cpu.PC = cpu.IR.m;
                }
                break;

            //SIO output RF[r], input and halt
            case 9:
                if(cpu.IR.m == 1)
                {
                    print(&cpu, levels, stack);
                    printf("RF[0]: %d\n", cpu.RF[cpu.IR.r]);
                    continue;
                }

                else if(cpu.IR.m == 2)
                {
                    scanf("%d", &cpu.RF[cpu.IR.r]);
                    break;
                }

                else if(cpu.IR.m == 3)
                    print(&cpu, levels, stack);

                    printf("\n\nFactorial Output:\n\n%d\n", cpu.RF[0]);
                    return 0;

            //set RF[r] to the opposite value of RF[l]
            case 10:
                cpu.RF[cpu.IR.r] = -(cpu.RF[cpu.IR.l]);
                break;


            //add RF values of element m and l
            case 11:
                cpu.RF[cpu.IR.r] = cpu.RF[cpu.IR.l] + cpu.RF[cpu.IR.m];
                break;

            //subtract RF l from element m
            case 12:
                cpu.RF[cpu.IR.r] = cpu.RF[cpu.IR.l] - cpu.RF[cpu.IR.m];
                break;

            //multiply RF elements m and l
            case 13:
                cpu.RF[cpu.IR.r] = cpu.RF[cpu.IR.l] * cpu.RF[cpu.IR.m];
                break;

            //divide RF element m from l
            case 14:
                cpu.RF[cpu.IR.r] = cpu.RF[cpu.IR.l] / cpu.RF[cpu.IR.m];
                break;

            //mod 2 operator to make odd
            case 15:
                cpu.RF[cpu.IR.r] = cpu.RF[cpu.IR.r] % 2;
                break;

            // mod operator
            case 16:
                cpu.RF[cpu.IR.r] = cpu.RF[cpu.IR.l] % cpu.RF[cpu.IR.m];
                break;

            //eql RF[r] set to 1 if element l and m are equal, 0 otherwise
            case 17:
                cpu.RF[cpu.IR.r] = (cpu.RF[cpu.IR.l] == cpu.RF[cpu.IR.m]) ? 1 : 0;
                break;

            //neq comparing RF element r and l
            case 18:
                cpu.RF[cpu.IR.r] = (cpu.RF[cpu.IR.l] != cpu.RF[cpu.IR.m]) ? 1 : 0;
                break;

            //lss comparing if RF element l is less than RF element m
            case 19:
                cpu.RF[cpu.IR.r] = (cpu.RF[cpu.IR.l] < cpu.RF[cpu.IR.m]) ? 1 : 0;
                break;

            //leq comparing if RF element l is less than or equal to RF element m
            case 20:
                cpu.RF[cpu.IR.r] = (cpu.RF[cpu.IR.l] <= cpu.RF[cpu.IR.m]) ? 1 : 0;

            //gtr comparing if RF element l is greater than RF element m
            case 21:
                cpu.RF[cpu.IR.r] = (cpu.RF[cpu.IR.l] > cpu.RF[cpu.IR.m]) ? 1 : 0;
                break;

            //geq comparing if RF element l is greater than or equal to RF element m
            case 22:
                cpu.RF[cpu.IR.r] = (cpu.RF[cpu.IR.l] >= cpu.RF[cpu.IR.m]) ? 1 : 0;
                break;

        }

        // Prints the rest of the output
        print(&cpu, levels, stack);
    }

    return 0;

}

void print(CPU *cpu, int levels[], int stack[])
{
    // For looping
    int i;
    int j;

    // Prints PC, BP and SP for each instruction
    printf("\t%4d\t%4d\t%4d\t", cpu->PC, cpu->BP, cpu->SP);

    // Loops through each stack index
    for (i = 1; i <= cpu->SP; i++)
    {
        // Uses | to seperate activation records
        for (j = 0; j < MAX_LEXI_LEVELS; j++)
        {
            if (i > 1 && i == levels[j])
            {
                printf("|");
            }
        }
        // Prints the stack
        printf("%d", stack[i]);
    }

    // Skips to the next line once an instruction cycle is complete
    printf("\n");

    printf("RF: ");

    // Prints the register content
    for (j = 0; j < 8; j++)
    {
        printf("%d ", cpu->RF[j]);
    }
        printf("\n");
}

//function that finds base of stack based on l levels down
int findBase(int stack[], int l, int base)
{
    int b = base;

    while(l > 0)
    {
        b = stack[b + 1];
        l--;
    }

    return b;
}

// Reads input from a file and stores it into a 2D array
void readInput(char *argv[], int instructs[][4])
{
    int i = 0;

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
        // Reads instructions until file ends
        while(!feof(fpt))
        {
            fscanf(fpt, "%d %d %d %d", &instructs[i][0], &instructs[i][1], &instructs[i][2], &instructs[i][3]);
            i++;
        }
    }
    // File is closed after being read
    fclose(fpt);
}

void createCPU(int stack[], int instructs[][4], CPU *cpu)
{
    // Declaring variables for looping
    int i = 0;
    int j = 0;

    cpu->PC = 0; // Initializes program counter
    cpu->BP = 1; // Initializes base pointer
    cpu->SP = 0; // Initializes stack pointer

    //Initializes instruction register
    cpu->IR.op = 0; // Initializes op code
    cpu->IR.r = 0; // Initializes register
    cpu->IR.l = 0; // Initializes lexicographical level
    cpu->IR.m = 0; // Initializes M

    // Initializes register file
    for(i = 0; i < 8; i++)
    {
        cpu->RF[i] = 0;
    }

    // Initializes stack array
    for(i = 0; i < MAX_STACK_HEIGHT; i++)
    {
        stack[i] = 0;
    }

    // Initializes 2D array storing incoming code
    for(i = 0; i < MAX_CODE_LENGTH; i++)
    {
        for(j = 0; j < 4; j++)
        {
            instructs[i][j] = 0;
        }
    }
}

// Takes the op code and returns the corresponding name
char *opcodeToName (int op)
{
    char *name;
    switch (op)
    {
        case 1:
            name = "lit";
            break;

        case 2:
            name = "rtn";
            break;

        case 3:
            name = "lod";
            break;

        case 4:
            name = "sto";
            break;

        case 5:
            name = "cal";
            break;

        case 6:
            name = "inc";
            break;

        case 7:
            name = "jmp";
            break;

        case 8:
            name = "jpc";
            break;

        case 9:
            name = "sio";
            break;

        case 10:
            name = "neg";
            break;

        case 11:
            name = "add";
            break;

        case 12:
            name = "sub";
            break;

        case 13:
            name = "mul";
            break;

        case 14:
            name = "div";
            break;

        case 15:
            name = "odd";
            break;

        case 16:
            name = "mod";
            break;

        case 17:
            name = "eql";
            break;

        case 18:
            name = "neq";
            break;

        case 19:
            name = "lss";
            break;

        case 20:
            name = "leq";
            break;

        case 21:
            name = "gtr";
            break;

        case 22:
            name = "geq";
            break;

        default:
            name = NULL;
            break;
     }

     return name;
}
