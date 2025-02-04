#ifndef LEXICAL_H
#define LEXICAL_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <string>
#include <unordered_set>
#include <unordered_map>

class Lexical
{
private:
    // Files to be created
    std::ofstream tokenFile;
    std::ofstream symbolTableFile;
    std::ofstream errorFile;

    // Count
    int tokenNo = 0;
    int nKeywords = 0;
    int nIdentifiers = 0;
    int nNumbers = 0;
    int nPunctuations = 0;
    int nOperators = 0;
    int nInvalid = 0;

    // Common column widths for formatting
    const int colWidthToken = 20;
    const int colWidthType = 20;
    const int colWidthLine = 10;
    const int colWidthTokenNo = 10;

    // All valid keywords
    std::unordered_set<std::string> keywords =
    {
        "loop", "agar", "magar", "asm", "else", "new", "this", "auto",
        "enum", "operator", "throw", "bool", "explicit", "private", "true",
        "break", "export", "protected", "try", "case", "extern", "public",
        "typedef", "catch", "false", "register", "typeid", "char", "float",
        "typename", "class", "for", "return", "union", "const", "friend",
        "short", "unsigned", "goto", "signed", "using", "continue", "if",
        "sizeof", "virtual", "default", "inline", "static", "void", "delete",
        "int", "volatile", "do", "long", "struct", "double", "mutable",
        "switch", "while", "namespace"
    };

    // Transition Tables

    /* <summary>
    This is a state transition table for the finite state machine (FSM) that validates identifier tokens, typically used for variable names, function names, or other user-defined symbols in programming languages.

    Logic:
    1. The table consists of 5 rows and 4 columns. Each row represents a state in the FSM, and each column corresponds to an input character:
       - `L`: A letter (uppercase or lowercase).
       - `D`: A digit (0-9).
       - `_`: An underscore character (`_`).
       - `O`: Other characters (anything not a letter, digit, or underscore).

    2. The rows represent the following states:
       - S0: The starting state where the FSM begins processing the identifier.
       - S1: After encountering an underscore (`_`).
       - S2: After encountering a letter (`L`).
       - S3: The accepting state, indicating the identifier is valid.
       - S4: An invalid state for when invalid transitions occur.

    3. The transitions are defined as follows:
       - From S0 (the start state), the FSM can transition to:
         - S2 if a letter (`L`) is encountered.
         - S1 if an underscore (`_`) is encountered.
         - Any other character is rejected (indicated by `-1`).
       - From S1 (after encountering an underscore), the FSM can continue to:
         - S3 if a letter (`L`), digit (`D`), or underscore (`_`) is encountered.
         - Any other character leads to rejection.
       - From S2 (after encountering a letter), the FSM can transition to:
         - S2 if another letter (`L`) or digit (`D`) is encountered.
         - S3 if an underscore (`_`) is encountered (valid identifier character).
         - Any other character leads to rejection.
       - From S3 (the accepting state), the FSM continues with:
         - S3 if a letter (`L`), digit (`D`), or underscore (`_`) is encountered.
         - Any other character leads to rejection.
       - Any invalid character encountered in any state leads to the invalid state (S4) where further transitions are impossible (all are `-1`).
    4. The FSM accepts the token as a valid identifier if it reaches state S2 or S3, indicating that the identifier follows the rules: it starts with a letter or underscore, and may be followed by letters, digits, or underscores.
    5. Invalid transitions or characters lead to rejection.
    </summary>*/
    const int identifierTable[5][4] =
    {
        // Columns:      L,   D,  _,  O
                        {2,  -1,  1, -1}, // S0: Start -> (L: S2, _: S1, Others: Reject)
                        {3,   3,  3, -1}, // S1: Accept '_' -> Continue with (L, D, _) or Reject
                        {2,   2,  3, -1}, // S2: After L -> Continue with (L, D, _), Accept '_'
                        {3,   3,  3, -1}, // S3: Accepting state -> Continue with (L, D, _) or Reject
                        {-1, -1, -1, -1}  // Invalid state
    };

