#include <chrono>
#include <cstdint>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

#define PROGRAM_SIZE 30000

std::string get_file_contents(const char *filename) {
  std::ifstream myfile(filename);
  std::stringstream strStream;
  strStream << myfile.rdbuf();
  std::string content = strStream.str();
  return content;
}

void filterProgram(const char *text, char *program) {
  std::string allowed = "><+-.,[]";
  while (*text) {
    if (allowed.find(*text) != std::string::npos) {
      uint8_t dupes = 0;

      if (*text == '[' && *(text + 1) == '-' && *(text + 2) == ']' &&
          *(text + 3) == '>') {
        *program = 'C';
        program++;
        text += 4;
        dupes = 1;

        while (*text == '[' && *(text + 1) == '-' && *(text + 2) == ']' &&
               *(text + 3) == '>') {
          dupes += 1;
          text += 4;
        }
        text--;

        *program = dupes;
      } else if (*text == '[' && *(text + 1) == '-' && *(text + 2) == ']') {
        *program = 'N';
        text++;
        text++;
      } else if (*text == '[' && *(text + 1) == '-' && *(text + 2) == '>' &&
                 *(text + 3) == '+' && *(text + 4) == '<' &&
                 *(text + 5) == ']') {
        *program = 'S';
        text += 5;
      } else if (*text == '+') {
        while (*text == '+') {
          dupes++;
          text++;
        }
        text--;

        if (dupes > 1) {
          *program = 'P';
          program++;
          *program = dupes;
        } else {
          *program = *text;
        }
      } else if (*text == '-') {
        while (*text == '-') {
          dupes++;
          text++;
        }
        text--;

        if (dupes > 1) {
          *program = 'M';
          program++;
          *program = dupes;
        } else {
          *program = *text;
        }
      } else if (*text == '>') {
        while (*text == '>') {
          dupes++;
          text++;
        }
        text--;

        if (dupes > 1) {
          *program = 'R';
          program++;
          *program = dupes;
        } else {
          *program = *text;
        }
      } else if (*text == '<') {
        while (*text == '<') {
          dupes++;
          text++;
        }
        text--;

        if (dupes > 1) {
          *program = 'L';
          program++;
          *program = dupes;
        } else {
          *program = *text;
        }
      } else {
        *program = *text;
      }
      program++;
    }
    text++;
  }
}

// Function to precompute jump locations for loops
void precomputeJumps(const char *program, int *jumps, int programSize) {
  // Initialize jumps array with -1 to indicate no jump
  for (int i = 0; i < programSize; i++) {
    jumps[i] = -1;
  }

  std::deque<int> stack;
  int position = 0;

  while (program[position] != '\0') {
    if (program[position] == '[') {
      stack.push_back(position);
    } else if (program[position] == ']') {
      if (stack.empty()) {
        std::cerr << "Unmatched ']' at position " << position << std::endl;
        exit(1);
      }
      int matchingBracket = stack.back();
      stack.pop_back();

      // Store the positions
      jumps[matchingBracket] = position;
      jumps[position] = matchingBracket;
    }
    position++;
  }

  if (!stack.empty()) {
    std::cerr << "Unmatched '[' at position " << stack.back() << std::endl;
    exit(1);
  }
}

void executeCommand(char *&memory, int *&jumps, char *&program,
                    char *programStart) {
  int index = program - programStart; // Explicitly cast to int

  switch (*program) {
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
    if (*memory == 0) {
      program = programStart + jumps[index]; // Jump forward to matching ]
    }
    break;
  case ']':
    if (*memory != 0) {
      program = programStart + jumps[index]; // Jump back to matching [
    }
    break;
  case 'N':
    *memory = 0;
    break;
  case 'P':
    *memory += *(++program);
    break;
  case 'M':
    *memory -= *(++program);
    break;
  case 'R':
    memory += *(++program);
    break;
  case 'L':
    memory -= *(++program);
    break;
  case 'S':
    *(memory + 1) = *memory;
    *memory = 0;
    break;
  case 'C':
    memset(memory, 0, *(++program));
    memory += *program;
  }
}

int main(int _, char *argv[]) {
  auto start = std::chrono::high_resolution_clock::now();

  std::string code = get_file_contents(argv[1]);

  char *memory = (char *)malloc(PROGRAM_SIZE * sizeof(char));
  char *program = (char *)malloc(PROGRAM_SIZE * sizeof(char));
  char *programStart = program; // Save the original pointer for free later
  char *memoryStart = memory;   // Save the original pointer for free later

  memset(memory, 0, PROGRAM_SIZE);
  memset(program, 0, PROGRAM_SIZE);

  filterProgram(code.c_str(), program);

  // Precompute jump locations for loops
  int *jumps = (int *)malloc(PROGRAM_SIZE * sizeof(int));
  precomputeJumps(program, jumps, PROGRAM_SIZE);

  while (*program) {
    executeCommand(memory, jumps, program, programStart);
    program++;
  }

  free(memoryStart);
  free(programStart);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  printf("Program time: %.6f seconds\n", elapsed.count());
}