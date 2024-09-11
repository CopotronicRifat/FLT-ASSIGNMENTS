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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input NDFSM file> <output DFSM file>" << std::endl;
        return 1;
    }

    NDFSMtoDFSM::convert(argv[1], argv[2]);
    return 0;
}
