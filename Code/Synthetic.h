#ifndef SYNTHETIC_H
#define SYNTHETIC_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <stack>
#include <vector>
#include <iomanip>
#include <algorithm>

/*
CFG Rules for my Language:
<program> -> <statements> | <statement> | <declaration> | <assignment> | <loop> | <conditional> | <return_statement> | <function_call> | <arguments> | <expression> | <term> | <identifier> | <letter> |<number> | <digit> | <operator> | <punctuation> | <type>
<statements> -> <statement> <statements>
<statement> -> <declaration> | <type> | <assignment> | <expression> : | <loop> | <conditional> | <return_statement> | <function_call>
<declaration> -> <type> <identifier> :
<assignment> -> <identifier> =:= <expression> :
<loop> -> loop [ <expression> ] { <statements> }
<conditional> -> if [ <expression> ] { <statements> } [else { <statements> }]
<return_statement> -> return <expression> :
<function_call> -> <identifier> [ <arguments> ]:
<arguments> -> <expression> <rest_arguments>
<rest_arguments> || <arguments> | ε
<expression> -> <term> <rest_expression>
<rest_expression> -> ε | <operator> <expression>
<term> -> <identifier> | <number> | [ <expression> ]
<identifier> -> <start_identifier> <identifier_tail>
<start_identifier> -> <letter> | _
<identifier_tail> -> _ <rest_identifier_tail> | <letter> <rest_identifier_tail> | <digit> <rest_identifier_tail> | ε
<rest_identifier_tail> -> <identifier_tail> | ε
<letter> -> [a-zA-Z]
<number> -> [+-]? <digit>+ [ . <digit>+] [ e [+-]? <digit>+]
<digit> -> 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0
<operator> -> != | < <greaterOp> | = <equalOpRest> | * | + <PlusOp> | / | - <MinusOp> | >> | && | % | : <colonOp> | ||
<equalOpRest> -> := | = | + | > | < | ε
<MinusOp> -> - | ε
<PlusOp> -> + | ε
<colonOp> -> : | ε
<greaterOp> -> > | < | ε
<punctuation> -> [ | { | < | > | } | ]
<type> -> int | float | char | bool | double | void | long | short | <keyword>
<keyword> -> loop | agar | magar | asm | else | new | this | auto | enum | operator | throw | bool | explicit | private | true | break | export | protected | try | case | extern | public | typedef | catch | false | register | typeid | char | float | typename | class | for | return | union | const | friend | short | unsigned | goto | signed | using | continue | if | sizeof | virtual | default | inline | static | void | delete | int | volatile | do | long | struct | double | mutable | switch | while | namespace
*/


/* <summary>
The `Synthetic` class is responsible for performing syntactic analysis on a context-free grammar (CFG). This class facilitates the parsing process, including generating and analyzing FIRST and FOLLOW sets, eliminating left recursion and left factoring, and building a parse table. The class can also parse input strings and files based on the constructed parse table, while also outputting relevant results (such as the grammar, FIRST/ FOLLOW sets, and parse table) to various files.
The functinos in it are related to:
1. Load grammar from a file
2. Analyze the grammar for left recursion and left factoring
3. Compute FIRST and FOLLOW sets
4. Build and print the parse table
5. Parse input strings or files based on the parse table
6. Output various results to files.

### Private Member Variables:
- `firstSetFile`, `followSetFile`, `parseTableFile`, `parsingFile`, `parsingTree`, `errorFile`, `grammarFile`: Output files for storing FIRST sets, FOLLOW sets, parse table, parsing steps, error logs, and grammar.
- `EPSILON`: A constant string representing the epsilon symbol in grammar.
- `grammar`: A map representing the grammar where the key is a non-terminal, and the value is a set of its productions.
- `firstSets`: A map representing the FIRST sets for each non-terminal.
- `followSets`: A map representing the FOLLOW sets for each non-terminal.
- `parseTable`: A map representing the parse table, with non-terminals as keys and another map containing terminal symbols and corresponding production rules.

It includes both private and public methods for processing the grammar and performing the syntactic analysis. The class is designed to work with context-free grammars that may need transformations to be suitable for LL(1) parsing.
### Public Functions:
1. **loadGrammarFromFile**: Loads a context-free grammar from a file.
2. **analyzeGrammar**: Analyzes the grammar to check for left recursion and left factoring.
3. **printGrammarToFile**: Writes the grammar to a file.
4. **computeFirstAndFollow**: Computes the FIRST and FOLLOW sets for all non-terminals.
5. **buildParseTable**: Constructs the LL(1) parse table for the grammar.
6. **writeParseTableToFile**: Writes the constructed parse table to a file.
7. **printParseTable**: Prints the parse table to the console.
8. **printTree**: Recursively prints the parsing tree for a given symbol.
9. **parseInput**: Parses an input string based on the constructed parse table.
10. **parseFromFile**: Parses an input string from a file using the parse table.

### Private Functions:
1. **splitProductions**: Splits a production rule into individual alternatives.
2. **tokenize**: Converts a production into a set of tokens.
3. **hasLeftFactoring**: Checks if the grammar contains left factoring.
4. **hasLeftRecursion**: Checks if the grammar contains left recursion.
5. **split**: Splits a string into tokens using a specified delimiter.
6. **trim**: Trims leading and trailing spaces from a string.
7. **removeLeftRecursion**: Removes left recursion from the grammar.
8. **removeLeftFactoring**: Removes left factoring from the grammar.
9. **computeFollow**: Computes the FOLLOW set for a non-terminal.
10. **computeFirst**: Computes the FIRST set for a non-terminal.
11. **isTerminal**: Checks if a token is a terminal.
12. **computeAllTerminals**: Computes the set of all terminal symbols.
13. **printFollowSetsToFile**: Prints the FOLLOW sets to a file.
14. **printFirstSetsToFile**: Prints the FIRST sets to a file.
15. **printGrammar**: Prints the grammar to the console.
</summary>
*/
class Synthetic 
{
    // |-------------------------------------------------------------------------------------------------------------------------------|
    // |-------------------------------------------------------------------------------------------------------------------------------|
    // |                                                                                                                               |
    // |                                              Private Functions  and Variables                                                 |
    // |                                                                                                                               |
    // |-------------------------------------------------------------------------------------------------------------------------------|
    // |-------------------------------------------------------------------------------------------------------------------------------|
private:

    // Files to be created
    std::ofstream firstSetFile;
    std::ofstream followSetFile;
    std::ofstream parseTableFile;
    std::ofstream parsingFile;
    std::ofstream parsingTree;
    std::ofstream errorFile;
    std::ofstream grammarFile;

    const std::string EPSILON = "ε";
    std::unordered_map<std::string, std::unordered_set<std::string>> grammar;
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;
    std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parseTable;
    // |-------------------------------------------------------------------------------------------------------------|
    // |                                          Helper Functions                                                   |
    // |-------------------------------------------------------------------------------------------------------------|

    /* <summary>
    This function splits a given production rule into individual productions, handling alternatives represented by the '|' (OR) symbol. If the '|' symbol appears in pairs (e.g., '||'), it's treated as a single token.

    ### Parameters:
    - `production`: A string representing a production rule. The rule may contain alternative productions separated by the '|' symbol.

    ### Returns:
    - A set of strings (`result`), where each element represents a separate production.

    ### Function Logic:
    1. **Processing Characters**:
       - The function iterates through each character of the `production` string.

    2. **Handling Alternatives (`|`)**:
       - If the `|` symbol is encountered:
         - If two consecutive `|` symbols are detected (`||`), treat them as a single token and append it to the current production.
         - Otherwise, treat the `|` as a delimiter between separate productions and add the current production to the result set.

    3. **Final Production**:
       - After processing the entire string, the function ensures the last production is added to the result.

    ### Example:

    For the production rule `"A -> α | β || γ"`, the function would return a set containing the productions `{"α", "β", "|| γ"}`.

    ### Complexity:
    - The time complexity is O(n), where `n` is the number of characters in the production string. This is because each character is processed exactly once.

    </summary>
    */
    std::unordered_set<std::string> splitProductions(const std::string& production)
    {
        std::unordered_set<std::string> result;
        std::string current;
        bool inOperator = false;

        for (size_t i = 0; i < production.length(); ++i)
        {
            if (production[i] == '|' && !inOperator)
            {
                // Check for '||' and handle it as a single token
                if (i + 1 < production.length() && production[i + 1] == '|')
                {
                    current += "||";
                    ++i; // Skip the next '|'
                }
                else
                {
                    // End of a single production
                    if (!current.empty())
                    {
                        result.insert(current);
                        current.clear();
                    }
                }
            }
            else
            {
                current += production[i];
            }
        }

        // Add the last production
        if (!current.empty())
        {
            result.insert(current);
        }

        return result;
    }

