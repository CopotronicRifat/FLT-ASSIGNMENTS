#include <iostream>
#include <string>

// Assuming the necessary includes for NDFSMBuilder, NDFSMtoDFSM, and DFSM classes or functions are available
class NDFSM {
public:
    std::vector<std::vector<std::set<int>>> transitions; // Transitions for each state and symbol index
    std::vector<char> alphabet; // Alphabet including epsilon at the end
    std::set<int> acceptingStates; // Accepting states of the NDFSM

    void readFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open input file: " + filename);
        }

        std::string line;
        int section = 0;
        std::vector<std::string> lines;

        // Read lines into sections based on empty lines
        while (getline(file, line)) {
            if (line.empty()) {
                section++;
                continue;
            }
            if (section == 0) { // Alphabet section
                parseAlphabet(line);
            } else if (section == 1) { // Transitions section
                parseTransitions(line);
            } else if (section == 2) { // Accepting states section
                parseAcceptingStates(line);
            }
        }

        file.close();
    }

    void parseAlphabet(const std::string& line) {
        std::istringstream stream(line);
        char symbol;
        while (stream >> symbol) {
            alphabet.push_back(symbol);
        }
        transitions.resize(alphabet.size());
        for (auto& stateTransitions : transitions) {
            stateTransitions.resize(alphabet.size());
        }
    }

    void parseTransitions(const std::string& line) {
        std::istringstream stream(line);
        std::vector<std::set<int>> stateTransitions(alphabet.size());
        std::string transition;
        int symbolIndex = 0;

        while (getline(stream, transition, ' ')) {
            if (transition != "#") {
                std::istringstream transStream(transition.substr(1, transition.size() - 2)); // Strip brackets
                int state;
                while (transStream >> state) {
                    if (transStream.peek() == ',') transStream.ignore();
                    stateTransitions[symbolIndex].insert(state);
                }
            }
            symbolIndex++;
        }
        transitions.push_back(stateTransitions);
    }

    void parseAcceptingStates(const std::string& line) {
        std::istringstream stream(line);
        int state;
        while (stream >> state) {
            acceptingStates.insert(state);
        }
    }

    void print() const {
        std::cout << "NDFSM Alphabet: ";
        for (char c : alphabet) {
            std::cout << c << " ";
        }
        std::cout << std::endl;

        std::cout << "Transition Table:" << std::endl;
        for (size_t state = 0; state < transitions.size(); ++state) {
            for (size_t symbol = 0; symbol < alphabet.size(); ++symbol) {
                std::cout << "State " << state << " via " << alphabet[symbol] << " -> {";
                for (int nextState : transitions[state][symbol]) {
                    std::cout << nextState << " ";
                }
                std::cout << "} ";
            }
            std::cout << std::endl;
        }

        std::cout << "Accepting States: ";
        for (int state : acceptingStates) {
            std::cout << state << " ";
        }
        std::cout << std::endl;
    }
};
class NDFSMtoDFSM {
public:
    static void convert(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream inFile(inputFileName);
        if (!inFile) {
            std::cerr << "Error: Cannot open NDFSM file: " << inputFileName << std::endl;
            exit(1);
        }

        std::string line;
        std::vector<std::string> lines;
        while (std::getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();

        if (lines.empty()) {
            std::cerr << "Error: The input file is empty." << std::endl;
            exit(1);
        }

        std::vector<std::string> alphabet;
        std::vector<std::vector<std::string>> transitionTable;
        std::set<int> acceptingStates;
        int section = 0;

        for (const auto& l : lines) {
            if (l.empty()) {
                section++;
                continue;
            }

            std::istringstream iss(l);
            switch (section) {
                case 0: {  // Alphabet section
                    std::string symbol;
                    while (iss >> symbol) {
                        alphabet.push_back(symbol);
                    }
                    break;
                }
                case 1: {  // Transition table section
                    std::vector<std::string> transitions;
                    std::string transition;
                    while (iss >> transition) {
                        transitions.push_back(transition);
                    }
                    if (transitions.size() != alphabet.size()) {
                        std::cerr << "Error: The number of transitions does not match the number of alphabet symbols." << std::endl;
                        exit(1);
                    }
                    transitionTable.push_back(transitions);
                    break;
                }
                case 2: {  // Accepting states section
                    int state;
                    while (iss >> state) {
                        acceptingStates.insert(state);
                    }
                    break;
                }
            }
        }

        if (alphabet.empty() || transitionTable.empty() || acceptingStates.empty()) {
            std::cerr << "Error: Necessary sections are empty." << std::endl;
            exit(1);
        }

        // Removing epsilon transition and initiating conversion process
        alphabet.pop_back();  // Assume epsilon is always the last in the alphabet
        auto epsilonClosures = computeEpsilonClosures(transitionTable, alphabet.size());
        auto dfsm = convertToDFSM(epsilonClosures, transitionTable, alphabet, acceptingStates);

        writeDFSM(dfsm, outputFileName, alphabet);
    }

    static std::map<std::set<int>, std::vector<std::set<int>>> computeEpsilonClosures(const std::vector<std::vector<std::string>>& transitionTable, int numSymbols) {
        std::map<std::set<int>, std::vector<std::set<int>>> epsilonClosures;

        for (int i = 0; i < transitionTable.size(); i++) {
            std::set<int> closure;
            closure.insert(i + 1);  // states are 1-indexed
            computeClosure(closure, transitionTable, numSymbols);
            epsilonClosures[i + 1] = closure;
        }

        return epsilonClosures;
    }

    static void computeClosure(std::set<int>& closure, const std::vector<std::vector<std::string>>& transitionTable, int numSymbols) {
        std::queue<int> toProcess;
        for (int s : closure) toProcess.push(s);

        while (!toProcess.empty()) {
            int state = toProcess.front();
            toProcess.pop();

            // Assuming epsilon is always the last symbol in each row
            std::string epsTransitions = transitionTable[state - 1][numSymbols];
            std::istringstream epsStream(epsTransitions);
            int nextState;
            while (epsStream >> nextState) {
                if (closure.insert(nextState).second) {
                    toProcess.push(nextState);
                }
            }
        }
    }

    static void writeDFSM(const std::vector<std::vector<int>>& dfsm, const std::string& fileName, const std::vector<std::string>& alphabet) {
        std::ofstream outFile(fileName);
        if (!outFile) {
            std::cerr << "Error: Cannot write to output file: " << fileName << std::endl;
            exit(1);
        }

        for (const auto& sym : alphabet) {
            outFile << sym << " ";
        }
        outFile << "\n\n";

        for (const auto& row : dfsm) {
            for (auto state : row) {
                outFile << state << " ";
            }
            outFile << "\n";
        }
        outFile.close();
    }
};
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
        if (!isalpha(alphabet[i])) {
            fprintf(stderr, "Error: Alphabet must contain only alphabetic characters.\n");
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
                token = strtok(line, " \n");
                while (token) {
                    alphabet[numAlphabet++] = token[0];
                    token = strtok(NULL, " \n");
                }
                break;
            case 2: // Transition table
                if (stateIndex >= MAX_STATES) {
                    fprintf(stderr, "Error: State index exceeds maximum allowed states.\n");
                    fclose(file);
                    return -1;
                }
                token = strtok(line, " \n");
                int colIndex = 0;
                while (token) {
                    int nextState = atoi(token) - 1;
                    if (nextState < 0 || nextState >= MAX_STATES) {
                        fprintf(stderr, "Error: Invalid state number %d\n", nextState + 1);
                        fclose(file);
                        return -1;
                    }
                    transitionTable[stateIndex][colIndex++] = nextState;
                    token = strtok(NULL, " \n");
                }
                stateIndex++;
                break;
            case 3: // Accepting states
                numAcceptingStates = 0;
                token = strtok(line, " \n");
                while (token) {
                    int acceptingState = atoi(token) - 1;
                    if (acceptingState < 0 || acceptingState >= MAX_STATES) {
                        fprintf(stderr, "Error: Invalid accepting state number %d\n", acceptingState + 1);
                        fclose(file);
                        return -1;
                    }
                    acceptingStates[numAcceptingStates++] = acceptingState;
                    token = strtok(NULL, " \n");
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

    int currentState = 0; // Start state is 0 (state 1 in file, 0-indexed in array)
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
