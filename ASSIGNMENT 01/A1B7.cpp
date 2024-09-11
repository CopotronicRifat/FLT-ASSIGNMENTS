#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>

class NDFSMtoDFSM {
public:
    static void convert(const std::string& inputFileName, const std::string& outputFileName) {
        std::ifstream reader(inputFileName);
        if (!reader.is_open()) {
            std::cerr << "Error: Could not open input file." << std::endl;
            exit(1);
        }

        std::string line;
        std::vector<std::string> lines;
        while (std::getline(reader, line)) {
            lines.push_back(line);
        }
        reader.close();

        if (lines.empty()) {
            std::cout << "Error: The input file is empty." << std::endl;
            exit(1);
        }

        std::vector<std::string> alphabet;
        std::vector<std::vector<std::string>> transitionTable;
        std::vector<int> acceptingStates;
        int section = 0;

        for (const auto& l : lines) {
            if (l.empty()) {
                section++;
                continue;
            }

            switch (section) {
                case 0:
                    std::istringstream alphaStream(l);
                    std::string symbol;
                    while (alphaStream >> symbol) {
                        alphabet.push_back(symbol);
                    }
                    break;
                case 1:
                    std::istringstream transitionStream(l);
                    std::vector<std::string> transitions;
                    std::string transition;
                    while (transitionStream >> transition) {
                        transitions.push_back(transition);
                    }
                    if (transitions.size() != alphabet.size()) {
                        std::cerr << "Error: The number of transitions does not match the number of alphabet symbols." << std::endl;
                        exit(1);
                    }
                    transitionTable.push_back(transitions);
                    break;
                case 2:
                    std::istringstream stateStream(l);
                    int state;
                    while (stateStream >> state) {
                        acceptingStates.push_back(state);
                    }
                    break;
            }
        }

        if (alphabet.empty() || transitionTable.empty()) {
            std::cerr << "Error: Necessary sections are empty." << std::endl;
            exit(1);
        }

        // DFSM Conversion logic here (not implemented in this snippet)
        // For demonstration, print NDFSM components
        std::cout << "Alphabet: ";
        for (const auto& a : alphabet) {
            std::cout << a << " ";
        }
        std::cout << "\nTransition table:" << std::endl;
        for (const auto& row : transitionTable) {
            for (const auto& cell : row) {
                std::cout << cell << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Accepting states: ";
        for (const auto& a : acceptingStates) {
            std::cout << a << " ";
        }
        std::cout << std::endl;

        // Assuming conversion to DFSM is done, here we would write the result to output file
        std::ofstream writer(outputFileName);
        if (!writer.is_open()) {
            std::cerr << "Error: Could not open output file." << std::endl;
            exit(1);
        }

        // Write example output (for demonstration purposes)
        writer << "DFSM output would be written here based on conversion logic." << std::endl;
        writer.close();

        std::cout << "DFSM specification written to " << outputFileName << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        exit(1);
    }

    NDFSMtoDFSM::convert(argv[1], argv[2]);
    return 0;
}