    /* <summary>
    This function tokenizes a given production rule by splitting it into individual tokens (symbols).

    ### Parameters:
    - `production`: A string representing a production rule. The rule may contain terminals, non-terminals, and the epsilon symbol ("ε").

    ### Returns:
    - A set of strings (`tokens`), where each element represents a token extracted from the production rule.

    ### Function Logic:
    1. **Input String Processing**:
       - The function reads the `production` string token by token, where each token is separated by whitespace.

    2. **Handling the Epsilon Symbol**:
       - If the token is the epsilon symbol ("ε"), it is inserted into the set as "NULL", which is a common representation for epsilon in grammar analysis.

    3. **Return Tokens**:
       - All tokens (including "NULL" for epsilon) are collected in the unordered set `tokens` and returned.

    ### Example:

    For the production rule `"A -> α β ε"`, the function would return a set with the tokens `{"A", "α", "β", "NULL"}`.

    ### Complexity:
    - The time complexity is O(n), where `n` is the number of tokens in the production rule. This is because the function processes each token exactly once.

    </summary>
    */
    std::unordered_set<std::string> tokenize(const std::string& production)
    {
        std::unordered_set<std::string> tokens;
        std::istringstream stream(production);
        std::string token;
        while (stream >> token)
        {
            if (token == "ε") 
            {
                tokens.insert("NULL");
            }
            else {
                tokens.insert(token);
            }
        }
        return tokens;
    }

    /* <summary>
    This function splits a given string into tokens based on a specified delimiter and returns a set of unique tokens.

    ### Parameters:
    - `str`: The input string that needs to be split.
    - `delimiter`: The character used to split the string into tokens.

    ### Returns:
    - A `std::unordered_set<std::string>` containing the tokens. The unordered set ensures that each token is unique (i.e., duplicates are discarded).

    ### Function Logic:
    1. **Initialize Token Stream**:
       - The function creates a `std::istringstream` object, `tokenStream`, initialized with the input string.

    2. **Split the String**:
       - It uses the `getline()` function to read from the `tokenStream` line by line, splitting the string at every occurrence of the specified `delimiter`. Each resulting token is inserted into the unordered set, ensuring uniqueness.

    3. **Return Unique Tokens**:
       - Once the entire string is processed, the function returns the unordered set containing all unique tokens.

    ### Example:

    For the input string `"apple,banana,apple,orange"` with the delimiter `','`, the function will return a set containing `{"apple", "banana", "orange"}`.

    ### Complexity:
    - The function reads through each character of the string once, so the time complexity is O(n), where `n` is the length of the input string. The insertion into the unordered set is O(1) on average.

    </summary>
    */
    std::unordered_set<std::string> split(const std::string& str, char delimiter)
    {
        std::unordered_set<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (getline(tokenStream, token, delimiter)) {
            tokens.insert(token);
        }
        return tokens;
    }

