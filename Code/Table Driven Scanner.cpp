#include <iostream>
#include "lexical.h"
#include "Synthetic.h"
#include<Windows.h>

using namespace std;

// |-------------------------------------------------------------------------------------------------------------|
// |                                              Main Program                                                   |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
This is the main function that orchestrates the process of lexical and syntax analysis. It first performs lexical analysis on the content of a file, then checks and processes a context-free grammar for syntax analysis. It also computes the necessary sets for parsing, builds the parse table, and parses input based on the tokenized source code.

Logic:
1. Instantiate the `Lexical` object and call the `PerformLexical` function to perform lexical analysis on the file `test_code.txt`. The results are saved in `tokenLex.txt`, `symbolTable.txt`, and `errorLex.txt`.
2. If an error occurs during lexical analysis, output an error message and terminate the program.
3. Instantiate the `Synthetic` object (for syntax analysis).
4. Load the grammar rules from the file `cfg_rules.txt` using the `loadGrammarFromFile` method.
5. Analyze the grammar to check for left recursion and left factoring issues using the `analyzeGrammar` method. If issues are found, output an error message and terminate the program.
6. Compute the FIRST and FOLLOW sets using the `computeFirstAndFollow` method.
7. Build the parse table using the `buildParseTable` method.
8. Print the parse table to the console using the `printParseTable` method.
9. Parse the tokenized input from `tokenLex.txt` starting from the `<program>` non-terminal using the `parseFromFile` method.
10. Print the parse tree for each processing action.
11. Return 0 indicating successful execution of the program.
</summary> */
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Lexical lexical;
    int res = lexical.PerformLexical("test_code.txt", "tokenLex.txt", "symbolTable.txt", "error.txt");
    if (res == 1)
    {
        cerr << "Error: Problem in Lexical Analysis encountered\n";
    }

    Synthetic syntheticAnalzer;

    std::string fileName = "cfg_rules.txt";
    syntheticAnalzer.loadGrammarFromFile(fileName);
    if (!syntheticAnalzer.analyzeGrammar()) 
    {
        std::cerr << "Grammar contains left recursion or requires left factoring.\n";
        return 1;
    }
    syntheticAnalzer.printGrammarToFile();
    // Compute FIRST and FOLLOW sets and generate the parse table
    syntheticAnalzer.computeFirstAndFollow();
    syntheticAnalzer.buildParseTable();
    syntheticAnalzer.printParseTable();
    syntheticAnalzer.writeParseTableToFile();
    syntheticAnalzer.parseFromFile("tokenLex.txt", "<program>");
    return 0;
}