#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 42
#define MATCH_SUCCESS 1
#define MATCH_FAIL 0

/**
 * Finds index number of pattern.
 * argv are commands given in command line and argc is how many commands exist.
 * Returns this index number.
 * (Note: if doesn't exist, exit program)
 */
int get_pattern_index(int argc, char *argv[]) {
    int patternIndex = 0;      
    for (int i = 1; i < argc; i++) {
        // Pattern never contains '-'
        if (strstr(argv[i], "-") == NULL) {
            patternIndex = i;
            break;
        }
    }    
    if (patternIndex == 0) {
        fprintf(stderr, "Usage: search [-exact|-prefix|-anywhere]"
                " [-sort] pattern [filename]\n");
        exit(1);
    }
    return patternIndex;
}

/**
 * Finds filename given in command line.
 * argc is number of commands given, argv are commands given and
 * patternIndex is index number of pattern.
 * Returns path if exists, otherwise returns the default path.
 */
char *get_file_name(int argc, char *argv[], int patternIndex) { 
    if (patternIndex < (argc - 1)) {
        // File name is given after pattern
        // Note the -1, since index is always 1 less
        char *fileName = argv[argc - 1];
        return fileName;
    } else {
        char *defaultDirectory = "/usr/share/dict/words"; 
        return defaultDirectory; 
    }
}

/**
 * Exit if contain invalid search type errors.
 * 1)If 2 or more search type.
 * 2)If not a valid command(i.e random words).
 * argc is number of commands, argv are commands given.
 */
void invalid_search_type(int argc, char *argv[], int patternIndex) {
    int counter = 0;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-exact") == 0 || strcmp(argv[i], "-prefix") == 0
                || strcmp(argv[i], "-anywhere") == 0) {
            // Count number of types given
            counter += 1;
        } 
    }
    if (counter > 1) {
        // Contains 2 or more search type
        fprintf(stderr, "Usage: search [-exact|-prefix|-anywhere]"
                " [-sort] pattern [filename]\n");
        exit(1);
    }
    for (int j = 1; j < patternIndex; j++) {
        if (strcmp(argv[j], "-exact") != 0 &&  
                strcmp(argv[j], "-prefix") != 0 &&
                strcmp(argv[j], "-anywhere") != 0 &&
                strcmp(argv[j], "-sort") != 0) {
            // Command line not one of the valid options
            fprintf(stderr, "Usage: search [-exact|-prefix|-anywhere]"
                    " [-sort] pattern [filename]\n");
            exit(1);
        } 
    }
}

/**
 * Exit if pattern is not a letter or question mark.
 * The parameter pattern is the pattern given.
 */
void invalid_pattern(char pattern[]) {
    int i;
    if (pattern[0] != '\0') {
        // Note: Null terminal is not alphabetical
        for (i = 0; i < strlen(pattern); i++) {
            if (isalpha(pattern[i]) == 0 && pattern[i] != '?') {
                // Only alphabetical and '?'
                fprintf(stderr, "search: pattern should only contain question"
                        " marks and letters\n");
                exit(1);  
            }

        }
    }
}

/**
 * Exact search algorithm to match pattern with words.
 * -Matches word exactly where both same length and
 * -a question mark in pattern represents any alphabetical character.
 * pattern is the pattern given and line is a word from dictionary.
 * Returns 1 if pattern matches or 0 if doesn't.
 * (Note: Exit if pattern is empty)
 */
int exact_search(char pattern[], char line[]) {
    if (strlen(pattern) == 0) {
        // Exact cannot search empty pattern
        exit(1);
    }
    if (strlen(pattern) != strlen(line)) {
        return MATCH_FAIL;
    }
    for (int j = 0; j < strlen(line); j++) {
        // Word only contains alphabet
        if (isalpha(line[j]) == 0) {
            return MATCH_FAIL;
        }
    }
    for (int i = 0; i < strlen(pattern); i++) {
        if (pattern[i] != '?') {
            if (toupper(pattern[i]) != line[i] && 
                    tolower(pattern[i]) != line[i]) {
                /* Check for matching letters
                 * Note: case insensitive, i.e A = a*/
                return MATCH_FAIL;
            }
        }
    }
    return MATCH_SUCCESS;
}   

