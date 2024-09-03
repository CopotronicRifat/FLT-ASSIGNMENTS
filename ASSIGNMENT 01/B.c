#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STATES 100
#define MAX_ALPHABET 26

int transitionTable[MAX_STATES][MAX_ALPHABET];
int acceptingStates[MAX_STATES];
int numStates = 0;
int numAcceptingStates = 0;
int numAlphabet = 0;
char alphabet[MAX_ALPHABET];

// Function to validate the alphabet
int validateAlphabet() {
    for (int i = 0; i < numAlphabet; i++) {
        if (!isalnum(alphabet[i]) && !ispunct(alphabet[i])) {
            fprintf(stderr, "Error: Alphabet must contain only valid characters.\n");
            return 0;
        }
    }
    return 1;
}

// Load the DFSM from a file
int loadDFSM(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening DFSM specification file");
        return -1;
    }
    char line[256];
    char *token;
    int stateIndex = 0;
    int section = 1;

    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0) {
            section++;
            continue;
        }

        switch (section) {
            case 1: // Alphabet section
                numAlphabet = 0;
                token = strtok(line, " \t\n");
                while (token) {
                    alphabet[numAlphabet++] = token[0];
                    token = strtok(NULL, " \t\n");
                }
                break;
            case 2: // Transition table
                if (stateIndex >= MAX_STATES) {
                    fprintf(stderr, "Error: State index exceeds maximum allowed states.\n");
                    fclose(file);
                    return -1;
                }
                int colIndex = 0;
                token = strtok(line, " \t\n");
                while (token) {
                    int nextState = atoi(token) - 1; // Zero-indexed in the array
                    if (nextState < 0 || nextState >= MAX_STATES) {
                        fprintf(stderr, "Error: Invalid state number %d\n", nextState + 1);
                        fclose(file);
                        return -1;
                    }
                    transitionTable[stateIndex][colIndex++] = nextState;
                    token = strtok(NULL, " \t\n");
                }
                stateIndex++;
                break;
            case 3: // Accepting states
                token = strtok(line, " \t\n");
                if (!token) {
                    // If there is no token, it means no accepting states are defined
                    numAcceptingStates = 0;
                    break;
                }
                while (token) {
                    int acceptingState = atoi(token) - 1;
                    if (acceptingState < 0 || acceptingState >= MAX_STATES) {
                        fprintf(stderr, "Error: Invalid accepting state number %d\n", acceptingState + 1);
                        fclose(file);
                        return -1;
                    }
                    acceptingStates[numAcceptingStates++] = acceptingState;
                    token = strtok(NULL, " \t\n");
                }
                break;
        }
    }
    fclose(file);
    return validateAlphabet() ? 0 : -1;
}

// Simulate the DFSM with an input string
int simulateDFSM(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening input string file");
        return -1;
    }

    int currentState = 0; // Start state is 0 (state 1 in the file and zero-indexed here)
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n' || ch == ' ') {
            continue;
        }
        char *ptr = strchr(alphabet, ch);
        if (ptr == NULL) {
            fprintf(stderr, "Error: Character '%c' is not in the alphabet\n", ch);
            fclose(file);
            return -1;
        }
        int alphabetIndex = ptr - alphabet;
        currentState = transitionTable[currentState][alphabetIndex];
    }
    fclose(file);

    for (int i = 0; i < numAcceptingStates; i++) {
        if (currentState == acceptingStates[i]) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <DFSM file> <input string file>\n", argv[0]);
        return 1;
    }

    if (loadDFSM(argv[1]) != 0) {
        return 1;
    }

    int result = simulateDFSM(argv[2]);

    if (result == -1) {
        return 1;
    } else if (result == 1) {
        printf("yes\n");
    } else {
        printf("no\n");
    }
    return 0;
}
