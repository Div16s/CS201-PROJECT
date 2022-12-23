There are three codes in this folder
1. subandrepeat.c
    This above code demonstrates the implementation of suffix tree using ukkonen algorithm and its first two applications.
    A.)To find if a substring is present in a text
    B.)To find the longest repeated substring in a test
2. longestcommon.c
    In this code we have used suffix tree to achieve the goal of finding longest common substring between two strings.
3  palindrome.cpp
    This code demonstrates another application of suffix tree to find longest palindromic substring of a text.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SOME NOTES:-
1.) Code 1 and 2 are our main project files that we implemented in C.

2.) Code 3 is another good application of suffix tree that we implemented and since unordered_set data structure needs to be used to maintain the O(N) complexity , so it is implemented in c++.
    other than use of undordered_set all implementation is done as C language in this code("palindrome.cpp").
3.) To run simply compile code and run to see some examples that we have shown

4.) for palindrome.cpp if you want to check a test case simply change text string in line 299 to "<string>#<reversestring>$".and change size1 to (stringlength of <string>)+1.
     here <string> is the original string in which you want to search for palindrome eg. "HELLO" is <string> then "OLLEH" is <reversestring>.

5.) for longest common.c if you want to check a test case simply change text string in line 299 to "<string1>#<string2>$".and change size1 to (stringlength of <string>)+1.
     here <string1> is the first string and <string2> is second string. now run code to find longest common substring between them.
6.) for subandrepeat.c  to check for longest repeating substring.
    write the following lines in int main()
.........................................
    strcpy(text, "string$");
    buildsuffixtree();   
    repeat();
    freetree(root); 
.........................................
here string is the string in which you want to search

7.) for subandrepeat.c to check for a substring 
    write the following lines in int main()
.........................................
    strcpy(text,"string$");
    buildsuffixtree();
    subcheck("string2")
    freetree(root);
.........................................
here string is the main text in which you want to search and "string2" is the substring you want to search.