    /* <summary>
    This function trims leading and trailing whitespace characters (spaces, tabs, newlines, and carriage returns) from a given string.

    ### Parameters:
    - `str`: The input string that will be trimmed.

    ### Returns:
    - A new string that contains the input string with leading and trailing whitespace removed.

    ### Function Logic:
    1. **Find the First Non-Whitespace Character**:
       - The function uses `find_first_not_of()` to find the position of the first character that is not a whitespace character. If no non-whitespace characters are found, it returns `std::string::npos`.

    2. **Find the Last Non-Whitespace Character**:
       - Similarly, `find_last_not_of()` is used to find the last non-whitespace character.

    3. **Return Substring**:
       - If either `start` or `end` is `std::string::npos`, the function returns an empty string (indicating the input string was entirely whitespace).
       - Otherwise, it returns a substring from the `start` position to the `end` position (inclusive), effectively removing any leading or trailing whitespace characters.

    ### Example:

    For the input string `"   hello world  "`, the function will return `"hello world"`.

    ### Complexity:
    - The function performs a few `find` operations, each of which scans the string. The time complexity is O(n), where `n` is the length of the input string.

    </summary>
    */
    std::string trim(const std::string& str) {
        std::size_t start = str.find_first_not_of(" \t\n\r");
        std::size_t end = str.find_last_not_of(" \t\n\r");
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    /* <summary>
    This function checks whether a given token is a terminal symbol in the grammar.

    The function works as follows:

    1. **Look for the Token in Grammar**:
       - It checks if the provided token exists as a key in the `grammar` map (which contains non-terminal symbols as keys).

    2. **Return True or False**:
       - If the token is **not** found in the `grammar` map, this implies that the token is a terminal symbol (since non-terminals are keys in the `grammar` map). The function returns `true` to indicate the token is a terminal.
       - If the token is found in the `grammar` map, it is a non-terminal, and the function returns `false` to indicate the token is not a terminal.

    Example:

    Given the grammar:
    S -> A b A -> a | b

    If the token is `"a"`, the function returns `true` because `"a"` is not a key in the `grammar` map, indicating it's a terminal. Similarly, if the token is `"S"`, the function returns `false` because `"S"` is a non-terminal.

    </summary>
    */
    bool isTerminal(const std::string& token)
    {
        return grammar.find(token) == grammar.end();
    }

    /* <summary>
    This function computes and returns a set of all terminals present in the FIRST and FOLLOW sets of the grammar.

    The function works as follows:

    1. **Create a Set to Store Terminals**:
       - It initializes an unordered set called `allTerminals` that will store all unique terminal symbols.

    2. **Iterate Through FIRST Sets**:
       - It iterates through all entries in the `firstSets` map. For each entry, it adds all the symbols from the FIRST set (the second element of the map entry) to the `allTerminals` set.

    3. **Iterate Through FOLLOW Sets**:
       - Similarly, it iterates through all entries in the `followSets` map. Each symbol in the FOLLOW set is added to the `allTerminals` set.

    4. **Remove EPSILON**:
       - The function removes the `EPSILON` symbol from the `allTerminals` set, since `EPSILON` is not considered a terminal.

    5. **Return the Set of Terminals**:
       - Finally, the function returns the set `allTerminals`, which contains all unique terminals found in the grammar.

    Example:
    Given the following sets:
    FOLLOW(S) = { $ } FOLLOW(A) = { a b } FOLLOW(B) = { c }


    5. **Error Handling**:
       - If the file cannot be opened, an error message is displayed, and the program terminates.
    </summary>
    */
    std::unordered_set<std::string> computeAllTerminals()
    {
        std::unordered_set<std::string> allTerminals;
        for (const auto& entry : firstSets)
            allTerminals.insert(entry.second.begin(), entry.second.end());
        for (const auto& entry : followSets)
            allTerminals.insert(entry.second.begin(), entry.second.end());
        allTerminals.erase(EPSILON);
        return allTerminals;
    }

    // |-------------------------------------------------------------------------------------------------------------|
    // |                                          Ambiguity Checker                                                  |
    // |-------------------------------------------------------------------------------------------------------------|

    /* <summary>
    This function checks if a given context-free grammar contains left factoring.

    ### Parameters:
    - `grammar`: A reference to an unordered map that represents the grammar, where each key is a non-terminal symbol, and the corresponding value is a set of production rules for that non-terminal.

    ### Returns:
    - `true`: If left factoring is detected in the grammar.
    - `false`: If no left factoring is found.

    ### Function Logic:
    1. **Iterate through Grammar Rules**:
       - The function iterates over each non-terminal in the grammar.

    2. **Count Common Prefixes**:
       - For each production rule of a non-terminal, it extracts the prefix of the production up to the first space (if available). It uses a map (`prefixCount`) to count how many times each prefix appears in the production rules.

    3. **Detect Left Factoring**:
       - If any prefix appears in more than one production rule for a non-terminal (i.e., it has a count greater than 1), it indicates left factoring, and the function immediately returns `true`.

    4. **Return Result**:
       - If no left factoring is detected after checking all rules, the function returns `false`.

    ### Example:

    For a grammar with the production rule `A -> alpha beta | alpha gamma`, the function will detect left factoring due to the common prefix `alpha`.

    ### Complexity:
    - The time complexity is O(n*m), where `n` is the number of non-terminals and `m` is the average number of production rules per non-terminal. This is because the function processes each production rule and checks for common prefixes.

    </summary>
    */
    bool hasLeftFactoring(const std::unordered_map<std::string, std::unordered_set<std::string>>& grammar) {
        for (const auto& rule : grammar) {
            std::unordered_map<std::string, int> prefixCount;

            for (const std::string& prod : rule.second) {
                std::size_t pos = prod.find(' ');
                std::string prefix = (pos == std::string::npos) ? prod : prod.substr(0, pos);
                prefixCount[prefix]++;
            }

            for (const auto& count : prefixCount) {
                if (count.second > 1) {
                    return true;
                }
            }
        }
        return false;
    }

    /* <summary>
    This function checks if a given context-free grammar contains left recursion.

    ### Parameters:
    - `grammar`: A reference to an unordered map that represents the grammar, where each key is a non-terminal symbol, and the corresponding value is a set of production rules for that non-terminal.

    ### Returns:
    - `true`: If left recursion is detected in the grammar.
    - `false`: If no left recursion is found.

    ### Function Logic:
    1. **Iterate through Grammar Rules**:
       - The function iterates over each non-terminal in the grammar.

    2. **Check for Left Recursion**:
       - For each production rule of a non-terminal, it checks whether the production starts with the non-terminal itself. If a production has this form, it indicates left recursion, and the function immediately returns `true`.

    3. **Return Result**:
       - If no left-recursive production is found after checking all rules, the function returns `false`.

    ### Example:

    For a grammar with a production rule `A -> A alpha | beta`, the function will detect left recursion in the rule `A -> A alpha`.

    ### Complexity:
    - The time complexity is O(n*m), where `n` is the number of non-terminals and `m` is the average number of production rules per non-terminal. This is because the function checks each production rule for left recursion.

    </summary>
    */
    bool hasLeftRecursion(const std::unordered_map<std::string, std::unordered_set<std::string>>& grammar) {
        for (const auto& rule : grammar) {
            std::string nonTerminal = rule.first;
            for (const std::string& prod : rule.second) {
                if (prod.find(nonTerminal) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    // |-------------------------------------------------------------------------------------------------------------|
    // |                                          Ambiguity Remover                                                  |
    // |-------------------------------------------------------------------------------------------------------------|

    /* <summary>
    This function removes left recursion from a context-free grammar. Left recursion occurs when a non-terminal appears in the first position of one of its own productions, which can lead to infinite recursion in parsers like recursive descent. The function removes this type of recursion and transforms the grammar into an equivalent one without left recursion.

    ### Steps of the Algorithm:

    1. **Iterate Through Grammar Rules**:
       - The function processes each non-terminal and its corresponding productions. It checks for left-recursive productions and separates them from non-left-recursive productions.

    2. **Separate Left-Recursive and Non-Left-Recursive Productions**:
       - For each production of a non-terminal, the function checks if the production starts with the non-terminal itself (`prod.find(nonTerminal) == 0`). This condition indicates that the production is left-recursive.
       - The left-recursive productions are stored in a set called `alpha`, while the non-left-recursive productions are stored in a set called `beta`.

    3. **Remove Left Recursion**:
       - If any left-recursive productions are found (`alpha` is not empty), the function applies a transformation to eliminate the recursion:
         - A new non-terminal (e.g., `nonTerminal'`) is created to represent the recursive part of the grammar.
         - The original non-terminal is updated to have productions that follow the pattern: `<nonTerminal> -> <beta> <nonTerminal'>`.
         - The new non-terminal (e.g., `nonTerminal'`) gets the recursive part of the productions, i.e., `<nonTerminal'> -> <alpha> <nonTerminal'>`.
         - The new non-terminal also receives an epsilon production (`^`), representing the case where the recursion terminates.

    4. **Handling Left Recursion**:
       - After left recursion removal, the grammar now has productions where the non-terminal does not directly recurse in the leftmost position, avoiding infinite recursion during parsing.

    ### Example:

    Consider the following grammar with left recursion:
    A -> A alpha | beta
    Here, `A` has a left-recursive production `A -> A alpha`. After applying the left recursion removal technique, the grammar is transformed into:
    A -> beta A' A' -> alpha A' | ^
    This transformation removes the left recursion and allows for parsing with techniques like recursive descent.

    ### Complexity:
    - The function processes each non-terminal and its productions. For each production, the function checks if it is left-recursive, and the overall complexity depends on the number of non-terminals and productions in the grammar.

    </summary>
    */
    void removeLeftRecursion(std::unordered_map<std::string, std::unordered_set<std::string>>& grammar) {
        for (auto& rule : grammar) {
            std::string nonTerminal = rule.first;
            std::unordered_set<std::string> productions = rule.second;
            std::unordered_set<std::string> alpha, beta;

            // Separate left-recursive and non-left-recursive productions
            for (const std::string& prod : productions) {
                if (prod.find(nonTerminal) == 0) {
                    alpha.insert(prod.substr(nonTerminal.size())); // Remove the left-recursive part
                }
                else {
                    beta.insert(prod);
                }
            }

            // If left recursion exists, remove it
            if (!alpha.empty()) {
                std::string newNonTerminal = nonTerminal + "'";
                grammar[nonTerminal].clear();

                for (const std::string& b : beta) {
                    grammar[nonTerminal].insert(b + " " + newNonTerminal);
                }

                for (const std::string& a : alpha) {
                    grammar[newNonTerminal].insert(a + " " + newNonTerminal);
                }

                grammar[newNonTerminal].insert("^"); // Add epsilon production
            }
        }
    }

    /* <summary>
    This function removes left factoring from a context-free grammar. Left factoring is a technique used to transform a grammar that has common prefixes in its productions into an equivalent grammar without the common prefixes.

    ### Steps of the Algorithm:

    1. **Iterate Through Grammar Rules**:
       - The function processes each non-terminal and its corresponding productions. It checks for common prefixes in the productions of a non-terminal.

    2. **Group Productions by Common Prefix**:
       - For each production of a non-terminal, the function extracts the common prefix (if any) by finding the first space in the production. This is done using the `find` function. If there is no space (i.e., the production has no space, meaning no further symbols follow the first one), the entire production is treated as the prefix.
       - A `prefixMap` is created to group all productions that share the same prefix. Each group contains all productions that start with the same prefix.

    3. **Check for Left Factoring**:
       - If the number of unique prefixes (`prefixMap.size()`) is less than the total number of productions for that non-terminal (`productions.size()`), left factoring is detected. This indicates that some productions share a common prefix.

    4. **Apply Left Factoring**:
       - If left factoring is required, the function:
         - Clears the productions for the current non-terminal.
         - Iterates through each group of productions in the `prefixMap`. For groups that have more than one production (indicating the need for left factoring), the function creates a new non-terminal (e.g., `nonTerminal'`) to represent the rest of the production after the common prefix.
         - The productions of the original non-terminal are updated to include the prefix followed by the new non-terminal.
         - The new non-terminal gets the remaining part of each production after the common prefix.

    5. **Handle Productions Without Left Factoring**:
       - If a group has only one production (no common prefix with other productions), the production is directly inserted into the original non-terminal’s set of productions without modification.

    ### Example:

    Consider the following grammar with left factoring:
    A -> a B | a C | b

    - The common prefix "a" in `a B` and `a C` is factored out.

    After left factoring, the grammar will look like:

    A -> a A' A' -> B | C A -> b
    ### Complexity:
    - The function processes each non-terminal and its productions, which involves grouping productions by common prefixes and potentially creating new non-terminals. The overall complexity depends on the number of non-terminals and productions in the grammar, but the main work involves iterating through productions and matching prefixes.

    </summary>
    */
    void removeLeftFactoring(std::unordered_map<std::string, std::unordered_set<std::string>>& grammar) {
        for (auto& rule : grammar) {
            std::string nonTerminal = rule.first;
            std::unordered_set<std::string> productions = rule.second;
            std::unordered_map<std::string, std::unordered_set<std::string>> prefixMap;

            // Group productions by common prefix
            for (const std::string& prod : productions) {
                std::size_t pos = prod.find(' ');
                std::string prefix = (pos == std::string::npos) ? prod : prod.substr(0, pos);
                prefixMap[prefix].insert(prod);
            }

            // If left factoring exists, remove it
            if (prefixMap.size() < productions.size()) {
                grammar[nonTerminal].clear();

                for (const auto& prefixPair : prefixMap) {
                    const std::string& prefix = prefixPair.first;
                    const std::unordered_set<std::string>& group = prefixPair.second;

                    if (group.size() > 1) {
                        std::string newNonTerminal = nonTerminal + "'";
                        grammar[nonTerminal].insert(prefix + " " + newNonTerminal);

                        for (const std::string& g : group) {
                            grammar[newNonTerminal].insert(trim(g.substr(prefix.size())));
                        }
                    }
                    else {
                        grammar[nonTerminal].insert(*group.begin());
                    }
                }
            }
        }
    }

    // |-------------------------------------------------------------------------------------------------------------|
    // |                                     Computation of First and Follow                                         |
    // |-------------------------------------------------------------------------------------------------------------|

    /* <summary>
    This function computes the FOLLOW set for a given non-terminal in a context-free grammar.

    ### Explanation of the Algorithm:

    1. **Base Case Check**:
       - If the `nonTerminal` has already been visited (i.e., it is present in the `visited` set) or its FOLLOW set is already computed (i.e., `followSets[nonTerminal].empty()` is `false`), the function simply returns to avoid redundant work.

    2. **Mark Non-Terminal as Visited**:
       - The `nonTerminal` is inserted into the `visited` set to prevent recalculating its FOLLOW set during recursive calls.

    3. **Add '$' to FOLLOW of the Start Symbol**:
       - The start symbol (first non-terminal in the grammar) has `"$"` (indicating end of input) in its FOLLOW set. This is a standard rule in context-free grammars.

    4. **Iterate Through Grammar Productions**:
       - The function iterates over all productions in the grammar to find occurrences of the `nonTerminal` in the right-hand side (RHS) of the productions.
       - For each production, the tokens are split and analyzed to determine if the `nonTerminal` is present.

    5. **Processing Productions**:
       - If the `nonTerminal` is found in a production, the function looks at the tokens that follow it:
         - If the next token is a **terminal**, it is directly added to the FOLLOW set of the `nonTerminal`, and the processing for this production stops.
         - If the next token is a **non-terminal**, the FIRST set of that non-terminal is added to the FOLLOW set of the `nonTerminal`. The function also removes `"NULL"` (representing epsilon) from the set.
         - If the next token can derive the empty string (i.e., if its FIRST set contains `"NULL"`), the function continues to the next token in the production.

    6. **Handling Nullable Productions**:
       - If all the tokens after the `nonTerminal` in the production can derive the empty string (i.e., they are nullable), the function recursively computes the FOLLOW set for the left-hand side (`LHS`) of the production and adds the FOLLOW set of the `LHS` to the FOLLOW set of the `nonTerminal`.

    7. **Recursive Call**:
       - If the FOLLOW set of the `nonTerminal` depends on the FOLLOW set of another non-terminal (`LHS`), a recursive call to `computeFollow` is made to compute the FOLLOW set of the `LHS`.

    ### Example Walkthrough:

    Given the following grammar:
    S -> A B A -> a | NULL B -> b

    - **FOLLOW(S)**:
      - Since `S` is the start symbol, its FOLLOW set will contain `"$"`.

    - **FOLLOW(A)**:
      - `A` is found in `S -> A B`. Since `B` follows `A` and `B` is a terminal (`b`), `b` is added to the FOLLOW set of `A`.
      - Thus, `FOLLOW(A)` will be `{b}`.

    - **FOLLOW(B)**:
      - `B` is the last token in the production `S -> A B`, so `FOLLOW(B)` will include `FOLLOW(S)`, which is `"$"`.
      - Thus, `FOLLOW(B)` will be `{"$"}`.

    ### Complexity:
    - The function iterates through all grammar productions and applies the rules for computing the FOLLOW set. It may make recursive calls, so the complexity depends on the size of the grammar and the recursive depth required to compute FOLLOW sets.

    </summary>
    */
    void computeFollow(const std::string& nonTerminal, std::unordered_set<std::string>& visited)
    {
        // If already visited or follow set computed, return
        if (visited.count(nonTerminal) || !followSets[nonTerminal].empty()) 
        {
            return;
        }

        visited.insert(nonTerminal);

        // Add "$" to FOLLOW set of the start symbol
        if (nonTerminal == grammar.begin()->first) 
        {
            followSets[nonTerminal].insert("$");
        }

        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = grammar.begin(); it != grammar.end(); ++it)
        {
            const std::string& lhs = it->first;
            const std::unordered_set<std::string>& productions = it->second;

            for (std::unordered_set<std::string>::iterator prodIt = productions.begin(); prodIt != productions.end(); ++prodIt)
            {
                const std::string& production = *prodIt;
                std::istringstream stream(production);
                std::vector<std::string> tokens;
                std::string token;
                while (stream >> token) {
                    tokens.push_back(token);
                }

                for (size_t i = 0; i < tokens.size(); ++i) 
                {
                    if (tokens[i] == nonTerminal) 
                    {
                        bool addFollowOfLHS = true;

                        // Add FIRST of the following tokens to FOLLOW(nonTerminal)
                        for (size_t j = i + 1; j < tokens.size(); ++j) 
                        {
                            const std::string& nextToken = tokens[j];
                            if (grammar.find(nextToken) == grammar.end()) 
                            {
                                followSets[nonTerminal].insert(nextToken); // Terminal
                                addFollowOfLHS = false;
                                break;
                            }

                            followSets[nonTerminal].insert(firstSets[nextToken].begin(), firstSets[nextToken].end());
                            followSets[nonTerminal].erase("NULL");

                            if (!firstSets[nextToken].count("NULL")) 
                            {
                                addFollowOfLHS = false;
                                break;
                            }
                        }

                        // If all subsequent tokens can derive NULL, add FOLLOW(LHS)
                        if (addFollowOfLHS) 
                        {
                            computeFollow(lhs, visited);
                            followSets[nonTerminal].insert(followSets[lhs].begin(), followSets[lhs].end());
                        }
                    }
                }
            }
        }
    }

    /* <summary>
    This function computes the FIRST set for a given non-terminal in a context-free grammar.

    ### Explanation of the Algorithm:

    1. **Base Case Check**:
       - If the `nonTerminal` has already been visited (i.e., it is present in the `visited` set) or its FIRST set is already computed (i.e., `firstSets[nonTerminal].empty()` is `false`), the function simply returns, preventing redundant work and infinite recursion.

    2. **Mark Non-Terminal as Visited**:
       - The `nonTerminal` is inserted into the `visited` set to avoid recalculating its FIRST set during recursive calls.

    3. **Process Productions**:
       - The function iterates over all the productions for the `nonTerminal` in the grammar. Each production is treated as a sequence of tokens.
       - A stream (`std::istringstream`) is used to break the production into individual tokens.

    4. **Handling Tokens in the Production**:
       - The function checks each token in the production to determine if it is a terminal or another non-terminal:
         - If the token is a **terminal**, it is directly added to the FIRST set of the `nonTerminal`, and the loop is terminated (since a terminal symbol is the first symbol of a production).
         - If the token is a **non-terminal**, the function recursively computes the FIRST set for that non-terminal by calling `computeFirst` for the token. The FIRST set of the token is then added to the FIRST set of the `nonTerminal`.

    5. **Nullable Tokens**:
       - If the token is a non-terminal and its FIRST set contains `"NULL"` (representing epsilon, i.e., the empty string), the current non-terminal's production can derive the empty string, making it **nullable**. If a nullable non-terminal is encountered, the algorithm continues to process the next token in the production.

    6. **Epsilon Production**:
       - If a production can derive the empty string (i.e., nullable), `"NULL"` is added to the FIRST set of the `nonTerminal`.

    ### Example Walkthrough:

    - Suppose we have the following grammar:
        ```
        S -> A B
        A -> a | NULL
        B -> b
        ```

    - **First Set Calculation**:
        - `FIRST(S)`:
            - `S`'s production is `A B`.
            - `A` is processed, which can derive `a` and `NULL`.
            - Since `A` can be `NULL`, the function moves to `B` and adds `b` to `FIRST(S)`.
            - Thus, `FIRST(S)` will be `{a, b}`.
        - `FIRST(A)`:
            - `A`'s production is `a` or `NULL`.
            - `FIRST(A)` will be `{a, NULL}`.
        - `FIRST(B)`:
            - `B`'s production is `b`.
            - `FIRST(B)` will be `{b}`.

    </summary>
    */
    void computeFirst(const std::string& nonTerminal, std::unordered_set<std::string>& visited)
    {
        if (visited.count(nonTerminal) || !firstSets[nonTerminal].empty()) 
            return;

        visited.insert(nonTerminal);

        for (const auto& production : grammar[nonTerminal]) 
        {
            std::istringstream stream(production);
            std::string token;
            bool nullable = true;

            while (nullable && stream >> token) 
            {
                nullable = false;

                if (isTerminal(token)) 
                {
                    firstSets[nonTerminal].insert(token);
                    break;
                }

                computeFirst(token, visited);
                firstSets[nonTerminal].insert(firstSets[token].begin(), firstSets[token].end());
                if (firstSets[token].count("NULL"))
                {
                    nullable = true;
                    firstSets[nonTerminal].erase("NULL");
                }
            }

            if (nullable) firstSets[nonTerminal].insert("NULL");
        }
    }

    // |-------------------------------------------------------------------------------------------------------------|
    // |                                              Output Functions                                               |
    // |-------------------------------------------------------------------------------------------------------------|

    /* <summary>
    This function prints the FOLLOW sets of all non-terminals in the grammar to a file named "FollowSet.txt".

    The function operates as follows:

    1. **Open the File**:
       - It opens a file named `FollowSet.txt` in write mode. If the file cannot be opened, an error message is printed to the standard error stream, and the program exits.

    2. **Iterate Through the FOLLOW Sets**:
       - The function loops through the `followSets` data structure, which is a map where the key is a non-terminal, and the value is a set of strings representing the FOLLOW set of that non-terminal.

    3. **Print Each FOLLOW Set**:
       - For each non-terminal, the function writes the FOLLOW set to the file in the following format:
         - `FOLLOW(non-terminal) = { ... }` where the non-terminal is the key, and the set of FOLLOW symbols is printed inside curly braces `{}`.
       - Each symbol in the FOLLOW set is separated by a space.

    4. **File Closing**:
       - After all FOLLOW sets are written, the function closes the file.

    Example Output in the file:
    FOLLOW(S) = { $ } FOLLOW(A) = { a b } FOLLOW(B) = { c }


    5. **Error Handling**:
       - If the file cannot be opened, an error message is displayed, and the program terminates.
    </summary> */
    void printFollowSetsToFile()
    {
        followSetFile.open("FollowSet.txt");
        if (!followSetFile)
        {
            std::cerr << "Error: Unable to open FollowSet File." << std::endl;
            exit(1);
        }
        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = followSets.begin(); it != followSets.end(); ++it)
        {
            const std::string& nonTerminal = it->first;
            const std::unordered_set<std::string>& followSet = it->second;
            followSetFile << "FOLLOW(" << nonTerminal << ") = { ";
            for (std::unordered_set<std::string>::iterator setIt = followSet.begin(); setIt != followSet.end(); ++setIt)
            {
                followSetFile << *setIt << " ";
            }
            followSetFile << "}\n";
        }
        followSetFile.close();
    }

    /* <summary>
    This function prints the FIRST sets of all non-terminals in the grammar to a file named "FirstSet.txt".

    The function works as follows:

    1. **Open the File**:
       - The function opens a file named `FirstSet.txt` in write mode. If the file cannot be opened, an error message is printed to the standard error stream, and the program exits.

    2. **Iterate Through the FIRST Sets**:
       - The function loops through the `firstSets` data structure, which is a map where the key is a non-terminal, and the value is a set of strings representing the FIRST set of that non-terminal.

    3. **Print Each FIRST Set**:
       - For each non-terminal, the function writes the FIRST set to the file in the following format:
         - `FIRST(non-terminal) = { ... }` where the non-terminal is the key, and the set of FIRST symbols is printed inside curly braces `{}`.
       - Each symbol in the FIRST set is separated by a space.

    4. **File Closing**:
       - Once all FIRST sets have been written, the function closes the file.

    Example Output in the file:
    FIRST(S) = { a b } FIRST(A) = { a } FIRST(B) = { b }

    5. **Error Handling**:
       - If the file cannot be opened, an error message is displayed, and the program terminates.
    </summary> */
    void printFirstSetsToFile()
    {
        firstSetFile.open("FirstSet.txt");
        if (!firstSetFile) 
        {
            std::cerr << "Error: Unable to open FirstSet File." << std::endl;
            exit(1);
        }
        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = firstSets.begin(); it != firstSets.end(); ++it)
        {
            const std::string& nonTerminal = it->first;
            const std::unordered_set<std::string>& firstSet = it->second;
            firstSetFile << "FIRST(" << nonTerminal << ") = { ";
            for (std::unordered_set<std::string>::iterator setIt = firstSet.begin(); setIt != firstSet.end(); ++setIt)
            {
                firstSetFile << *setIt << " ";
            }
            firstSetFile << "}\n";
        }
        firstSetFile.close();
    }

    /* <summary>
    This function prints the contents of the grammar to the standard output (console).

    The function works as follows:

    1. **Display the Header**:
       - It first prints the header "Grammar Contents:" to inform the user that the grammar will be displayed next.

    2. **Iterate Through the Grammar**:
       - The function loops through each entry in the `grammar` data structure, which is assumed to be a map where the key is a non-terminal and the value is a set of corresponding productions.

    3. **Print Each Production Rule**:
       - For each non-terminal, it prints the non-terminal's name followed by " -> ".
       - The function then iterates over the set of productions for that non-terminal. Each production is printed, and if there are multiple productions, they are separated by the " | " symbol.
       - A flag `first` is used to ensure that the first production does not have a leading separator.

    4. **Formatting**:
       - After printing the productions for a non-terminal, the function prints a newline to separate each production rule.

    5. **End the Output**:
       - The function adds an additional newline after printing the entire grammar for better readability.

    For example, if the grammar contains:
    S -> A B A -> a B -> b

    The output will be:
    Grammar Contents: S -> A B A -> a B -> b
    </summary> */
    void printGrammar()
    {
        std::cout << "Grammar Contents:" << std::endl;
        for (const auto& entry : grammar)
        {
            const std::string& nonTerminal = entry.first;
            const std::unordered_set<std::string>& productions = entry.second;

            std::cout << nonTerminal << " -> ";
            bool first = true;
            for (const auto& production : productions)
            {
                if (!first) std::cout << " | ";
                std::cout << production;
                first = false;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }

    // |-------------------------------------------------------------------------------------------------------------------------------|
    // |-------------------------------------------------------------------------------------------------------------------------------|
    // |                                                                                                                               |
    // |                                           Public Functions and Variables                                                      |
    // |                                                                                                                               |
    // |-------------------------------------------------------------------------------------------------------------------------------|
    // |-------------------------------------------------------------------------------------------------------------------------------|
public:

    // |-------------------------------------------------------------------------------------------------------------|
    // |                                             Core Functions                                                  |
    // |-------------------------------------------------------------------------------------------------------------|

    //
    /*<summary>
    This function loads a context-free grammar (CFG) from a file into a data structure for further processing.

    The function works as follows:

    1. **Open the File**:
       - It tries to open the file specified by the `fileName` parameter using `std::ifstream`.
       - If the file cannot be opened (e.g., the file doesn't exist), it prints an error message and returns from the function.

    2. **Read Lines from the File**:
       - The function reads the file line by line using `getline()`.
       - For each line, it checks if it contains a production rule by looking for the "->" symbol. If "->" is found, the line is processed as a valid production.

    3. **Extract Non-Terminal and Productions**:
       - The line is split into two parts:
         - The non-terminal, which is the part before "->".
         - The production, which is the part after "->".
       - The `splitProductions()` function is used to handle the splitting of the productions on the right-hand side of the production rule.

    4. **Store the Grammar**:
       - The extracted non-terminal and its corresponding productions are stored in the `grammar` data structure. The grammar is assumed to be stored as a map of non-terminals to their corresponding productions.

    5. **Close the File**:
       - After all lines are processed, the file is closed.

    This function is essential for loading a grammar from a file in a format like:
    S -> A B A -> a B -> b
    Where each line represents a production rule in the format: `Non-terminal -> production`.
    </summary> */
    void loadGrammarFromFile(const std::string& fileName)
    {
        std::ifstream file(fileName);
        if (!file.is_open()) 
        {
            std::cerr << "Error: Unable to open file " << fileName << std::endl;
            return;
        }

        std::string line;
        while (getline(file, line)) 
        {
            size_t pos = line.find("->");
            if (pos != std::string::npos)
            {
                std::string nonTerminal = line.substr(0, pos - 1);
                std::string production = line.substr(pos + 2);
                grammar[nonTerminal] = splitProductions(production);
            }
        }
        file.close();
    }

    /* <summary>
    This function analyzes the grammar for left factoring and left recursion and attempts to remove them if detected.

    The function performs the following tasks:

    1. **Detect Left Factoring**:
       - The function iterates over all the non-terminals in the grammar and checks if left factoring is present using `hasLeftFactoring()`.
       - If left factoring is detected, it calls `removeLeftFactoring()` to modify the grammar by eliminating the left factoring.
       - After removal, it checks if left factoring is still present and prints appropriate messages indicating the results.

    2. **Detect Left Recursion**:
       - The function then checks for left recursion in the grammar. If left recursion is detected using `hasLeftRecursion()`, it attempts to remove it by calling `removeLeftRecursion()`.
       - Similar to left factoring, after removal, the function checks if left recursion is still present and prints corresponding messages.

    3. **Print Updated Grammar**:
       - The updated grammar (after potential transformations) is printed using the `printGrammar()` function.

    4. **Final Output**:
       - If no left factoring or left recursion is found, the function prints appropriate messages indicating their absence.
       - The function returns `true` if no left factoring or left recursion is detected (indicating the grammar is clean), and `false` otherwise.

    This function helps in detecting and cleaning up common issues in context-free grammars (CFG) that can complicate parsing, such as left recursion and left factoring.

    </summary>
    */
    bool analyzeGrammar() 
    {
        bool leftFactoringFound = false;
        bool leftRecursionFound = false;
        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = grammar.begin(); it != grammar.end(); ++it)
        {
            
            const std::string& nonTerminal = it->first;
            if (hasLeftFactoring(grammar)) 
            {
                std::cout << "Left factoring detected in: " << nonTerminal << std::endl;
                removeLeftFactoring(grammar);
                leftFactoringFound = hasLeftFactoring(grammar);
                if (leftFactoringFound)
                {
                    std::cout << "Left factoring detected after removal: " << nonTerminal << std::endl;
                }
                else
                {
                    std::cout << "Left factoring not detected after removal: " << nonTerminal << std::endl;
                }
            }
        }

        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = grammar.begin(); it != grammar.end(); ++it)
        {
            const std::string& nonTerminal = it->first;
            if (hasLeftRecursion(grammar)) 
            {
                std::cout << "Left recursion detected in: " << nonTerminal << std::endl;
                removeLeftRecursion(grammar);
                leftRecursionFound = hasLeftRecursion(grammar);
                if (leftRecursionFound)
                {
                    std::cout << "Left Recursion detected after removal: " << nonTerminal << std::endl;
                }
                else
                {
                    std::cout << "Left Recursion not detected after removal: " << nonTerminal << std::endl;
                }
            }
        }
        printGrammar();
        if (!leftFactoringFound) 
        {
            std::cout << "No left factoring detected.\n";
        }
        if (!leftRecursionFound) 
        {
            std::cout << "No left recursion detected.\n";
        }

        return !(leftFactoringFound || leftRecursionFound);
    }

    /* <summary>
    This function calculates the FIRST and FOLLOW sets for all non-terminals in the grammar and outputs them in a readable format.
    It ensures that all FIRST sets are computed before FOLLOW sets, as FOLLOW computation depends on FIRST. The results are printed to files as well as displayed in a table format for easy debugging and verification.

    Logic:
    1. Compute FIRST sets:
       - Traverse through each non-terminal in the grammar.
       - Use a recursive helper function `computeFirst` to populate the FIRST set for each non-terminal.
       - Track visited non-terminals to prevent infinite recursion or redundant computations.

    2. Compute FOLLOW sets:
       - Reset the visited set to start fresh.
       - Traverse through each non-terminal in the grammar again.
       - Use a recursive helper function `computeFollow` to populate the FOLLOW set for each non-terminal.
       - FOLLOW sets are computed after all FIRST sets are finalized.

    3. Output the results:
       - Write FIRST and FOLLOW sets to separate files for detailed inspection.
       - Display a formatted table in the console with columns for:
         - Non-terminal name
         - FIRST set as a space-separated string
         - FOLLOW set as a space-separated string

    4. The output allows developers to verify the correctness of the FIRST and FOLLOW sets and identify any issues in the grammar.

    </summary>
    */
    void computeFirstAndFollow() 
    {
        std::unordered_set<std::string> visited;
        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = grammar.begin(); it != grammar.end(); ++it)
        {
            computeFirst(it->first, visited);
        }

        visited.clear();
        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = grammar.begin(); it != grammar.end(); ++it)
        {
            computeFollow(it->first, visited);
        }

        // Print FIRST and FOLLOW sets separately
        printFirstSetsToFile();
        printFollowSetsToFile();

        // Display FIRST and FOLLOW sets in table format
        std::cout << std::left << std::setw(20) << "Non-terminal"
            << std::setw(40) << "First"
            << std::setw(40) << "Follow" << std::endl;

        for (std::unordered_map<std::string, std::unordered_set<std::string>>::iterator it = grammar.begin(); it != grammar.end(); ++it)
        {
            const std::string& nonTerminal = it->first;

            std::cout << std::setw(20) << nonTerminal;

            std::string firstSet;
            for (std::unordered_set<std::string>::iterator firstIt = firstSets[nonTerminal].begin(); firstIt != firstSets[nonTerminal].end(); ++firstIt)
            {
                firstSet += *firstIt + " ";
            }
            std::cout << std::setw(40) << firstSet;

            std::string followSet;
            for (std::unordered_set<std::string>::iterator followIt = followSets[nonTerminal].begin(); followIt != followSets[nonTerminal].end(); ++followIt)
            {
                followSet += *followIt + " ";
            }
            std::cout << followSet << std::endl;
        }
    }