    /* <summary>
    This is a state transition table for the finite state machine (FSM) that validates number tokens, including integers and floating-point numbers, with support for scientific notation.

    Logic:
    1. The table consists of 8 rows and 5 columns. Each row represents a state in the FSM, and each column corresponds to an input character:
       - `D`: A digit (0-9).
       - `S`: A sign (`+` or `-`).
       - `.`: A decimal point.
       - `E`: The character `e` or `E` for scientific notation.
       - `O`: Other characters (anything not a digit, sign, decimal, or `e`/`E`).

    2. The rows represent the following states:
       - S0: Start state.
       - S1: After a sign (`+` or `-`).
       - S2: After a digit.
       - S3: After a decimal point.
       - S4: After a digit following a decimal point.
       - S5: After encountering `e` or `E` (for scientific notation).
       - S6: After a sign following `e`/`E`.
       - S7: Accepting state, indicating a valid number token has been recognized.

    3. The transitions are defined as follows:
       - From S0, the FSM can transition to:
         - S2 if a digit (`D`) is encountered.
         - S1 if a sign (`S`) is encountered.
         - S3 if a decimal point (`.`) is encountered.
         - Any other character leads to rejection (indicated by `-1`).
       - In S1 (after a sign), the FSM transitions to:
         - S2 if a digit (`D`) is encountered.
         - S3 if a decimal point (`.`) is encountered.
         - Any other character is rejected.
       - In S2 (after a digit), the FSM can transition to:
         - S2 if another digit is encountered.
         - S4 if a decimal point (`.`) is encountered (starting a floating point).
         - S5 if `e` or `E` is encountered (starting scientific notation).
         - Any other character is accepted as a valid number.
       - In S3 (after a decimal point), the FSM can transition to:
         - S4 if a digit (`D`) is encountered.
         - Any other character leads to rejection.
       - In S4 (after a digit following a decimal point), the FSM can transition to:
         - S4 if another digit (`D`) is encountered.
         - S5 if `e` or `E` is encountered (starting scientific notation).
         - Any other character is accepted as a valid number.
       - In S5 (after encountering `e`/`E`), the FSM can transition to:
         - S7 if a digit (`D`) is encountered.
         - S6 if a sign (`S`) is encountered (for exponent sign).
         - Any other character leads to rejection.
       - In S6 (after encountering a sign in scientific notation), the FSM transitions to:
         - S7 if a digit (`D`) is encountered.
         - Any other character leads to rejection.
       - In S7 (the accepting state), the FSM can transition to:
         - S7 if another digit (`D`) is encountered.
         - Any other character is accepted as part of the number.
    4. The FSM rejects any invalid transitions, marked with `-1`, and only accepts valid number formats when the final state is one of the accepting states (like S2, S4, or S7).
    </summary>*/
    const int numberTable[8][5] =
    {
        // Columns:      D,  S,  .,  E,  O
                        {2,  1,  3, -1, -1}, // S0: Start -> (D: S2, Sign: S1, .: S3, Others: Reject)
                        {2, -1,  3, -1, -1}, // S1: After Sign -> (D: S2, .: S3, Others: Reject)
                        {2, -1,  4,  5, -1}, // S2: After D -> (D: S2, .: S4, E: S5, Others: Accept)
                        {4, -1, -1, -1, -1}, // S3: After . -> (D: S4, Others: Reject)
                        {4, -1, -1,  5, -1}, // S4: After D following . -> (D: S4, E: S5, Others: Accept)
                        {7,  6, -1, -1, -1}, // S5: After E -> (D: S7, Sign: S6, Others: Reject)
                        {7, -1, -1, -1, -1}, // S6: After Sign in Exponent -> (D: S7, Others: Reject)
                        {7, -1, -1, -1, -1}  // S7: Accepting state -> (D: S7, Others: Accept)
    };

    /* <summary>
    This is a state transition table for the finite state machine (FSM) that validates punctuation tokens.

    Logic:
    1. The table has 2 rows and 7 columns. Each row represents a state in the FSM, and each column corresponds to a specific punctuation character or "Other" characters.
    2. The rows represent the following states:
       - S0: The start state, where the FSM begins the transition based on the input punctuation character.
       - S1: An accepting state, which indicates that a valid punctuation token has been recognized. Once the FSM reaches this state, no further input is allowed (all subsequent inputs are rejected).
    3. The columns represent specific punctuation characters:
       - `[` (column 0)
       - `{` (column 1)
       - `<` (column 2)
       - `>` (column 3)
       - `}` (column 4)
       - `]` (column 5)
       - "Other" characters (column 6)
    4. The transitions are defined as follows:
       - From S0, if any of the valid punctuation characters (`[`, `{`, `<`, `>`, `}`, `]`) is encountered, the FSM transitions to state S1, indicating a valid punctuation token.
       - Once in S1, the FSM rejects any further inputs, as indicated by the `-2` entries in the second row (S1). This means that no further characters are valid after a valid punctuation token is recognized.
    5. The table effectively handles the detection of individual punctuation characters, treating them as valid and rejecting any additional characters after the first valid punctuation.
    </summary>*/
    const int punctuationTable[2][7] =
    {
        // Columns:      [,  {,  <,  >,  },  ], Other
                        { 1,  1,  1,  1,  1,  1, -1}, // S0: Start -> Transition to states S1-S6 for respective punctuation
                        {-2, -2, -2, -2, -2, -2, -2}, // S1: Accepting state for Valid, Rejects all inputs.
    };

    /* <summary>
    This is a state transition table for the finite state machine (FSM) that validates operator tokens in a programming language.

    Logic:
    1. The table is structured with 14 rows and 13 columns, where each row represents a state and each column corresponds to a specific input character (operators or others).
    2. The rows represent states in the FSM:
       - S0: Start state, transitions to specific operator states based on the input character.
       - S1-S12: States for each operator, where further transitions depend on the input character.
    3. The columns represent specific operator characters (`!`, `<`, `>`, `=`, `:`, `*`, `+`, `/`, `-`, `&`, `|`, `%`, and "Other" characters).
    4. The transitions between states are defined as follows:
       - For each character, the table specifies the next state or an invalid state (`-1`).
       - If a valid transition is found, it moves to a specific state, otherwise, it returns `-1` (indicating an invalid input).
    5. Example transitions:
       - From S0, if the character is `!`, the FSM transitions to S1.
       - In S1, if the next character is `=`, it transitions to state 13 (`!=` operator).
       - In S3, if the character is `=`, it transitions to state 13, handling operators like `=>`.
       - The last row (S13) represents final acceptance of operator tokens like `!=`, `< >`, and `=>`, rejecting all further inputs.
    6. The table defines how different operator characters and combinations (e.g., `+`, `++`, `-`, `--`, `&&`, `||`, etc.) are processed by the FSM.
    </summary>*/
    const int operatorTable[14][13] =
    {
        // Columns:    !,  <,  >,  =,  :,  *,  +,  /,  -,  &,  |,  %, Other
                    {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, -1 }, // S0: Start -> Transition to specific operator states
                    { -1, -1, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // S1: `!` -> Accept `!=` (transition to S13), reject others
                    { -1, 13, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // S2: `<` -> Accept `<` or `<>` (transition to S14), reject others
                    { -1, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // S3: `>` -> Accept `>` or `<`, reject others
                    { -1, 13, 13, 13,  4, -1, 13, -1, -1, -1, -1, -1, -1 }, // S4: `=` -> Accept `=`, `==`, `=+`, `=>`, or `=<`
                    { -1, -1, -1, 13, 13, -1, -1, -1, -1, -1, -1, -1, -1 }, // S5: `:` -> Accept `:`, `=:=`, or `::`
                    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // S6: `*` -> Accept `*`
                    { -1, -1, -1, -1, -1, -1, 13, -1, -1, -1, -1, -1, -1 }, // S7: `+` -> Accept `+` or `++`
                    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // S8: `/` -> Accept `/`
                    { -1, -1, -1, -1, -1, -1, -1, -1, 13, -1, -1, -1, -1 }, // S9: `-` -> Accept `-` or `--`
                    { -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1, -1, -1 }, // S10: `&` -> Accept `&` or `&&`
                    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1, -1 }, // S11: `|` -> Accept `|` or `||`
                    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1 }, // S12: `%` -> Accept `%`
                    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }  // S13: `!=` , '<>', '=>' -> Reject all further inputs
    };
public:
    // Mapping Functions
    int getIdentifierCol(char c);
    int getNumberCol(char c);
    int getPunctuationCol(char c);
    int getOperatorCol(char c);
    // Utility Functions
    void seperateKeywordToken(const std::string& token, std::string& tokenPart, std::string& lastChar);
    void seperateIdentifierToken(const std::string& token, std::string& tokenPart, std::string& lastChar);
    void seperateNumToken(const std::string& token, std::string& tokenPart, std::string& lastChar);
    void seperatePunctuationToken(const std::string& token, std::string& tokenPart, std::string& lastChar);
    void separateOperatorToken(const std::string& token, std::string& tokenPart, std::string& lastChar);

