// NDFSMtoDFSM Program
// Author: Mohammed Rakib
// Course: CS 5313
// Assignment Number: 1b

import java.io.*;
import java.util.*;

public class NDFSMtoDFSM {

    /**
     * Converts an NDFSM specification to a DFSM specification.
     *
     * @param inputFileName  The name of the file containing the NDFSM specification.
     * @param outputFileName The name of the file to write the DFSM specification.
     */
    public static void NDFSMtoDFSM(String inputFileName, String outputFileName) {
        try {
            // Read the NDFSM specification from the input file
            BufferedReader reader = new BufferedReader(new FileReader(inputFileName));
            List<String> lines = new ArrayList<>();
            String line;

            while ((line = reader.readLine()) != null) {
                lines.add(line.trim());
            }
            reader.close();

            // Check if the file is empty
            if (lines.isEmpty()) {
                System.out.println("Error: The input file is empty.");
                System.exit(1);
            }

            // Parse the NDFSM specification
            List<String> alphabet = new ArrayList<>();
            List<String[]> transitionTable = new ArrayList<>();
            List<Integer> acceptingStates = new ArrayList<>();

            int section = 0; // 0: alphabet, 1: transition table, 2: accepting states
            for (String l : lines) {
                if (l.isEmpty()) {
                    section++;
                    continue;
                }

                switch (section) {
                    case 0: // Alphabet section
                        alphabet.addAll(Arrays.asList(l.split("\\s+")));
                        break;
                    case 1: // Transition table section
                        String[] transitions = l.split("\\s+");
                        // Validate the number of transitions in the row
                        if (transitions.length != alphabet.size()) {
                            System.out.println("Error: The number of transitions (" + transitions.length +
                                               ") in a row does not match the number of alphabet symbols (" + alphabet.size() + ").");
                            System.exit(1);
                        }
                        transitionTable.add(transitions);
                        break;
                    case 2: // Accepting states section
                        for (String state : l.split("\\s+")) {
                            acceptingStates.add(Integer.parseInt(state));
                        }
                        break;
                }
            }

            // Check if the NDFSM sections are empty
            if (alphabet.isEmpty()) {
                System.out.println("Error: The alphabet section is empty.");
                System.exit(1);
            }
            if (transitionTable.isEmpty()) {
                System.out.println("Error: The transition table section is empty.");
                System.exit(1);
            }
            if (acceptingStates.isEmpty()) {
                System.out.println("Warning: The accepting states section is empty."); // Not a critical error
            }

            // Print the NDFSM specification
            System.out.println("Alphabet: " + String.join(" ", alphabet));
            System.out.println("Transition table:");
            for (String[] row : transitionTable) {
                System.out.println(String.join(" ", row));
            }
            System.out.println("Accepting states: " + Arrays.toString(acceptingStates.toArray()));

            // Remove epsilon from the alphabet (last element)
            String epsilon = "$";
            alphabet.remove(epsilon);

            // Compute epsilon closures for all states
            Map<Integer, Set<Integer>> epsilonClosures = computeEpsilonClosures(transitionTable, alphabet.size());

            // Initialize the DFSM structures
            Map<Set<Integer>, Integer> dStates = new HashMap<>();
            List<String[]> dfsmTransitions = new ArrayList<>();
            Set<Integer> dfsmAcceptingStates = new HashSet<>();
            Queue<Set<Integer>> queue = new LinkedList<>();

            // Start state is the epsilon closure of NDFSM state 1
            Set<Integer> startState = epsilonClosures.get(1);
            dStates.put(startState, 1);
            queue.add(startState);

            // Process each DFSM state using the subset construction method
            int nextStateId = 2;
            while (!queue.isEmpty()) {
                Set<Integer> currentSet = queue.poll();
                String[] currentTransitions = new String[alphabet.size()];

                for (int i = 0; i < alphabet.size(); i++) {
                    String symbol = alphabet.get(i);
                    Set<Integer> moveResult = move(currentSet, symbol, transitionTable, alphabet);
                    Set<Integer> epsilonClosureResult = new HashSet<>();

                    // Calculate epsilon closure of the move result
                    for (int state : moveResult) {
                        epsilonClosureResult.addAll(epsilonClosures.get(state));
                    }

                    // If the resulting epsilon closure is not empty and not yet in dStates, add it
                    if (!epsilonClosureResult.isEmpty() && !dStates.containsKey(epsilonClosureResult)) {
                        dStates.put(epsilonClosureResult, nextStateId++);
                        queue.add(epsilonClosureResult);
                    }

                    // Set the transition to the DFSM state id or new state for empty set
                    currentTransitions[i] = epsilonClosureResult.isEmpty() ? String.valueOf(dStates.size() + 1) : dStates.get(epsilonClosureResult).toString();
                }

                // Add to DFSM transitions
                dfsmTransitions.add(currentTransitions);

                // Check if the current DFSM state is accepting
                for (int state : currentSet) {
                    if (acceptingStates.contains(state)) {
                        dfsmAcceptingStates.add(dStates.get(currentSet));
                        break;
                    }
                }
            }

            // Add the new empty state for the empty set (`#`) transitions
            String[] emptyStateTransitions = new String[alphabet.size()];
            Arrays.fill(emptyStateTransitions, String.valueOf(dStates.size() + 1)); // Self-loop for all symbols
            dfsmTransitions.add(emptyStateTransitions);

            // Write the DFSM specification to the output file
            try (FileWriter writer = new FileWriter(outputFileName)) {
                // Write the alphabet
                for (String symbol : alphabet) {
                    writer.write(symbol + " ");
                }
                writer.write("\n\n"); // Separate sections

                // Write the transition table
                for (String[] transitions : dfsmTransitions) {
                    writer.write(String.join(" ", transitions) + "\n");
                }
                writer.write("\n"); // Separate sections

                // Write the accepting states
                for (int state : dfsmAcceptingStates) {
                    writer.write(state + " ");
                }
                writer.write("\n");

                System.out.println("DFSM specification written to " + outputFileName);
            }

        } catch (IOException e) {
            System.out.println("Error reading or writing files: " + e.getMessage());
        }
    }