    /* <summary>
    This function builds the LL(1) parse table based on the provided grammar, FIRST sets, and FOLLOW sets. It determines the appropriate productions for each non-terminal and terminal pair, populating the parse table and handling error recovery through the addition of `sync` entries.

    Logic:
    1. Iterate through each non-terminal and its associated productions in the grammar.
    2. For each production:
       - Compute its FIRST set:
         - Traverse each token in the production sequentially.
         - If the token is a terminal, add it to the FIRST set and terminate traversal.
         - If the token is a non-terminal, merge its FIRST set into the current FIRST set.
         - If a non-terminal includes EPSILON, continue to the next token; otherwise, stop.
       - If the production is nullable, include EPSILON in the FIRST set.
    3. Populate the parse table with entries for terminals in the FIRST set of the production, except for EPSILON.
    4. For nullable productions (containing EPSILON), add entries for terminals in the FOLLOW set of the non-terminal to the parse table.
    5. Handle error recovery using panic mode by:
       - Adding `sync` entries to the parse table for terminals in the FOLLOW set of the non-terminal if no production exists for those terminals.
    6. The parse table is now complete, and every cell is either populated with a production or a `sync` entry for error recovery.
    </summary> */
    void buildParseTable()
    {
        for (const auto& entry : grammar)
        {
            const std::string& nonTerminal = entry.first;
            for (const auto& production : entry.second)
            {
                std::unordered_set<std::string> firstOfProduction;
                bool isNullable = true;

                std::istringstream stream(production);
                std::string token;

                // Compute FIRST set for the production
                while (isNullable && stream >> token)
                {
                    isNullable = false;

                    if (isTerminal(token))
                    {
                        firstOfProduction.insert(token);
                        break;
                    }
                    else
                    {
                        firstOfProduction.insert(firstSets[token].begin(), firstSets[token].end());
                        isNullable = firstSets[token].count(EPSILON) > 0;
                        if (isNullable)
                            firstOfProduction.erase(EPSILON);
                    }
                }

                if (isNullable)
                    firstOfProduction.insert(EPSILON);

                // Populate parse table with FIRST set entries
                for (const std::string& terminal : firstOfProduction)
                {
                    if (terminal != EPSILON)
                        parseTable[nonTerminal][terminal] = production;
                }

                // Populate parse table with FOLLOW set entries if nullable
                if (firstOfProduction.count(EPSILON) > 0)
                {
                    for (const std::string& followSymbol : followSets[nonTerminal])
                    {
                        parseTable[nonTerminal][followSymbol] = production;
                    }
                }
            }

            // Panic mode: Add 'sync' to empty cells in the row for the non-terminal
            for (const std::string& followSymbol : followSets[nonTerminal])
            {
                if (parseTable[nonTerminal][followSymbol].empty())
                {
                    parseTable[nonTerminal][followSymbol] = "sync";
                }
            }
        }
    }