    // FSM Function
    template <size_t NumColumns>
    int runFSM(const std::string& token, int startState, const int table[][NumColumns], 
        int numColumns, int (Lexical::*getCol)(char), const std::unordered_map<int, bool>& validStates);

    int identifierFSM(const std::string& token);
    int numberFSM(const std::string& token);
    int punctuationFSM(const std::string& token);
    int operatorFSM(const std::string& token);
    bool isKeyword(const std::string& token);

    // Token Processing
    int PerformLexical(const std::string& Input, const std::string& Token, const std::string& Symbol, const std::string& Error);
    void processToken(const std::string& token, int lineNum);
};

// |-------------------------------------------------------------------------------------------------------------|
// |                                         Column-mapping functions                                            |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
This function returns the column index for a given character in an identifier token, used in the finite state machine (FSM) for identifier validation.

Logic:
1. The function takes a single character `c` as input and checks its type to determine the corresponding column index.
2. If the character is an alphabetic character (`a-z`, `A-Z`), the function returns `0`, indicating the column for letters in the identifier state transition table.
3. If the character is a digit (`0-9`), the function returns `1`, indicating the column for digits in the identifier token.
4. If the character is an underscore (`_`), the function returns `2`, representing the column for the underscore in identifiers.
5. If the character doesn't match any of the above, the function returns `3`, indicating an invalid character for an identifier token.
</summary>*/
int Lexical::getIdentifierCol(char c)
{
    if (isalpha(c))
        return 0;
    if (isdigit(c))
        return 1;
    if (c == '_')
        return 2;
    return 3;
}

/* <summary>
This function returns the column index for a given character in a number token, used in the finite state machine (FSM) for number validation.

Logic:
1. The function takes a single character `c` as input and checks its type to determine the corresponding column index.
2. If the character is a digit (`0-9`), the function returns `0`, indicating the column for digits in the number state transition table.
3. If the character is either a plus (`+`) or minus (`-`), the function returns `1`, indicating the column for sign characters.
4. If the character is a decimal point (`.`), the function returns `2`, indicating the column for the decimal point in the number token.
5. If the character is either `e` or `E` (indicating scientific notation), the function returns `3`, representing the column for scientific notation.
6. If the character doesn't match any of the above, the function returns `4`, indicating an invalid or unrecognized character in a number token.
</summary>*/
int Lexical::getNumberCol(char c)
{
    if (isdigit(c))
        return 0;
    if (c == '+' || c == '-')
        return 1;
    if (c == '.')
        return 2;
    if (c == 'e' || c == 'E')
        return 3;
    return 4;
}

/* <summary>
This function returns the column index for a given punctuation character, used in the finite state machine (FSM) for punctuation validation.

Logic:
1. The function takes a single character `c` as input and checks it against a set of predefined punctuation characters.
2. Depending on the character, the function returns an integer value representing the corresponding column index in the punctuation state transition table.
   - Each case corresponds to a specific punctuation character (e.g., `[`, `{`, `<`, `>`, `}`, `]`).
   - Each punctuation character is assigned a unique column index ranging from 0 to 5.
3. If the character does not match any of the predefined punctuation characters, the function returns `6`, indicating that the character is invalid for punctuation.
</summary>*/
int Lexical::getPunctuationCol(char c)
{
    switch (c)
    {
    case '[':
        return 0;
    case '{':
        return 1;
    case '<':
        return 2;
    case '>':
        return 3;
    case '}':
        return 4;
    case ']':
        return 5;
    default:
        return 6;
    }
}

/* <summary>
This function returns the column index for a given operator character, used in the finite state machine (FSM) for operator validation.

Logic:
1. The function takes a single character `c` as input and checks it against a set of predefined operator characters.
2. Depending on the character, the function returns an integer value representing the corresponding column index in the operator state transition table.
   - Each case corresponds to a specific operator (e.g., `!`, `<`, `>`, `=`, `+`, etc.).
   - Each operator is assigned a unique column index ranging from 0 to 11.
3. If the character does not match any of the predefined operators, the function returns `12`, indicating that the character is invalid for an operator.
</summary>*/
int Lexical::getOperatorCol(char c)
{
    switch (c)
    {
    case '!':
        return 0;
    case '<':
        return 1;
    case '>':
        return 2;
    case '=':
        return 3;
    case ':':
        return 4;
    case '*':
        return 5;
    case '+':
        return 6;
    case '/':
        return 7;
    case '-':
        return 8;
    case '&':
        return 9;
    case '|':
        return 10;
    case '%':
        return 11;
    default:
        return 12; // Invalid input
    }
}