/**
 * Prefix search algorithm to match pattern with words.
 * -Matches the beginning part of words
 * -Words must be equal or longer than pattern 
 * -Question mark represents any alphabetical character.
 * pattern is pattern given and line is a word from dictionary.
 * Returns 1 if matches or 0 if doesn't.
 */
int prefix_search(char pattern[], char line[]) {
    for (int j = 0; j < strlen(line); j++) {
        if (isalpha(line[j]) == 0) {
            // Word only contains alphabet
            return MATCH_FAIL;
        }
    }
    if (strlen(pattern) == 0) {
        return MATCH_SUCCESS;
    }
    if (strlen(pattern) > strlen(line)) {
        // Must be equal or longer in length to match
        return MATCH_FAIL;
    }
    for (int i = 0; i < strlen(pattern); i++) {
        if (pattern[i] == '?') {
            continue;
        } else {
            if (toupper(pattern[i]) != line[i] && 
                    tolower(pattern[i]) != line[i]) {
                // Case insensitive letter matching check
                return MATCH_FAIL;
            }
        }
    }
    return MATCH_SUCCESS;
}

/**
 * Helper function to assist anywhere search algorithm. Deals with simple 
 * cases(word contains non-alphabet, pattern has only 1 character, empty 
 * pattern).
 * pattern is pattern given and line is word from dictionary
 * Returns 1 if matches, 0 if doesn't and 3 if not sufficient info(so
 * carry on with anywhere search algo).
 */
int anywhere_basic_check(char pattern[], char line[]) {
    for (int i = 0; i < strlen(line); i++) {
        if (isalpha(line[i]) == 0) {
            // If word is not alphabetical
            return MATCH_FAIL;
        }
    }
    /* Represents how many characters, its position and number of end question
     * marks at the end in pattern
     * */
    int counter = 0, letterIndex = 0, endQuestionNum = 0;
    for (int d = 0; d < strlen(pattern); d++) {
        if (isalpha(pattern[d]) != 0) {
            /* Keep track of 1 letter and its position
             * i.e Pattern = ?k or ??e
             */
            counter++;
            letterIndex = d;
        }
    }
    /* Number of question marks after the last letter
     * For example: ?e??? has 3 question marks at the end
     */
    endQuestionNum = (int) strlen(pattern) - letterIndex - 1;
    if (counter == 1) {
        // Pattern contains only 1 character
        for (int m = letterIndex; m < strlen(line); m++) {
            /* Note: m starts at letterIndex, therefore "?k" will not return
             * "king" but will return "fluking".
             */
            if (((int) strlen(line) - m - 1) >= endQuestionNum &&
                    (line[m] == tolower(pattern[letterIndex]) ||
                    line[m] == toupper(pattern[letterIndex]))) {
                /*
                 * Word must have characters to represent '?'
                 * (See above comment).
                 * Also must contain the character.
                 * Note: case insensitive
                 */
                return MATCH_SUCCESS;
            }
        }
    }
    if (strlen(pattern) == 0) {
        return MATCH_SUCCESS;
    }
    return 3;
}

/**
 * Anywhere search algorithm to match pattern with words.
 * -Matches any part of word
 * -Any number of letters
 * -Question mark represents alphabetical character 
 * pattern is pattern given and line is word from dict.
 * Returns 1 if matches and 0 if doesn't.
 */
