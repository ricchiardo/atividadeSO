#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS_SWAP 100
#define COLS_SWAP 6
#define ROWS_RAM 10
#define COLS_RAM 6

int swap[ROWS_SWAP][COLS_SWAP];
int ram[ROWS_RAM][COLS_RAM];

int instructions_executed = 0;
int total_instructions = 1000;
int restart_r_interval = 10;

int choosePageToReplace(int algorithm)
{

    if (algorithm == 0)
    { // NRU
        int class0 = -1, class1 = -1, class2 = -1, class3 = -1;

        for (int i = 0; i < ROWS_RAM; i++)
        {
            if (ram[i][3] == 0 && ram[i][4] == 0)
            {
                class0 = i;
            }
            else if (ram[i][3] == 0 && ram[i][4] == 1)
            {
                class1 = i;
            }
            else if (ram[i][3] == 1 && ram[i][4] == 0)
            {
                class2 = i;
            }
            else if (ram[i][3] == 1 && ram[i][4] == 1)
            {
                class3 = i;
            }
        }
        if (class0 != -1)
            return class0;
        if (class1 != -1)
            return class1;
        if (class2 != -1)
            return class2;
        return class3;
    }

    else if (algorithm == 1)
    { // FIFO

        static int index = 0;
        int page_to_replace = index;
        index = (index + 1) % ROWS_RAM;
        return page_to_replace;
    }

    else if (algorithm == 2)
    { // FIFO-SC

        static int index = 0;

        while (1)
        {
            if (ram[index][3] == 0)
            {
                int page_to_replace = index;
                index = (index + 1) % ROWS_RAM;
                return page_to_replace;
            }
            else
            {
                ram[index][3] = 0;
                index = (index + 1) % ROWS_RAM;
            }
        }
    }

    else if (algorithm == 3)
    { // CLOCK

        static int index = 0;
        while (1)
        {
            if (ram[index][3] == 0)
            {
                int page_to_replace = index;
                index = (index + 1) % ROWS_RAM;
                return page_to_replace;
            }
            else
            {
                ram[index][3] = 0;
                index = (index + 1) % ROWS_RAM;
            }
        }
    }

    else if (algorithm == 4)
    { // WS-CLOCK

        int min_age = ram[0][5];
        int page_to_replace = 0;

        for (int i = 1; i < ROWS_RAM; i++)
        {
            if (ram[i][5] < min_age)
            {
                min_age = ram[i][5];
                page_to_replace = i;
            }
        }
        return page_to_replace;
    }
    return 0;
}

void initializeSwapMatrix()
{
    for (int i = 0; i < ROWS_SWAP; i++)
    {
        swap[i][0] = i;
        swap[i][1] = i + 1;
        swap[i][2] = rand() % 50 + 1;
        swap[i][3] = 0;
        swap[i][4] = 0;
        swap[i][5] = rand() % 9900 + 100;
    }
}

void initializeRamMatrix()
{
    for (int i = 0; i < ROWS_RAM; i++)
    {
        int randomIndex = rand() % ROWS_SWAP;
        for (int j = 0; j < COLS_RAM; j++)
        {
            ram[i][j] = swap[randomIndex][j];
        }
    }
}

void updateModifiedBit(int page_index)
{
    if (ram[page_index][4] == 1)
    {
        swap[ram[page_index][0]][2] = ram[page_index][2];
        swap[ram[page_index][0]][4] = 0;
    }
}

void updateAging(int EP)
{
    for (int i = 0; i < ROWS_RAM; i++)
    {
        if (ram[i][3] == 0)
        {
            ram[i][5] += 1;
        }
        if (ram[i][5] > EP)
        {
            updateModifiedBit(i);
            ram[i][1] = 0;
        }
    }
}

void printRamMatrix()
{
    printf("Matriz RAM:\n");
    for (int i = 0; i < ROWS_RAM; i++)
    {
        for (int j = 0; j < COLS_RAM; j++)
        {
            printf("%d ", ram[i][j]);
        }
        printf("\n");
    }
}

void printSwapMatrix()
{
    printf("Matriz SWAP:\n");
    for (int i = 0; i < ROWS_SWAP; i++)
    {
        for (int j = 0; j < COLS_SWAP; j++)
        {
            printf("%d ", swap[i][j]);
        }
        printf("\n");
    }
}

void simulateCpuExecution(int algorithm)
{
    int instruction = rand() % 100 + 1;
    int found = 0;

    for (int i = 0; i < ROWS_RAM; i++)
    {
        if (ram[i][1] == instruction)
        {
            ram[i][3] = 1;
            if ((double)rand() / RAND_MAX < 0.3)
            {
                ram[i][2]++;
                ram[i][4] = 1;
            }
            found = 1;
            break;
        }
    }

    if (!found)
    {
        int page_to_replace = choosePageToReplace(algorithm);
        updateModifiedBit(page_to_replace);
        ram[page_to_replace][1] = instruction;
        ram[page_to_replace][3] = 1;
        ram[page_to_replace][4] = 0;

        instructions_executed++;

        if (instructions_executed % restart_r_interval == 0)
        {
            for (int i = 0; i < ROWS_RAM; i++)
            {
                ram[i][3] = 0;
            }
        }
    }
}

void simulateCpuInstructions(int algorithm, int EP)
{
    while (instructions_executed < total_instructions)
    {
        simulateCpuExecution(algorithm);
        if (algorithm == 4)
        {
            updateAging(EP);
        }
    }
}

void runPageReplacementAlgorithm(int algorithm, int EP)
{

    initializeSwapMatrix();
    initializeRamMatrix();
    instructions_executed = 0;
    simulateCpuInstructions(algorithm, EP);
    printf("Matriz RAM ao final (Algoritmo %d):\n", algorithm);
    printRamMatrix();
}

int main()
{
    srand(time(NULL));
    initializeSwapMatrix();
    initializeRamMatrix();
    printf("Matriz RAM no início:\n");
    printRamMatrix();
    printf("Matriz SWAP no início:\n");
    printSwapMatrix();

    for (int algorithm = 0; algorithm < 5; algorithm++)
    {
        int EP = rand() % 9900 + 100;
        runPageReplacementAlgorithm(algorithm, EP);
    }

    printf("Matriz RAM ao final:\n");
    printRamMatrix();

    printf("Matriz SWAP ao final:\n");
    printSwapMatrix();

    return 0;
}