// |-------------------------------------------------------------------------------------------------------------|
// |                                         Unified FSM driver function                                         |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
    This function runs a finite state machine (FSM) on a given token to validate its structure according to a state transition table.

    Logic:
    1. The function takes a template parameter `NumColumns`, which specifies the number of columns in the state transition table.
    2. Initialize `state` with the `startState` value, which represents the starting point of the FSM.
    3. Iterate through each character `c` in the token:
       - Use the `getCol` function to determine the column index for the current character.
       - If the column index is invalid (i.e., less than 0 or greater than or equal to `numColumns`), return `-1`, indicating an invalid token.
       - Use the current state and column to find the next state from the `table`.
       - If the state is `-1`, return `-1`, indicating an invalid state.
    4. After processing all characters, check if the final state is a valid state by looking it up in the `validStates` map.
       - If the state is valid, return the final state.
       - If the state is not valid, return `-1`, indicating an invalid token.
    </summary>*/
template <size_t NumColumns>
int Lexical::runFSM(const std::string& token, int startState, const int table[][NumColumns], int numColumns, int (Lexical::*getCol)(char), const std::unordered_map<int, bool>& validStates)
{
    int state = startState;
    for (char c : token)
    {
        int col = (this->*getCol)(c);
        if (col >= numColumns || col < 0)
            return -1; // Invalid column
        state = table[state][col];
        if (state == -1)
            return -1; // Invalid state
    }
    return validStates.count(state) ? state : -1;
}

// |-------------------------------------------------------------------------------------------------------------|
// |                                         Wrapper FSM Functions                                               |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
This function checks if a given token is a valid identifier by running a finite state machine (FSM).

Logic:
1. Define a map `validStates` that contains the valid final state for identifier tokens. The key `3` represents the valid final state, and the value `true` indicates its validity.
2. The function calls `runFSM` to validate the identifier token:
   - Pass the token to the `runFSM` function, starting from the initial state (0).
   - The function uses the `identifierTable` to define the state transitions, with `4` being the final state.
   - The `getIdentifierCol` function is used to retrieve the appropriate column index for state transitions.
   - The `validStates` map is passed to check if the final state is one of the valid states for an identifier.
3. The return value of `runFSM` is the result of the FSM validation, which will indicate whether the token is a valid identifier or not.
</summary>*/
int Lexical::identifierFSM(const std::string& token)
{
    std::unordered_map<int, bool> validStates = { {3, true} };
    return runFSM(token, 0, identifierTable, 4, &Lexical::getIdentifierCol, validStates);
}

/* <summary>
This function checks if a given token is a valid number by running a finite state machine (FSM).

Logic:
1. Define a map `validStates` that contains the valid final states for number tokens. The keys represent valid states (`2`, `4`, `7`), and the values are all set to `true` to indicate validity.
2. The function calls `runFSM` to validate the number token:
   - Pass the token to the `runFSM` function, starting from the initial state (0).
   - The function uses the `numberTable` to define the state transitions, with `6` being the final state.
   - The `getNumberCol` function is used to retrieve the appropriate column index for state transitions.
   - The `validStates` map is passed to check if the final state is one of the valid states for a number.
3. The return value of `runFSM` is the result of the FSM validation, which will indicate whether the token is a valid number or not.
</summary>*/
int Lexical::numberFSM(const std::string& token)
{
    std::unordered_map<int, bool> validStates = { {2, true}, {4, true}, {7, true} };
    return runFSM(token, 0, numberTable, 6, &Lexical::getNumberCol, validStates);
}

/* <summary>
This function checks if a given token is a valid punctuation character by running a finite state machine (FSM).

Logic:
1. Define a map `validStates` that contains the valid final state for punctuation tokens. The key `1` represents the valid final state, and the value `true` indicates its validity.
2. The function calls `runFSM` to validate the punctuation token:
   - Pass the token to the `runFSM` function, starting from the initial state (0).
   - The function uses the `punctuationTable` to define the state transitions, with `7` being the final state.
   - The `getPunctuationCol` function is used to retrieve the appropriate column index for state transitions.
   - The `validStates` map is passed to check if the final state is one of the valid states for punctuation.
3. The return value of `runFSM` is the result of the FSM validation, which will indicate whether the token is a valid punctuation character or not.
</summary>*/
int Lexical::punctuationFSM(const std::string& token)
{
    std::unordered_map<int, bool> validStates = { {1, true} };
    return runFSM(token, 0, punctuationTable, 7, &Lexical::getPunctuationCol, validStates);
}

/* <summary>
This function checks if a given token is a valid operator by running a finite state machine (FSM).

Logic:
1. Define a map `validStates` that contains the valid final states for operator tokens. These states are represented by integer keys, and the values are all set to `true` to indicate validity.
2. The function calls `runFSM` to validate the operator token:
   - Pass the token to the `runFSM` function, starting from the initial state (0).
   - The function uses the `operatorTable` to define the state transitions, with `13` being the final state.
   - The `getOperatorCol` function is used to retrieve the appropriate column index for state transitions.
   - The `validStates` map is passed to check if the final state is one of the valid states for an operator.
3. The return value of `runFSM` is the result of the FSM validation, which will indicate whether the token is a valid operator or not.
</summary>*/
int Lexical::operatorFSM(const std::string& token)
{
    // Define valid final states for operators
    std::unordered_map<int, bool> validStates = { {13, true}, {6, true},  {7, true},  {8, true},  {9, true}, {12, true}, {5, true} };

    // Call runFSM to validate the operator token
    return runFSM(token, 0, operatorTable, 13, &Lexical::getOperatorCol, validStates);
}



