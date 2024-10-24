#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <deque>
#include <unordered_map>
#include <cstring>

#define LOOP_COUNT_BLOCK 29998

std::string get_file_contents(const char *filename)
{
    std::ifstream myfile(filename);
    std::stringstream strStream;
    strStream << myfile.rdbuf();
    std::string content = strStream.str();
    return content;
}

void filterProgram(const char *text, char *program)
{
    std::string allowed = "><+-.,[]";
    while (*text)
    {
        if (allowed.find(*text) != std::string::npos)
        {
            if (*text == '[' && *(text + 1) == '-' && *(text + 2) == ']')
            {
                *program = 'N';
                text++;
                text++;
            }
            else if (*text == '+')
            {
                uint8_t dupes = 0;

                while (*text == '+')
                {
                    dupes++;
                    text++;
                }
                text--;

                if (dupes > 1)
                {
                    *program = 'P';
                    program++;
                    *program = dupes;
                }
                else
                {
                    *program = *text;
                }
            }
            else if (*text == '-')
            {
                uint8_t dupes = 0;

                while (*text == '-')
                {
                    dupes++;
                    text++;
                }
                text--;

                if (dupes > 1)
                {
                    *program = 'M';
                    program++;
                    *program = dupes;
                }
                else
                {
                    *program = *text;
                }
            }
            else
            {
                *program = *text;
            }
            program++;
        }
        text++;
    }
}

// Function to precompute jump locations for loops
void precomputeJumps(const char *program, std::unordered_map<int, int> &jumps)
{
    std::deque<int> stack;
    int position = 0;

    while (program[position] != '\0') // Traverse through the filtered program
    {
        if (program[position] == '[')
        {
            stack.push_back(position); // Track the position of the '['
        }
        else if (program[position] == ']')
        {
            if (stack.empty())
            {
                std::cerr << "Unmatched ']' at position " << position << std::endl;
                exit(1); // Exit with an error if no matching '['
            }
            int matchingBracket = stack.back();
            stack.pop_back();

            // Map the '[' and ']' to each other
            jumps[matchingBracket] = position;
            jumps[position] = matchingBracket;
        }
        position++;
    }

    if (!stack.empty())
    {
        std::cerr << "Unmatched '[' at position " << stack.back() << std::endl;
        exit(1); // Exit with an error if no matching ']'
    }
}

void executeCommand(char *&memory, std::unordered_map<int, int> &jumps, char *&program, char *programStart)
{
    int index = program - programStart; // Explicitly cast to int

    switch (*program)
    {
    case '>':
        memory++;
        break;
    case '<':
        memory--;
        break;
    case '+':
        (*memory)++;
        break;
    case '-':
        (*memory)--;
        break;
    case '.':
        putchar(*memory);
        break;
    case ',':
        *memory = getchar();
        break;
    case '[':
        if (*memory == 0)
        {
            program = programStart + jumps[index]; // Jump forward to matching ]
        }
        break;
    case ']':
        if (*memory != 0)
        {
            program = programStart + jumps[index]; // Jump back to matching [
        }
        break;
    case 'N':
        *memory = 0;
        break;
    case 'P':
        *memory += *(program + 1);
        program++;
        break;
    case 'M':
        *memory -= *(program + 1);
        program++;
        break;
    }
}

int main(int _, char *argv[])
{
    std::string code = get_file_contents(argv[1]);

    char *memory = (char *)malloc(30000 * sizeof(char));
    char *program = (char *)malloc(30000 * sizeof(char));
    char *programStart = program; // Save the original pointer for free later
    char *memoryStart = memory;   // Save the original pointer for free later

    memset(memory, 0, 30000);
    memset(program, 0, 30000);

    filterProgram(code.c_str(), program);

    // Precompute jump locations for loops
    std::unordered_map<int, int> jumps;
    precomputeJumps(program, jumps);

    while (*program)
    {
        executeCommand(memory, jumps, program, programStart);
        program++;
    }

    free(memoryStart);
    free(programStart);
}