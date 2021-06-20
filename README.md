# WordSearch

A program to search for words in a dictionary, given a specific pattern. You have the ability to choose different types of pattern format of **Exact, Prefix and Anywhere**, whether to sort the matched words and choose the dictionary.

## Usage

### Compile files
cd WordSearch

make

### The program takes the following commandline arguments
Usage: **./search [-exact|-prefix|-anywhere] [-sort] pattern [filename]** where options with [] are optional.

**Note:** filename is the path to the dictionary to search. Default path is /usr/share/dict/words. Default pattern type is -exact. -sort is off on default.

-exact: Matches word exactly like the pattern i.e ar???? -> ardent, Artic.

-prefix: Pattern matches the beginning part of a word i.e are -> are, area.

-anywhere: Pattern matches any part of the word i.e en -> ardent, arena.

-sort: Sorts the list of returned words in alphabetical order.

pattern: patterns can be specified using words and "?", examples are seen above.
