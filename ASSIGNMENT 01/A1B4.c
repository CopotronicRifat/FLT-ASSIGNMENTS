#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <queue>

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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return EXIT_FAILURE;
    }

    try {
        NDFSM ndfsm;
        ndfsm.readFromFile(argv[1]);
        ndfsm.print(); // Print the NDFSM to verify correct reading

        // Here you would initiate the DFSM conversion
        DFSM dfsm;
        dfsm.convertFrom(ndfsm);
        dfsm.writeTo(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