    // |-------------------------------------------------------------------------------------------------------------|
    // |                                              Output Functions                                               |
    // |-------------------------------------------------------------------------------------------------------------|

    /* <summary>
    This function writes the current grammar (in non-ambiguous CFG format) to a file named `Updated_NoAmbiguity_CFG.txt`.
    The function appends the contents of the grammar to the file, ensuring that previous data in the file remains intact.

    Logic:
    1. Open the file in append mode (`std::ios::app`) to preserve previous content.
       - If the file cannot be opened, display an error message and exit the program.

    2. Write the grammar to the file:
       - Iterate through each non-terminal and its corresponding productions in the grammar.
       - Print the non-terminal followed by its productions, separated by " | " if there are multiple alternatives.
       - Use a flag (`first`) to handle formatting, ensuring no leading " | " appears.

    3. Separate the grammar content visually:
       - Add a double newline at the end of the grammar content for better readability.

    4. This function allows developers to inspect the grammar transformations applied during parsing preparation (e.g., removing ambiguity, left recursion).

    </summary>
    */
    void printGrammarToFile()
    {
        grammarFile.open("Updated_NoAmbiguity_CFG.txt", std::ios::app);
        if (!grammarFile)
        {
            std::cerr << "Error: Unable to open Updated_NoAmbiguity_CFG File." << std::endl;
            exit(1);
        }
        grammarFile << "Grammar Contents:" << std::endl;
        for (const auto& entry : grammar)
        {
            const std::string& nonTerminal = entry.first;
            const std::unordered_set<std::string>& productions = entry.second;

            grammarFile << nonTerminal << " -> ";
            bool first = true;
            for (const auto& production : productions)
            {
                if (!first) grammarFile << " | ";
                grammarFile << production;
                first = false;
            }
            grammarFile << std::endl;
        }
        grammarFile << std::endl << std::endl;
    }

