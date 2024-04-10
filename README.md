# Spell Checker

This is a simple spell checker program written in C. It loads a dictionary file and checks the spelling of words in one or more text files or directories.

## Design

The program uses a binary search tree (implemented as a sorted array) to store the words from the dictionary file. This allows for efficient lookups during the spelling check.

The `load_dictionary` function reads the dictionary file line by line and stores each word in the `dict` array, which is dynamically resized as needed.

The `process_file` function reads a text file and extracts individual words, passing them to the `extract_words` function.

The `extract_words` function tokenizes the input buffer into individual words and stores them in a temporary array. It then calls the `check_spelling` function to check each word against the dictionary.

The `check_spelling` function uses binary search (`binary_search`) to check if a word exists in the dictionary. If a word is not found, its location (file, line, and column) is printed to the console.

The program supports recursively traversing directories to check all text files (`*.txt`) using the `traverse_directory` function.

## Test Scenarios and Cases

1. **Load Dictionary**
   - Test case: Load a valid dictionary file
   - Test case: Load an invalid dictionary file (file not found, permission issues)

2. **Binary Search**
   - Test case: Search for a word present in the dictionary (case-insensitive)
   - Test case: Search for a word not present in the dictionary

3. **Process Files**
   - Test case: Process a valid text file
   - Test case: Process an invalid text file (file not found, permission issues)

4. **Extract Words**
   - Test case: Extract words with valid characters (alphanumeric, apostrophe, hyphen)
   - Test case: Extract words with invalid characters
   - Test case: Extract words across line boundaries

5. **Check Spelling**
   - Test case: Check spelling of words present in the dictionary
   - Test case: Check spelling of words not present in the dictionary

6. **Traverse Directory**
   - Test case: Traverse a directory with valid text files
   - Test case: Traverse a directory with invalid files/directories
   - Test case: Traverse a directory with nested directories

7. **Command-Line Arguments**
   - Test case: Run the program with valid arguments (dictionary file, text files/directories)
   - Test case: Run the program with invalid arguments (missing arguments, invalid file paths)

To run the program, compile the source code and execute the resulting binary, passing the dictionary file and the text files or directories to check as command-line arguments: