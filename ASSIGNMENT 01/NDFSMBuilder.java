// NDFSMBuilder Program
// Author: Mohammed Rakib
// Course: CS 5313
// Assignment Number: 1b

import java.io.FileWriter;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

public class NDFSMBuilder {

    /**
     * Constructs a non-deterministic finite state machine for substring pattern recognition.
     *
     * @param outputFilePath Path to save the NDFSM file.
     * @param targetPattern Pattern to identify within substrings.
     */
    public static void constructNDFSM(String outputFilePath, String targetPattern) {
        if (targetPattern.isEmpty()) {
            System.out.println("Error: Provided pattern is empty.");
            return;
        }

        // Extract unique characters from the pattern to create the alphabet
        Set<Character> uniqueCharacters = new HashSet<>();
        for (char character : targetPattern.toCharArray()) {
            uniqueCharacters.add(character);
        }
        String[] alphabet = uniqueCharacters.stream().map(String::valueOf).toArray(String[]::new);

        // Append epsilon ('$') to the alphabet as the last element
        String[] completeAlphabet = new String[alphabet.length + 1];
        System.arraycopy(alphabet, 0, completeAlphabet, 0, alphabet.length);
        completeAlphabet[alphabet.length] = "$";

        // Calculate the number of states required based on pattern length
        int totalStates = targetPattern.length() + 1;

        // Building the NDFSM specification string
        StringBuilder fsmDefinition = new StringBuilder();

        // Section 1: Write the alphabet
        for (String symbol : completeAlphabet) {
            fsmDefinition.append(symbol).append(" ");
        }
        fsmDefinition.append("\n\n");  // Section separator

        // Section 2: Create the state transitions
        for (int state = 1; state <= totalStates; state++) {
            StringBuilder stateTransitions = new StringBuilder();

            for (int index = 0; index < completeAlphabet.length - 1; index++) {
                String symbol = completeAlphabet[index];

                if (state == 1) {
                    // Initial state behavior
                    if (symbol.equals(String.valueOf(targetPattern.charAt(0)))) {
                        stateTransitions.append("[1,2] ");
                    } else {
                        stateTransitions.append("[1] ");
                    }
                } else if (state < totalStates) {
                    // Intermediate states behavior
                    char currentChar = targetPattern.charAt(state - 1);
                    if (symbol.equals(String.valueOf(currentChar))) {
                        stateTransitions.append("[").append(state + 1).append("] ");
                    } else {
                        stateTransitions.append("# ");
                    }
                } else {
                    // Final state remains the same
                    stateTransitions.append("[").append(totalStates).append("] ");
                }
            }

            // Epsilon transition handling
            stateTransitions.append("#");  // No epsilon transitions

            fsmDefinition.append(stateTransitions.toString().trim()).append("\n");
        }

        fsmDefinition.append("\n");  // Section separator

        // Section 3: Define accepting states
        fsmDefinition.append(totalStates);  // Only the final state is accepting

        // Write the FSM to the specified file
        try (FileWriter writer = new FileWriter(outputFilePath)) {
            writer.write(fsmDefinition.toString());
            System.out.println("NDFSM file created at " + outputFilePath);
        } catch (IOException e) {
            System.out.println("Error when writing to file: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Usage: java NDFSMBuilder <output_file_path> <pattern_to_recognize>");
            System.exit(1);
        }

        String fileToWrite = args[0];
        String recognitionPattern = args[1];

        // Call to build the NDFSM
        constructNDFSM(fileToWrite, recognitionPattern);
    }
}