    /**
     * Computes epsilon closures for all states in the NDFSM.
     *
     * @param transitionTable The NDFSM transition table.
     * @param numSymbols      The number of symbols in the alphabet (excluding epsilon).
     * @return A map of epsilon closures for each state.
     */
    private static Map<Integer, Set<Integer>> computeEpsilonClosures(List<String[]> transitionTable, int numSymbols) {
        Map<Integer, Set<Integer>> epsilonClosures = new HashMap<>();

        for (int i = 0; i < transitionTable.size(); i++) {
            int state = i + 1; // States are 1-based
            Set<Integer> closure = new HashSet<>();
            computeEpsilonClosure(state, transitionTable, numSymbols, closure);
            epsilonClosures.put(state, closure);
        }

        return epsilonClosures;
    }

    /**
     * Recursively computes the epsilon closure for a given state.
     *
     * @param state           The state to compute the epsilon closure for.
     * @param transitionTable The NDFSM transition table.
     * @param numSymbols      The number of symbols in the alphabet (excluding epsilon).
     * @param closure         The current epsilon closure being computed.
     */
    private static void computeEpsilonClosure(int state, List<String[]> transitionTable, int numSymbols, Set<Integer> closure) {
        if (!closure.contains(state)) {
            closure.add(state);
            String[] transitions = transitionTable.get(state - 1);
            String epsilonTransition = transitions[numSymbols]; // Last column is epsilon

            if (!epsilonTransition.equals("#")) { // If there is an epsilon transition
                String[] epsilonStates = epsilonTransition.replace("[", "").replace("]", "").split(",");
                for (String epsilonState : epsilonStates) {
                    computeEpsilonClosure(Integer.parseInt(epsilonState), transitionTable, numSymbols, closure);
                }
            }
        }
    }

    /**
     * Computes the set of states reachable from the current set on the given symbol.
     *
     * @param currentSet      The current set of states.
     * @param symbol          The input symbol.
     * @param transitionTable The NDFSM transition table.
     * @param alphabet        The list of alphabet symbols.
     * @return A set of reachable states.
     */
    private static Set<Integer> move(Set<Integer> currentSet, String symbol, List<String[]> transitionTable, List<String> alphabet) {
        Set<Integer> result = new HashSet<>();
        int symbolIndex = -1; // Index of the symbol in the alphabet

        // Find the index of the symbol in the alphabet list
        for (int i = 0; i < alphabet.size(); i++) {
            if (alphabet.get(i).equals(symbol)) {
                symbolIndex = i;
                break;
            }
        }

        // If symbolIndex is -1, that means the symbol was not found in the alphabet
        if (symbolIndex == -1) {
            System.out.println("Error: Symbol '" + symbol + "' not found in the alphabet.");
            return result;  // Return an empty result set
        }

        // For each state in the current set, find the transitions on the symbol
        for (int state : currentSet) {
            String[] transitions = transitionTable.get(state - 1);
            String transitionStates = transitions[symbolIndex];
            if (!transitionStates.equals("#")) { // If there is a valid transition
                String[] nextStates = transitionStates.replace("[", "").replace("]", "").split(",");
                for (String nextState : nextStates) {
                    result.add(Integer.parseInt(nextState));
                }
            }
        }

        return result;
    }

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Usage: java NDFSMtoDFSM <input_file> <output_file>");
            System.exit(1);
        }

        String inputFileName = args[0];
        String outputFileName = args[1];

        // Convert NDFSM to DFSM
        NDFSMtoDFSM(inputFileName, outputFileName);
    }
}