// |-------------------------------------------------------------------------------------------------------------|
// |                                             Keyword Check                                                   |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
This function checks if a given token is a valid keyword.

Logic:
1. The function checks if the token exists in the `keywords` container (likely a set or map).
2. If the token is found in the `keywords` container, the function returns `true`, indicating that the token is a valid keyword.
3. If the token is not found in the `keywords` container, the function returns `false`, indicating that the token is not a keyword.
</summary>*/
bool Lexical::isKeyword(const std::string& token)
{
    return keywords.find(token) != keywords.end();
}



// |-------------------------------------------------------------------------------------------------------------|
// |                                             Utility Functions                                               |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
This function separates a string token into two parts:
- `tokenPart`: The portion of the token that represents the keyword (excluding the last character if it is non-alphabetic).
- `lastChar`: The last character of the token, if it is non-alphabetic.

Logic:
1. Initialize `tokenPart` with the entire token and `lastChar` as an empty string.
2. Check if the last character of the token is a non-alphabetic character:
   - If it is non-alphabetic, separate the token:
     - Extract the substring excluding the last character and assign it to `tokenPart`.
     - Assign the last character of the token to `lastChar`.
   - If the last character is alphabetic, no changes are made to `tokenPart` or `lastChar`.
</summary>*/
void Lexical::seperateKeywordToken(const std::string& token, std::string& tokenPart, std::string& lastChar)
{
    tokenPart = token;
    lastChar = "";

    // Check if the last character is non-alphabetic
    if (!isalpha(token.back()))
    {
        tokenPart = token.substr(0, token.length() - 1); // Extract all except the last character
        lastChar = token.back();                          // Store the last character
    }
}

/* <summary>
This function separates a string token into two parts:
- `tokenPart`: The portion of the token that represents a valid identifier, which can include letters, digits, or an underscore.
- `lastChar`: Any trailing characters that do not belong to a valid identifier.

Logic:
1. Initialize `tokenPart` with the entire token and `lastChar` as an empty string.
2. Traverse the token from the end:
   - Start at the last character of the token and move backwards.
   - Check each character to determine if it is part of a valid identifier. Valid identifier characters include:
     - Alphabetic characters (`a-z`, `A-Z`),
     - Digits (`0-9`),
     - The underscore (`_`).
   - Stop the traversal once a character that does not belong to a valid identifier is found.
3. Extract the valid identifier portion from the start of the token to the last valid identifier character and assign it to `tokenPart`.
4. Extract the trailing characters (those that don't belong to the identifier) and assign them to `lastChar`.
</summary>*/
void Lexical::seperateIdentifierToken(const std::string& token, std::string& tokenPart, std::string& lastChar)
{
    tokenPart = token;
    lastChar = "";

    // Check for trailing characters that are not part of a valid identifier
    size_t pos = token.length();
    while (pos > 0)
    {
        char c = token[pos - 1];
        // Stop if the character is a digit, an alphabet, or an underscore
        if (isalnum(c) || c == '_')
        {
            break;
        }
        pos--;
    }

    // Extract the valid identifier part and trailing characters
    tokenPart = token.substr(0, pos);       // Extract the valid identifier part
    lastChar = token.substr(pos);           // Extract the trailing characters
}

/* <summary>
This function separates a numeric token into two parts:
- `tokenPart`: The portion of the token that represents a valid number, including digits, a decimal point, or scientific notation (e/E).
- `lastChar`: Any trailing characters after the valid number portion.

Logic:
1. Initialize `tokenPart` with the entire token and `lastChar` as an empty string.
2. Traverse the token from the end to identify the last valid character that can be part of a number:
   - Stop the traversal when encountering a valid numeric character: a digit (`0-9`), a decimal point (`.`), or scientific notation markers (`e/E`).
   - Decrease the position index (`pos`) for trailing characters that are not valid number parts.
3. Extract the numeric portion into `tokenPart` using the substring from the start to the last valid position.
4. Extract the trailing non-numeric characters into `lastChar` from the last valid position to the end of the token.
</summary>*/
void Lexical::seperateNumToken(const std::string& token, std::string& tokenPart, std::string& lastChar)
{
    tokenPart = token;
    lastChar = "";

    // Check for trailing characters that are not part of a valid number
    size_t pos = token.length();
    while (pos > 0)
    {
        char c = token[pos - 1];
        // Stop if the character is a digit, decimal point, or part of scientific notation (E/e)
        if (isdigit(c) || c == '.' || c == 'e' || c == 'E')
        {
            break;
        }
        pos--;
    }

    // Extract the number part and trailing characters
    tokenPart = token.substr(0, pos);       // Extract the valid number part
    lastChar = token.substr(pos);           // Extract the trailing characters
}