    /* <summary>
    This function writes the parse table to a file in a structured and tabular format. It includes the grammar rules for each non-terminal and terminal pair, ensuring proper alignment and readability in the output file.

    Logic:
    1. Extract all unique terminals from the parse table and store them in an unordered set.
    2. Convert the set into a sorted vector to maintain consistent column ordering.
    3. Open a file named `ParseTable.txt` for writing in binary mode to support UTF-8 encoding.
    4. Write a UTF-8 Byte Order Mark (BOM) at the beginning of the file for compatibility with editors.
    5. Define helper functions to:
       - Print a row separator to delineate table rows.
       - Print the header row with column names for non-terminals and terminals.
    6. Write the header row and separator to the file.
    7. For each non-terminal in the parse table:
       - Print its name in the first column.
       - Iterate through the sorted terminal list and:
         - Print the corresponding grammar rule if a mapping exists.
         - Otherwise, print a placeholder (`-`) for missing entries.
    8. Write the final row separator to close the table structure.
    9. Close the file after writing all content.
    </summary> */
    void writeParseTableToFile()
    {
        // Compute all unique terminals and store them in a set
        std::unordered_set<std::string> terminalSet;
        for (const auto& entry : parseTable)
        {
            for (const auto& terminalRule : entry.second)
            {
                terminalSet.insert(terminalRule.first);
            }
        }

        // Convert the set to a sorted vector
        std::vector<std::string> allTerminals(terminalSet.begin(), terminalSet.end());
        std::sort(allTerminals.begin(), allTerminals.end());

        // Open the file in text mode with UTF-8 support
        std::ofstream parseTableFile("ParseTable.txt", std::ios::out | std::ios::binary);
        if (!parseTableFile.is_open())
        {
            std::cerr << "Error: Unable to open parse_table.txt for writing.\n";
            return;
        }

        // Write UTF-8 BOM (optional, useful for some editors)
        const unsigned char utf8BOM[] = { 0xEF, 0xBB, 0xBF };
        parseTableFile.write(reinterpret_cast<const char*>(utf8BOM), sizeof(utf8BOM));

        // Helper function to print a row separator
        auto printSeparator = [](std::ostream& os, size_t terminalCount) {
            os << "+" << std::string(15, '-') << "+"; // Non-terminal column
            for (size_t i = 0; i < terminalCount; ++i)
            {
                os << std::string(15, '-') << "+";
            }
            os << "\n";
            };

        // Print the top separator
        printSeparator(parseTableFile, allTerminals.size());

        // Print the header row
        parseTableFile << "| " << std::setw(14) << std::left << "Non-Terminal" << "|";
        for (const auto& terminal : allTerminals)
        {
            parseTableFile << " " << std::setw(14) << std::left << terminal << "|";
        }
        parseTableFile << "\n";

        // Print the header separator
        printSeparator(parseTableFile, allTerminals.size());

        // Print each row for the non-terminals
        for (const auto& entry : parseTable)
        {
            const std::string& nonTerminal = entry.first;

            // Print the non-terminal name
            parseTableFile << "| " << std::setw(14) << std::left << nonTerminal << "|";

            // Print the associated terminals
            for (const auto& terminal : allTerminals)
            {
                if (entry.second.count(terminal))
                {
                    parseTableFile << " " << std::setw(14) << std::left << entry.second.at(terminal) << "|";
                }
                else
                {
                    parseTableFile << " " << std::setw(14) << std::left << "-" << "|";
                }
            }
            parseTableFile << "\n";
        }

        // Print the bottom separator
        printSeparator(parseTableFile, allTerminals.size());

        // Close the file
        parseTableFile.close();
    }

