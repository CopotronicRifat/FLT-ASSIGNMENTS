#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATTERN_LENGTH 100
#define MAX_ALPHABET_SIZE 256  // You can adjust this size based on the expected range of input characters

void NDFSMBuilder(const char *fileName, const char *pattern) {
    if (strlen(pattern) == 0) {
        printf("Error: Pattern is empty.\n");
        return;
    }

    // Determine the alphabet from unique characters in the pattern
    int alphabetSet[MAX_ALPHABET_SIZE] = {0};
    for (int i = 0; i < strlen(pattern); i++) {
        alphabetSet[(unsigned char) pattern[i]] = 1;
    }

    // Number of states needed is pattern length + 1
    int numStates = strlen(pattern) + 1;

    // Create the NDFSM specification
    FILE *file = fopen(fileName, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Section 1: Write the alphabet (plus epsilon '$' at the end)
    for (int i = 0; i < MAX_ALPHABET_SIZE; i++) {
        if (alphabetSet[i]) {
            fprintf(file, "%c ", i);
        }
    }
    fprintf(file, "$\n\n");  // Include epsilon and separate sections

    // Section 2: Write the transition table
    for (int i = 1; i <= numStates; i++) {
        for (int j = 0; j < MAX_ALPHABET_SIZE; j++) {
            if (alphabetSet[j]) {
                char symbol = (char) j;
                if (i == 1) {
                    if (symbol == pattern[0]) {
                        fprintf(file, "[1,2] ");  // Stay in state 1 or move to state 2 on pattern start
                    } else {
                        fprintf(file, "[1] ");  // Stay in state 1 for other characters
                    }
                } else if (i < numStates) {
                    if (symbol == pattern[i - 1]) {
                        fprintf(file, "[%d] ", i + 1);  // Move to the next state
                    } else {
                        fprintf(file, "# ");  // No valid transition
                    }
                } else {
                    fprintf(file, "[%d] ", numStates);  // Stay in the final state for all inputs
                }
            }
        }
        fprintf(file, "#\n");  // Epsilon transitions always to '#'
    }

    fprintf(file, "\n");  // Separate sections

    // Section 3: Write the accepting states
    fprintf(file, "%d\n", numStates); // Accepting state is the last state

    fclose(file);
    printf("NDFSM specification written to %s\n", fileName);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <output_file> <pattern>\n", argv[0]);
        return 1;
    }

    NDFSMBuilder(argv[1], argv[2]);
    return 0;
}