/* <summary>
This function processes a string token to separate punctuation characters from the rest of the token:
- `tokenPart`: The part of the token that comes before encountering a valid punctuation character.
- `lastChar`: The part of the token containing the first punctuation character (and any remaining characters).

Logic:
1. Initialize `tokenPart` and `lastChar` to empty strings.
2. Iterate through the token, character by character:
   - If a valid punctuation character (determined by `getPunctuationCol`) is found:
     a. If `tokenPart` already contains non-punctuation characters, store the remaining string (from the punctuation onward) in `lastChar` and stop.
     b. Otherwise, set `tokenPart` to the punctuation character, and `lastChar` to the rest of the string after the punctuation, and stop.
   - If the character is not punctuation, append it to `tokenPart`.
3. If no punctuation character is encountered, `lastChar` remains empty.
</summary>*/
void Lexical::seperatePunctuationToken(const std::string& token, std::string& tokenPart, std::string& lastChar)
{
    tokenPart = "";
    lastChar = "";
    size_t pos = 0;

    // Iterate through the token to separate punctuation
    while (pos < token.length())
    {
        char c = token[pos];
        if (getPunctuationCol(c) >= 0 && getPunctuationCol(c) <= 5)
        {
            // If valid punctuation is found, terminate tokenPart
            if (!tokenPart.empty())
            {
                // Store the current tokenPart in lastChar and stop
                lastChar = token.substr(pos);  // Remaining characters (punctuation and beyond)
                return;
            }

            // Add the punctuation as the tokenPart and stop
            tokenPart = std::string(1, c);
            lastChar = token.substr(pos + 1);  // Remaining characters after the punctuation
            return;
        }
        else
        {
            // Add the character to tokenPart
            tokenPart += c;
        }
        pos++;
    }

    // If no punctuation is found, lastChar remains empty
}

/* <summary>
This function processes a string token to separate it into two parts:
- `tokenPart`: The central portion of the token that excludes any valid characters (letters, digits, or underscores) at the beginning or end of the token.
- `lastChar`: The valid characters found at the beginning and end of the token, concatenated together.

Logic:
1. The function first clears `tokenPart` and `lastChar`.
2. It scans the token from the start to identify valid characters (alphanumeric or underscores).
   - These characters, if found, set the `hasStartValid` flag to true and increment the starting index.
3. Similarly, it scans the token from the end for valid characters, setting the `hasEndValid` flag to true if any are found.
4. If valid characters are found at both the start and end, the entire token is treated as `tokenPart`, and `lastChar` remains empty.
5. If only a portion of the token is valid at the start or end, these valid characters are extracted into `lastChar`, while the remaining part is assigned to `tokenPart`.
</summary>*/
void Lexical::separateOperatorToken(const std::string& token, std::string& tokenPart, std::string& lastChar)
{
    tokenPart = "";
    lastChar = "";

    size_t start = 0;
    size_t end = token.length();

    // Check if the token has valid characters at both ends
    bool hasStartValid = false, hasEndValid = false;

    while (start < end && (isalnum(token[start]) || token[start] == '_'))
    {
        hasStartValid = true;
        start++;
    }

    while (end > start && (isalnum(token[end - 1]) || token[end - 1] == '_'))
    {
        hasEndValid = true;
        end--;
    }

    // If both start and end are valid, treat the whole token as tokenPart
    if (hasStartValid && hasEndValid)
    {
        tokenPart = token;
        return;
    }

    // Separate valid characters into lastChar and remaining into tokenPart
    lastChar = token.substr(0, start) + token.substr(end);
    tokenPart = token.substr(start, end - start);
}


// |-------------------------------------------------------------------------------------------------------------|
// |                                             Token Processing                                                |
// |-------------------------------------------------------------------------------------------------------------|