    /* <summary>
    This function prints the parse table in a tabular format, displaying the grammar rules associated with each non-terminal and terminal pair. It ensures proper formatting and alignment for readability.

    Logic:
    1. Extract all unique terminals from the parse table and store them in an unordered set.
    2. Convert the set of terminals into a sorted vector for consistent column order.
    3. Define and use helper functions to:
       - Print a row separator line to visually divide the table.
       - Print the header row containing the non-terminal and terminal column names.
    4. For each non-terminal in the parse table:
       - Print its name in the first column.
       - Iterate through the sorted terminal list and:
         - Print the corresponding rule if a mapping exists in the parse table.
         - Otherwise, print a placeholder (`-`) for missing entries.
    5. Print the final row separator to close the table structure.
    6. Output the formatted table to the console (`std::cout`).
    </summary> */
    void printParseTable()
    {
        // Compute all unique terminals and store them in an ordered array
        std::unordered_set<std::string> terminalSet;
        for (const auto& entry : parseTable)
        {
            for (const auto& terminalRule : entry.second)
            {
                terminalSet.insert(terminalRule.first);
            }
        }

        // Convert unordered_set to a sorted vector
        std::vector<std::string> allTerminals(terminalSet.begin(), terminalSet.end());
        std::sort(allTerminals.begin(), allTerminals.end());

        // Helper function to print a row separator
        auto printSeparator = [](std::ostream& os, size_t terminalCount) {
            os << "+" << std::string(15, '-') << "+"; // Non-terminal column
            for (size_t i = 0; i < terminalCount; ++i)
            {
                os << std::string(15, '-') << "+";
            }
            os << "\n";
            };

        printSeparator(std::cout, allTerminals.size());

        auto printHeaders = [&](std::ostream& os) {
            os << "| " << std::setw(14) << std::left << "Non-Terminal" << "|";
            for (const auto& terminal : allTerminals)
            {
                os << " " << std::setw(14) << std::left << terminal << "|";
            }
            os << "\n";
            };

        printHeaders(std::cout);

        printSeparator(std::cout, allTerminals.size());

        // Print rows for non-terminals
        for (const auto& entry : parseTable) // Iterates through non-terminals
        {
            const std::string& nonTerminal = entry.first; // Non-terminal key

            auto printRow = [&](std::ostream& os) {
                os << "| " << std::setw(14) << std::left << nonTerminal << "|"; // Non-terminal column
                for (const auto& terminal : allTerminals) // Iterates through sorted terminal list
                {
                    if (entry.second.count(terminal)) // If terminal exists in inner map
                    {
                        os << " " << std::setw(14) << std::left << entry.second.at(terminal) << "|";
                    }
                    else
                    {
                        os << " " << std::setw(14) << std::left << "-" << "|"; // Placeholder for missing terminal
                    }
                }
                os << "\n";
                };
            printRow(std::cout);     // Prints to console
        }

        // Final separator
        printSeparator(std::cout, allTerminals.size());
    }

    /* <summary>
    This function recursively prints the parse tree in a structured and indented format to an output stream. Each node of the tree corresponds to a grammar symbol, and its children represent the symbols derived from it.

    Logic:
    1. Indent the current output based on the depth of the recursion to visually represent the hierarchy of the tree.
    2. Print the current symbol prefixed by a tree marker (`|====>`).
    3. Check if the current symbol has children in the parse tree.
    4. If children exist, recursively call `printTree` for each child with an incremented depth to further indent the output.
    5. If the symbol has no children, terminate the recursion for that branch.
    </summary> */
    void printTree(const std::string& symbol, const std::unordered_map<std::string, std::vector<std::string>>& tree, std::ostream& output, int depth = 0)
    {
        for (int i = 0; i < depth; ++i)
        {
            output << "    ";
        }
        output << "|====> " << symbol << std::endl;

        if (tree.count(symbol))
        {
            for (const auto& child : tree.at(symbol))
            {
                printTree(child, tree, output, depth + 1);
            }
        }
    }

