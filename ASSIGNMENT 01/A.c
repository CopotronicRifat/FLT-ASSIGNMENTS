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

// Function to validate the alphabet: Check for uniqueness and alphabetic characters
int validateAlphabet() {
    for (int i = 0; i < numAlphabet; i++) {
        if (!isalpha(alphabet[i])) {
            fprintf(stderr, "Error: Alphabet must contain only alphabetic characters.\n");
            return 0;
        }
        for (int j = i + 1; j < numAlphabet; j++) {
            if (alphabet[i] == alphabet[j]) {
                fprintf(stderr, "Error: Duplicate alphabet character %c.\n", alphabet[i]);
                return 0;
            }
        }
    }
    return 1;
}

// Function to read and load the DFSM from a file
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
    int tokenIndex = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0) {
            section++;
            continue;
        }
        switch (section) {
            case 1: // Alphabet section
                token = strtok(line, " \t\n");
                while (token != NULL && numAlphabet < MAX_ALPHABET) {
                    alphabet[numAlphabet++] = token[0];
                    token = strtok(NULL, " \t\n");
                }
                break;

            case 2: // Transition Table
                token = strtok(line, " \t\n");
                tokenIndex = 0;
                while (token != NULL) {
                    if (tokenIndex >= numAlphabet) {
                        fprintf(stderr, "Error: More transitions than alphabet size\n");
                        fclose(file);
                        return -1;
                    }
                    int nextState = atoi(token) - 1;
                    if (nextState < 0 || nextState >= MAX_STATES) {
                        fprintf(stderr, "Error: Invalid state number %d\n", nextState + 1);
                        fclose(file);
                        return -1;
                    }
                    transitionTable[stateIndex][tokenIndex++] = nextState;
                    token = strtok(NULL, " \t\n");
                }
                stateIndex++;
                break;

            case 3: // Accepting States
                token = strtok(line, " \t\n");
                while (token != NULL) {
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
    if (!validateAlphabet()) {
        return -1;
    }
    numStates = stateIndex;
    return 0;
}

// Function to simulate the DFSM with the input string
int simulateDFSM(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening input string file");
        return -1;
    }

    int currentState = 0; // Start state is 0 (0-based index)
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n' || ch == ' ') {
            continue;
        }
        int alphabetIndex = strchr(alphabet, ch) - alphabet;
        if (alphabetIndex < 0 || alphabetIndex >= numAlphabet) {
            fprintf(stderr, "Error: Character '%c' is not in the alphabet\n", ch);
            fclose(file);
            return -1;
        }
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