/* <summary>
This is the main function that performs lexical analysis on the content of a file. It reads the file line by line, processes tokens, and outputs results to both the console and output files.

Logic:
1. Open the input file (`test_code.txt`) and check for errors. If the file cannot be opened, output an error message and exit.
2. Open two additional output files (`tokenFile` for valid tokens, 'SymbolTable' for symbol table, and `errorFile` for invalid tokens) and check for errors.
3. Initialize `lineNum` to keep track of the current line number as the file is processed.
4. For each line in the input file:
   - Increment the `lineNum`.
   - Iterate through each character in the line.
   - Collect characters into a `token` until a space or special character is encountered, indicating the end of a token.
   - Process the token using the `processToken` function and clear the `token` buffer.
5. After processing the line, if there is any remaining token, process it as well.
6. Once all lines are processed:
   - Output the counts of different token types (Keywords, Identifiers, Numbers, Punctuations, Operators, and Invalid tokens) to the console and to the `tokenFile` and `errorFile`.
   - Output a summary of the total token count and the invalid tokens in the `errorFile`.
7. Close all files (`inputFile`, `tokenFile`, and `errorFile`) after processing is complete.
8. Output a message indicating that the lexical analysis is done and results are saved to the `tokenFile` and `errorFile`.
</summary>*/
int Lexical::PerformLexical(const std::string& Input, const std::string& Token, const std::string& Symbol, const std::string& Error)
{
    // Input File
    std::ifstream inputFile(Input);
    // Files to be created
    tokenFile.open(Token);
    symbolTableFile.open(Symbol);
    errorFile.open(Error);

    if (!inputFile.is_open())
    {
        std::cerr << "Error opening input file.\n";
        return 1;
    }
    if (!tokenFile.is_open())
    {
        std::cerr << "Error opening token file.\n";
        return 1;
    }
    if (!symbolTableFile.is_open())
    {
        std::cerr << "Error opening Symbol Table file.\n";
        return 1;
    }
    if (!errorFile.is_open())
    {
        std::cerr << "Error opening Error file.\n";
        return 1;
    }

    // Write headers for tokenFile
    tokenFile << std::left
        << std::setw(colWidthToken) << "Token Value"
        << std::setw(colWidthType) << "Token Type"
        << "\n";
    tokenFile << std::string(colWidthToken + colWidthType, '-') << "\n";

    // Write headers for symbolTableFile
    symbolTableFile << std::left
        << std::setw(colWidthToken) << "Token Value"
        << std::setw(colWidthType) << "Token Type"
        << std::setw(colWidthLine) << "Line No"
        << std::setw(colWidthTokenNo) << "Token No"
        << "\n";
    symbolTableFile << std::string(colWidthToken + colWidthType + colWidthLine + colWidthTokenNo, '-') << "\n";

    std::string line;
    int lineNum = 0;

    while (getline(inputFile, line))
    {
        lineNum++;
        std::string token;

        for (size_t i = 0; i < line.length(); ++i)
        {
            char c = line[i];

            // Handle space or special characters (tokens are separated by spaces or special chars)
            if (isspace(c) || c == '$' || c == ',' || c == ';' || c == '(' || c == ')')
            {
                if (!token.empty())
                {
                    processToken(token, lineNum);
                    token.clear();
                }
                continue;
            }

            token += c;
        }

        if (!token.empty())
        {
            processToken(token, lineNum);
        }
    }
    //// Console Output
    //std::cout << "\n\n|+++++++++++++++++++++++++++++++++|\n";
    //std::cout << "|\tToken Count               |\n";
    //std::cout << "|+++++++++++++++++++++++++++++++++|\n";
    //std::cout << "Keywords: " << nKeywords << std::endl;
    //std::cout << "Identifiers: " << nIdentifiers << std::endl;
    //std::cout << "Numbers: " << nNumbers << std::endl;
    //std::cout << "Punctuations: " << nPunctuations << std::endl;
    //std::cout << "Operators: " << nOperators << std::endl;
    //std::cout << "Invalid: " << nInvalid << std::endl;
    //std::cout << "Total Tokens (Valid): " << nKeywords + nIdentifiers + nNumbers + nPunctuations + nOperators << std::endl << std::endl;

    // File Output to Token
    /*tokenFile << "\n\n"
        << std::string(35, '+') << "\n"
        << "|        Token Count Summary       |\n"
        << std::string(35, '+') << "\n";

    tokenFile << std::left
        << std::setw(colWidthToken + 10) << "Keywords:"
        << std::setw(colWidthToken + 10) << nKeywords << "\n"
        << std::setw(colWidthToken + 10) << "Identifiers:"
        << std::setw(colWidthToken + 10) << nIdentifiers << "\n"
        << std::setw(colWidthToken + 10) << "Numbers:"
        << std::setw(colWidthToken + 10) << nNumbers << "\n"
        << std::setw(colWidthToken + 10) << "Punctuations:"
        << std::setw(colWidthToken + 10) << nPunctuations << "\n"
        << std::setw(colWidthToken + 10) << "Operators:"
        << std::setw(colWidthToken + 10) << nOperators << "\n"
        << std::setw(colWidthToken + 10) << "Invalid:"
        << std::setw(colWidthToken + 10) << nInvalid << "\n"
        << std::setw(colWidthToken + 10) << "Total Tokens (Valid):"
        << std::setw(colWidthToken + 10) << (nKeywords + nIdentifiers + nNumbers + nPunctuations + nOperators) << "\n";*/

    // File Output to Symbol Table
    symbolTableFile << "\n\n"
        << std::string(40, '+') << "\n"
        << "|         Token Count Summary         |\n"
        << std::string(40, '+') << "\n";

    symbolTableFile << std::left
        << std::setw(colWidthToken + 10) << "Keywords:"
        << std::setw(colWidthToken + 10) << nKeywords << "\n"
        << std::setw(colWidthToken + 10) << "Identifiers:"
        << std::setw(colWidthToken + 10) << nIdentifiers << "\n"
        << std::setw(colWidthToken + 10) << "Numbers:"
        << std::setw(colWidthToken + 10) << nNumbers << "\n"
        << std::setw(colWidthToken + 10) << "Punctuations:"
        << std::setw(colWidthToken + 10) << nPunctuations << "\n"
        << std::setw(colWidthToken + 10) << "Operators:"
        << std::setw(colWidthToken + 10) << nOperators << "\n"
        << std::setw(colWidthToken + 10) << "Invalid:"
        << std::setw(colWidthToken + 10) << nInvalid << "\n"
        << std::setw(colWidthToken + 10) << "Total Tokens (Valid):"
        << std::setw(colWidthToken + 10) << (nKeywords + nIdentifiers + nNumbers + nPunctuations + nOperators) << "\n";

    // File Output to Error
    errorFile << "\n\n"
        << std::string(35, '+') << "\n"
        << "|       Token Error Summary         |\n"
        << std::string(35, '+') << "\n";

    errorFile << std::left
        << std::setw(colWidthToken + 10) << "Invalid:"
        << std::setw(colWidthToken + 10) << nInvalid << "\n"
        << std::setw(colWidthToken + 10) << "Total Tokens (including Invalid) :"
        << std::setw(colWidthToken + 10) << (nKeywords + nIdentifiers + nNumbers + nPunctuations + nOperators + nInvalid) << "\n";

    inputFile.close();
    tokenFile.close();
    errorFile.close();
    std::cout << "Lexical analysis done. See Output in " << Token << ", "<< Symbol << " and "<< Error << " file\n";
    return 0;
}