    /* <summary>
    This function performs the syntax analysis of a given input string using a parsing table and a starting symbol. It implements a stack-based parsing algorithm to match tokens or expand non-terminals based on grammar rules. The function logs parsing actions, errors, and the generated parse tree.

    Logic:
    1. Initialize a parsing stack with the end marker (`$`) and the start symbol.
    2. Tokenize the input string and add an end-of-input marker (`$`) to the token list.
    3. Set up data structures for tracking parsing actions and building the parse tree.
    4. Log and display the headers for the stack, input, and actions.
    5. While the parsing stack is not empty and there are tokens left:
       - Extract the top of the stack and the current input token.
       - Log the current stack and input states.
       - If the top of the stack matches the current token, record a "Match" action and advance the input token.
       - If the top is a terminal and doesn't match the token, record an error action and advance the input token.
       - If the top is a non-terminal with a rule in the parse table for the current token:
         - Expand the non-terminal using the rule, log the action, and push the production onto the stack in reverse order.
         - Update the parse tree to reflect the production.
       - If no rule exists, enter panic mode for error recovery by skipping tokens until a valid follow set token for the non-terminal is found, then pop the stack.
    6. After processing, check if the parsing stack is empty and all tokens are consumed to determine if parsing was successful.
    7. Log and display the final parse tree structure using the `printTree` function.
    8. Record all errors, parsing steps, and the parse tree in their respective output files.
    </summary> */
    void parseInput(const std::string& input, const std::string& startSymbol)
    {
        bool parseTokenPrinted = false;
        std::stack<std::string> parsingStack;
        parsingStack.push("$"); // End marker
        parsingStack.push(startSymbol); // Start symbol

        std::unordered_map<int, std::string> tokens; // To maintain ordered tokens
        std::istringstream inputStream(input);

        int tokenIndex = 0;
        std::string token;
        while (inputStream >> token)
        {
            tokens[tokenIndex++] = token;
        }
        tokens[tokenIndex] = "$"; // End-of-input marker

        tokenIndex = 0;
        bool success = true;

        std::unordered_map<int, std::string> actions; // Indexed actions for order
        std::unordered_map<std::string, std::vector<std::string>> parseTree; // Tree structure

        parsingFile << std::left << std::setw(20) << "Stack" << std::setw(20) << "Input" << "Action" << std::endl;
        std::cout << std::left << std::setw(20) << "Stack" << std::setw(20) << "Input" << "Action" << std::endl;

        while (!parsingStack.empty() && tokens.count(tokenIndex))
        {
            std::string top = parsingStack.top();
            std::string currentToken = tokens[tokenIndex];

            // Display stack and input
            std::stringstream stackContent, inputContent;
            std::stack<std::string> tempStack = parsingStack;
            while (!tempStack.empty())
            {
                stackContent << tempStack.top() << " ";
                tempStack.pop();
            }
            for (auto it = tokens.find(tokenIndex); it != tokens.end(); ++it)
            {
                inputContent << it->second << " ";
            }
            parsingFile << std::setw(20) << stackContent.str() << std::setw(20) << inputContent.str();
            if (!parseTokenPrinted)
            {
                parsingTree << "Token: " << std::setw(20) << inputContent.str();
                parseTokenPrinted = true;
            }
            std::cout << std::setw(20) << stackContent.str() << std::setw(20) << inputContent.str();

            if (top == currentToken) // Match
            {
                std::string action = "Match: " + currentToken;
                actions[tokenIndex] = action;
                parsingFile << action << std::endl;
                std::cout << action << std::endl;
                parsingFile << action << std::endl;
                parsingStack.pop();
                ++tokenIndex;
            }
            else if (isTerminal(top) || top == "$") // Error: Terminal mismatch
            {
                std::string action = "Error: Unexpected token '" + currentToken + "'. Expected: '" + top + "'.";
                actions[tokenIndex] = action;
                parsingFile << action << std::endl;
                errorFile << action << std::endl;
                std::cout << action << std::endl;
                success = false;
                ++tokenIndex;
            }
            else if (parseTable[top].count(currentToken)) // Expand using a production
            {
                std::string production = parseTable[top][currentToken];
                std::string action = "Expand: " + top + " -> " + production;
                actions[tokenIndex] = action;
                parsingFile << action << std::endl;
                std::cout << action << std::endl;
                parsingStack.pop();

                // Update the tree structure
                std::vector<std::string> productionSymbols;
                if (production != EPSILON)
                {
                    std::istringstream prodStream(production);
                    std::string symbol;
                    while (prodStream >> symbol)
                    {
                        productionSymbols.push_back(symbol);
                    }
                    parseTree[top] = productionSymbols; // Record production in the tree
                }

                // Push production onto stack in reverse order
                std::stack<std::string> tempStack;
                for (const auto& symbol : productionSymbols)
                {
                    tempStack.push(symbol);
                }
                while (!tempStack.empty())
                {
                    parsingStack.push(tempStack.top());
                    tempStack.pop();
                }
            }
            else // Panic Mode: Error recovery
            {
                std::string action = "Error: No rule for '" + top + "' with token '" + currentToken + "'. Entering Panic Mode.";
                actions[tokenIndex] = action;
                parsingFile << action << std::endl;               
                errorFile << action << std::endl;
                std::cout << action << std::endl;
                success = false;

                while (tokens.count(tokenIndex) && followSets[top].count(currentToken) == 0)
                {
                    ++tokenIndex;
                    if (tokens.count(tokenIndex))
                    {
                        currentToken = tokens[tokenIndex];
                    }
                }
                parsingStack.pop();
            }
        }

        // Check if parsing completed successfully
        if (success && parsingStack.empty() && !tokens.count(tokenIndex))
        {
            parsingFile << "Input successfully parsed." << std::endl;
            std::cout << "Input successfully parsed." << std::endl;
        }
        else
        {
            parsingFile << "Parsing failed." << std::endl;
            std::cout << "Parsing failed." << std::endl;
            errorFile << "Parsing failed" << std::endl;
        }

        // Output the parse tree
        parsingTree << "\nParse Tree:\n";
        std::cout << "\nParse Tree:\n";
        printTree(startSymbol, parseTree, parsingTree);
        printTree(startSymbol, parseTree, std::cout);
    }

    /* <summary>
    This function handles the parsing process of input tokens stored in a file. It reads the tokens line by line, skips the first two lines, and extracts the token values to perform syntax analysis. The results of the parsing process are recorded in output files, including errors, parsing steps, and the parse tree.

    Logic:
    1. Open the "error.txt" file in append mode for logging errors. If the file cannot be opened, print an error message to the console and terminate the program.
    2. Open the "ParsingProcess.txt" file to log the detailed parsing process. If it fails, log the error in the "error.txt" file and terminate.
    3. Open the "ParseTree.txt" file to record the parse tree. Handle any opening failure similarly.
    4. Open the input file specified by `fileName`. If it cannot be opened, log the error and terminate the program.
    5. Read the file line by line using a loop. Keep track of the line numbers.
    6. Skip the first two lines as they do not contain token information.
    7. Extract the first value (token) from each subsequent line. If the line is empty or improperly formatted, log a warning in the "error.txt" file.
    8. For valid tokens, print and log the parsing process to the console and "ParsingProcess.txt", then call the `parseInput` function to perform parsing using the given token and the starting symbol.
    9. Close all open files upon completing the parsing process.
    </summary> */
    void parseFromFile(const std::string& fileName, const std::string& startSymbol)
    {

        errorFile.open("error.txt", std::ios::app);
        if (!errorFile)
        {
            std::cerr << "Error: Unable to open error File." << std::endl;
            exit(1);
        }
        errorFile << "\n\n Synthethic Errors from parsing \n\n";
        parsingFile.open("ParsingProcess.txt");
        if (!parsingFile)
        {
            std::cerr << "Error: Unable to open ParsingProcess File." << std::endl;
            errorFile << "Error: Unable to open ParsingProcess File." << std::endl;

            exit(1);
        }
        parsingTree.open("ParseTree.txt");
        if (!parsingTree)
        {
            std::cerr << "Error: Unable to open ParseTree File." << std::endl;
            errorFile << "Error: Unable to open ParseTree File." << std::endl;
            exit(1);
        }
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open file " << fileName << std::endl;
            errorFile << "Error: Unable to open file " << fileName << std::endl;
            exit(1);
        }
        std::string line;
        int lineNumber = 0;

        while (getline(file, line))
        {
            lineNumber++;

            // Skip the first two lines
            if (lineNumber <= 2)
            {
                continue;
            }

            // Extract the first value from the line
            std::istringstream lineStream(line);
            std::string firstValue;
            if (lineStream >> firstValue)
            {
                std::cout << "Parsing line " << lineNumber << ": " << firstValue << std::endl;
                parsingFile << "Parsing line " << lineNumber << ": " << firstValue << std::endl;
                parseInput(firstValue, startSymbol);
                parsingFile << std::endl;
                std::cout << std::endl;
            }
            else
            {
                std::cerr << "Warning: Line " << lineNumber << " is empty or improperly formatted." << std::endl;
                errorFile << "Warning: Line " << lineNumber << " is empty or improperly formatted." << std::endl;
            }
        }
        parsingFile.close();
        parsingTree.close();
        errorFile.close();
        file.close();
    }
};

#endif // SYNTHETIC_H