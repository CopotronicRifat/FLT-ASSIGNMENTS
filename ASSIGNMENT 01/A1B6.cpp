#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

class NDFSMBuilder {
public:
    static void buildNDFSM(const std::string& fileName, const std::string& pattern) {
        if (pattern.empty()) {
            std::cout << "Error: Pattern is empty." << std::endl;
            return;
        }

        // Determine the alphabet from the unique characters in the pattern
        std::set<char> alphabetSet(pattern.begin(), pattern.end());
        std::vector<char> alphabet(alphabetSet.begin(), alphabetSet.end());

        // Add epsilon ('$') to the alphabet and ensure it is the last symbol
        alphabet.push_back('$');

        // Number of states needed is pattern length + 1
        int numStates = pattern.length() + 1;

        // Open file for writing
        std::ofstream writer(fileName);
        if (!writer) {
            std::cout << "Error writing NDFSM specification: could not open file " << fileName << std::endl;
            return;
        }

        // Section 1: Write the alphabet
        for (char symbol : alphabet) {
            writer << symbol << " ";
        }
        writer << "\n\n";  // Empty line to separate sections

        // Section 2: Write the transition table
        for (int i = 1; i <= numStates; ++i) {
            for (int j = 0; j < alphabet.size() - 1; ++j) {  // Exclude epsilon for now
                char symbol = alphabet[j];

                if (i == 1) {
                    // State 1: Stay in State 1 for any input or move to State 2 for the first character of the pattern
                    if (symbol == pattern[0]) {
                        writer << "[1,2] ";  // Stay in state 1 or move to state 2 on pattern start
                    } else {
                        writer << "[1] ";  // Stay in state 1 for non-pattern start characters
                    }
                } else if (i < numStates) {
                    // Intermediate states: Transition to the next state on the correct character
                    char currentChar = pattern[i - 1];
                    if (symbol == currentChar) {
                        writer << "[" << (i + 1) << "] ";  // Move to the next state
                    } else {
                        writer << "# ";  // No valid transition
                    }
                } else {
                    // Final state: Stay in the final state for all inputs
                    writer << "[" << numStates << "] ";
                }
            }

            // Epsilon transition ('$') handling - always the last column
            writer << "#\n";  // No epsilon transition for any states
        }

        writer << "\n";  // Empty line to separate sections

        // Section 3: Write the accepting states
        writer << numStates << std::endl; // Accepting state is the last state

        writer.close();
        std::cout << "NDFSM specification written to " << fileName << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <output_file> <pattern>" << std::endl;
        return 1;
    }

    std::string outputFileName = argv[1];
    std::string pattern = argv[2];

    // Generate NDFSM specification
    NDFSMBuilder::buildNDFSM(outputFileName, pattern);

    return 0;
}