/* <summary>
This function processes a given token to classify it into one of several categories:
- Keywords
- Identifiers
- Numbers
- Punctuation
- Operators
- Invalid Tokens

Logic:
1. If the token is empty, log "No Tokens" and return.
2. Initialize a flag `isProcessed` to `false`, which will track if the token has been successfully classified.
3. Attempt to classify the token into various categories in the following order:
   - **Keyword**:
     - Use `seperateKeywordToken` to separate the keyword part of the token.
     - If it is recognized as a keyword, increment the keyword count and log it.
     - If there are trailing characters, recursively process them as a new token.
   - **Identifier**:
     - Use `seperateIdentifierToken` to separate the identifier part of the token.
     - If it is recognized as a valid identifier, increment the identifier count and log it.
     - If there are trailing characters, recursively process them as a new token.
   - **Number**:
     - Use `seperateNumToken` to separate the number part of the token.
     - If it is recognized as a valid number, increment the number count and log it.
     - If there are trailing characters, recursively process them as a new token.
   - **Punctuation**:
     - Use `seperatePunctuationToken` to separate the punctuation part of the token.
     - If it is recognized as a valid punctuation character, increment the punctuation count and log it.
     - If there are trailing characters, recursively process them as a new token.
   - **Operator**:
     - Use `separateOperatorToken` to separate the operator part of the token.
     - If it is recognized as a valid operator, increment the operator count and log it.
     - If there are trailing characters, recursively process them as a new token.
4. If none of the categories match, classify the token as invalid, increment the invalid token count, and log an error message.
</summary>*/
void Lexical::processToken(const std::string& token, int lineNum)
{
    if (token.empty())
    {
        //cout << "No Tokens\n";
        tokenFile << std::left
            << std::setw(20) << "No Tokens"
            << std::setw(20) << "N/A" << "\n";
        symbolTableFile << std::left
            << std::setw(20) << "No Tokens"
            << std::setw(20) << "N/A"
            << std::setw(10) << "N/A"
            << std::setw(10) << "N/A" << "\n";
        return;
    }

    bool isProcessed = false; // Tracks if the token has been classified
    std::string tokenPart = token;
    std::string lastChar = "";

    // Keywords
    seperateKeywordToken(token, tokenPart, lastChar);
    if (isKeyword(tokenPart))
    {
        nKeywords++;
        std::cout << "Keyword: " << tokenPart << " at line " << lineNum << "\n";
        tokenFile << std::left
                  << std::setw(colWidthToken)  << tokenPart
                  << std::setw(colWidthType)   << "Keyword"
                  << "\n";

        symbolTableFile                   << std::left
            << std::setw(colWidthToken)   << tokenPart
            << std::setw(colWidthType)    << "Keyword"
            << std::setw(colWidthLine)    << lineNum
            << std::setw(colWidthTokenNo) << tokenNo
            << "\n";
        isProcessed = true;
        tokenNo++;
        if (lastChar != "")
            processToken(lastChar, lineNum);
        return;
    }

    // Identifier
    seperateIdentifierToken(token, tokenPart, lastChar);
    if (!isProcessed && identifierFSM(tokenPart) == 3) // Valid identifier state
    {
        nIdentifiers++;
        std::cout << "Identifier: " << tokenPart << " at line " << lineNum << "\n";
        tokenFile << std::left
                  << std::setw(colWidthToken) << tokenPart
                  << std::setw(colWidthType)  << "Identifier"
                  << "\n";

        symbolTableFile << std::left
                        << std::setw(colWidthToken)   << tokenPart
                        << std::setw(colWidthType)    << "Identifier"
                        << std::setw(colWidthLine)    << lineNum
                        << std::setw(colWidthTokenNo) << tokenNo
                        << "\n";
        isProcessed = true;
        tokenNo++;
        if (lastChar != "")
            processToken(lastChar, lineNum);
        return;
    }

    // Number
    seperateNumToken(token, tokenPart, lastChar);
    if (!isProcessed && numberFSM(tokenPart) != -1) // Valid number states
    {
        nNumbers++;
        std::cout << "Number: " << tokenPart << " at line " << lineNum << "\n";
        tokenFile << std::left
                  << std::setw(colWidthToken) << tokenPart
                  << std::setw(colWidthType)  << "Number"
                  << "\n";

        symbolTableFile << std::left
                        << std::setw(colWidthToken)   << tokenPart
                        << std::setw(colWidthType)    << "Number"
                        << std::setw(colWidthLine)    << lineNum
                        << std::setw(colWidthTokenNo) << tokenNo
                        << "\n";
        isProcessed = true;
        tokenNo++;
        if (lastChar != "")
            processToken(lastChar, lineNum);
        return;
    }

    // Punctuation
    seperatePunctuationToken(token, tokenPart, lastChar);
    if (!isProcessed && tokenPart.length() == 1 && punctuationFSM(tokenPart) != -1)
    {
        nPunctuations++;
        std::cout << "Punctuation: " << tokenPart << " at line " << lineNum << "\n";
        tokenFile << std::left
                  << std::setw(colWidthToken) << tokenPart
                  << std::setw(colWidthType)  << "Punctuation"
                  << "\n";

        symbolTableFile << std::left
                        << std::setw(colWidthToken)   << tokenPart
                        << std::setw(colWidthType)    << "Punctuation"
                        << std::setw(colWidthLine)    << lineNum
                        << std::setw(colWidthTokenNo) << tokenNo
                        << "\n";
        isProcessed = true;
        tokenNo++;
        if (lastChar != "")
            processToken(lastChar, lineNum);
        return;
    }

    // Operator
    separateOperatorToken(token, tokenPart, lastChar);
    if (!isProcessed && operatorFSM(tokenPart) != -1)
    {
        nOperators++;
        std::cout << "Operator: " << tokenPart << " at line " << lineNum << "\n";
        tokenFile << std::left
                  << std::setw(colWidthToken) << tokenPart
                  << std::setw(colWidthType)  << "Operator"
                  << "\n";

        symbolTableFile << std::left
                        << std::setw(colWidthToken)   << tokenPart
                        << std::setw(colWidthType)    << "Operator"
                        << std::setw(colWidthLine)    << lineNum
                        << std::setw(colWidthTokenNo) << tokenNo
                        << "\n";
        isProcessed = true;
        tokenNo++;
        if (lastChar != "")
            processToken(lastChar, lineNum);
        return;
    }

    // Invalid Token
    if (!isProcessed)
    {
        nInvalid++;
        std::cout << "Error: Invalid token \"" << token << "\" at line " << lineNum << "\n";
        errorFile << "Error: Invalid token \"" << token << "\" at line " << lineNum << "\n";
    }
}

#endif // LEXICAL_H