#include <iostream>
#include <string>

// Assuming the necessary includes for NDFSMBuilder, NDFSMtoDFSM, and DFSM classes or functions are available
#include "A1B4.cpp"
#include "A1B8.cpp"
#include "A1A.cpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <pattern_to_recognize>\n";
        return 1;
    }

    std::string pattern = argv[1];

    // Step 1: Build NDFSM using the NDFSMBuilder
    std::string ndfsmFile = "NDFSM.txt";
    NDFSMBuilder::buildNDFSM(pattern, ndfsmFile);  // Implement this method in NDFSMBuilder.cpp

    // Step 2: Convert NDFSM to DFSM
    std::string dfsmFile = "DFSM.txt";
    NDFSMtoDFSM::convert(ndfsmFile, dfsmFile);  // Implement this method in NDFSMtoDFSM.cpp

    // Step 3: Test DFSM
    DFSM::testDFSM(dfsmFile);  // Implement this function in DFSM.cpp to test the DFSM

    return 0;
}
