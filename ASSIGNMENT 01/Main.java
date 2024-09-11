import java.io.*;

public class Main {

    public static void main(String[] args) {
        // Ensure that exactly two arguments are provided
        if (args.length != 2) {
            System.out.println("Usage: java Main <pattern> <input_string_file>");
            System.exit(1);
        }

        String pattern = args[0]; // The substring pattern to detect
        String inputStringFile = args[1]; // File containing the string to test

        // Step 1: Create an NDFSM based on the provided pattern
        String ndfsmFilename = "NDFSM.txt"; // File to hold the NDFSM specification
        NDFSMBuilder.createNDFSM(ndfsmFilename, pattern);
        System.out.println("NDFSM specification is saved in " + ndfsmFilename);

        // Step 2: Convert the NDFSM into a DFSM
        String dfsmFilename = "DFSM.txt"; // File to hold the DFSM specification
        NDFSMtoDFSM.convertNDFSMtoDFSM(ndfsmFilename, dfsmFilename);
        System.out.println("DFSM specification is saved in " + dfsmFilename);

        // Step 3: Evaluate if the generated DFSM recognizes the input string as matching the pattern
        DFSMSimulator.simulateDFSM(dfsmFilename, inputStringFile); // Simulate the DFSM with the input file
    }
}