int anywhere_search(char pattern[], char line[]) {
    int basicCheck = anywhere_basic_check(pattern, line);// Check basic cases
    if (basicCheck == MATCH_SUCCESS) { //Check to match, discard or do nothing
        return MATCH_SUCCESS;
    } else if (basicCheck == MATCH_FAIL) {
        return MATCH_FAIL;
    }
    int lastLetterIndex = 0; 
    int index[strlen(pattern)];
    for (int z = 0; z < strlen(pattern); z++) {
        if (pattern[z] != '?') {//1 = letter, else = '?'."t?st"->index = {1011}
            index[z] = 1;
            lastLetterIndex = z;
        }
    }
    int numOfEndQuestion = strlen(pattern) - lastLetterIndex - 1;//"p?r???"->3 
    // If word matches, 1st letter in pat matches in word, index of last match
    int match = 0, firstFind = 0, wordLastMatch = 0;
    for (int j = 0; j < strlen(pattern); j++) {
        if (firstFind == 1) {// 1st char in pat and the other relative letter
            return MATCH_FAIL;// positions do not match word.
        }
        if (pattern[j] != '?') { // firstFind is tracked to stop j loop
            firstFind += 1;
            for (int k = j; k < strlen(line); k++) {
                if (match == 1 && ((int) strlen(line) - (k + wordLastMatch - j)
                        >= numOfEndQuestion)) {// char exists to represent '?'
                    return MATCH_SUCCESS; 
                }
                if (toupper(line[k]) == pattern[j] ||// Char in pattern in word
                        tolower(line[k]) == pattern[j]) {// Case insensitive
                    for (int l = j + 1; l < sizeof(index) / sizeof(index[0]);
                            // Loop to check relative positions of letters
                            l++) {//Note: l = j+1(first char already matched)
                        if (index[l] == 1) {// If is a letter and not '?'
                            if (toupper(line[k + l - j]) != pattern[l] &&
                                    tolower(line[k + l - j]) != pattern[l]) {
                                match = 0;//k + l - j = relative pos of  
                                break;// letters from j. Check if not the same
                            } else { /* If 2 letters are the same*/
                                match = 1;
                                wordLastMatch = l;// Last letter match in word
                            }
                        }
                    }
                }
            }
        }
    }
    return MATCH_FAIL;
}

/**
 * Determines if a sort command was given.
 * argc is number of commands, argv are commands given.
 * Returns 1 if exists and 0 if doesn't.
 */
int has_sort(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-sort") == 0) {
            // Check command line contains sort option
            return MATCH_SUCCESS;
        } 
    }
    return MATCH_FAIL;
}

/**
 * Sort function for alphabetical order.
 * s1 is the first word and s2 is second word.
 * Returns <0 if first word is less, >0 if is greater and 0 if equal. 
 */
int compare(const void *s1, const void *s2) {
    return strcasecmp(*(char**) s1, *(char**) s2); 
}

int main(int argc, char *argv[]) {
    int patternIndex = get_pattern_index(argc, argv);
    invalid_search_type(argc, argv, patternIndex);
    char *pattern = argv[patternIndex];
    invalid_pattern(pattern);
    char line[MAX_WORD_LENGTH];
    char *fileName = get_file_name(argc, argv, patternIndex);
    FILE *fContent;
    char **toSort = NULL; 
    int i = 0;
    fContent = fopen(fileName, "r");
    if (fContent != NULL) {
        while (fgets(line, MAX_WORD_LENGTH, fContent) != NULL) {
            int matches = 0;
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(argv[1], "-exact") == 0) {
                matches = exact_search(pattern, line); 
            } else if (strcmp(argv[1], "-prefix") == 0) {
                matches = prefix_search(pattern, line);
            } else if (strcmp(argv[1], "-anywhere") == 0) {
                matches = anywhere_search(pattern, line);
            } else {
                matches = exact_search(pattern, line); // Default argument
            }
            if (matches) {
                i++;
                toSort = (char**) realloc(toSort, i * sizeof(char*));
                toSort[i - 1] = strdup(line);
            }
        }
    } else {
        fprintf(stderr, "search: file \"%s\" can"
                " not be opened\n", fileName);  
        exit(1);
    }
    if (has_sort(argc, argv) == MATCH_SUCCESS) {
        qsort(toSort, i, sizeof(char*), compare);
    }
    for (int j = 0; j < i; j++) {
        printf("%s\n", toSort[j]);
        free(toSort[j]);
    }
    if (i > 0) {
        return 0;
    } else {
        return 1;
    }
}
