import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

public class DFSM {

    /**
     * Loads a DFSM configuration from a specified file.
     *
     * @param filePath The path to the file holding the DFSM configuration.
     * @return An array containing the alphabet, transition matrix, and list of accepting states.
     */
    private static Object[] loadDFSM(String filePath) {
        String[] symbols = null;
        int[][] stateTransitions = null;
        int[] finalStates = null;

        try (FileReader fileReader = new FileReader(filePath)) {
            StringBuilder fileContent = new StringBuilder();
            int character;
            while ((character = fileReader.read()) != -1) {
                fileContent.append((char) character);
            }

            if (fileContent.toString().trim().isEmpty()) {
                System.out.println("Error: DFSM configuration file is empty.");
                System.exit(1);
            }

            String[] lines = fileContent.toString().split("\n");
            int parsingSection = 0;
            int row = 0;

            for (int index = 0; index < lines.length; index++) {
                String line = lines[index].trim();
                if (line.isEmpty()) {
                    parsingSection++;
                    continue;
                }

                switch (parsingSection) {
                    case 0:
                        symbols = line.split("\\s+");
                        stateTransitions = new int[lines.length - index - 2][symbols.length];
                        break;
                    case 1:
                        String[] transitionValues = line.split("\\s+");
                        for (int col = 0; col < transitionValues.length; col++) {
                            stateTransitions[row][col] = Integer.parseInt(transitionValues[col]);
                        }
                        row++;
                        break;
                    case 2:
                        String[] stateNumbers = line.split("\\s+");
                        finalStates = new int[stateNumbers.length];
                        for (int col = 0; col < stateNumbers.length; col++) {
                            finalStates[col] = Integer.parseInt(stateNumbers[col]);
                        }
                        break;
                }
            }

            if (row > 0) {
                int[][] reducedStateTransitions = new int[row][symbols.length];
                System.arraycopy(stateTransitions, 0, reducedStateTransitions, 0, row);
                stateTransitions = reducedStateTransitions;
            }

        } catch (IOException ex) {
            System.out.println("Error reading DFSM file: " + ex.getMessage());
            System.exit(1);
        }

        return new Object[]{symbols, stateTransitions, finalStates};
    }

    /**
     * Retrieves the input sequence from the specified file.
     *
     * @param filePath The path to the file holding the input sequence.
     * @return The sequence as a string.
     */
    private static String fetchInputSequence(String filePath) {
        StringBuilder sequence = new StringBuilder();

        try (FileReader reader = new FileReader(filePath)) {
            int character;
            while ((character = reader.read()) != -1) {
                sequence.append((char) character);
            }

            if (sequence.toString().trim().isEmpty()) {
                System.out.println("Error: Input sequence file is empty.");
                System.exit(1);
            }
        } catch (IOException ex) {
            System.out.println("Error reading input sequence file: " + ex.getMessage());
            System.exit(1);
        }

        return sequence.toString().trim();
    }

    /**
     * Ensures the DFSM configuration is correct.
     *
     * @param symbols Set of symbols from the alphabet.
     * @param matrix The transition matrix.
     * @return True if the configuration is valid, false otherwise.
     */
    private static boolean confirmDFSMValidity(String[] symbols, int[][] matrix) {
        for (int[] transitions : matrix) {
            if (transitions.length != symbols.length) {
                System.out.println("Error: Incorrect number of transitions in a state.");
                return false;
            }

            boolean[] seen = new boolean[matrix.length + 1];
            for (int transition : transitions) {
                if (seen[transition]) {
                    System.out.println("Error: Duplicate transitions detected for a symbol in a state.");
                    return false;
                }
                seen[transition] = true;
            }
        }
        return true;
    }

    /**
     * Executes the DFSM based on the given configuration and input sequence.
     *
     * @param dfsFilePath Path to the DFSM configuration file.
     * @param inputFilePath Path to the input sequence file.
     */
    private static void runDFSM(String dfsFilePath, String inputFilePath) {
        Object[] dfsSpec = loadDFSM(dfsFilePath);
        String[] alphabet = (String[]) dfsSpec[0];
        int[][] transitions = (int[][]) dfsSpec[1];
        int[] acceptStates = (int[]) dfsSpec[2];

        System.out.println("Alphabet: " + Arrays.toString(alphabet));
        System.out.println("Transition Matrix: " + Arrays.deepToString(transitions));
        System.out.println("Accepting States: " + Arrays.toString(acceptStates));

        if (!confirmDFSMValidity(alphabet, transitions)) {
            System.out.println("Error: DFSM configuration is not valid.");
            return;
        }

        String sequence = fetchInputSequence(inputFilePath);

        int[] symbolIndexMap = new int[256];
        for (int i = 0; i < alphabet.length; i++) {
            symbolIndexMap[alphabet[i].charAt(0)] = i;
        }

        int currentState = 1;

        for (int i = 0; i < sequence.length(); i++) {
            char currentChar = sequence.charAt(i);
            if (symbolIndexMap[currentChar] == 0 && currentChar != alphabet[0].charAt(0)) {
                System.out.println("Error: Character '" + currentChar + "' is not in the alphabet.");
                return;
            }
            currentState = transitions[currentState - 1][symbolIndexMap[currentChar]];
        }

        boolean isAccepting = Arrays.binarySearch(acceptStates, currentState) >= 0;

        if (isAccepting) {
            System.out.println("yes");
        } else {
            System.out.println("no");
        }
    }

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Usage: java DFSM <DFSM_CONFIGURATION_FILE> <INPUT_SEQUENCE_FILE>");
            System.exit(1);
        }

        runDFSM(args[0], args[1]);
    }
}
