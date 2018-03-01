* Fiddled around with pycparser
* Started down one way of doing preprocessor. Eventually settled on system of going to first #include match, making the
  replacement, then restarting the search.
* Programmed this and made REPL at end, but it had no effect (git: da26fca)
* Some fiddling later, the include statement is added in. Also changed to mean that any number of includes should work.
* Wrote an initial attempt at the #define management and made define.c to test it. Problems were:
  * Referencing m.start() and m.end(), but m was left as None from the #include stuff. Changed to m_def and m_undef.
  * Forgot to write regex for #undef and put logic in to check for the right undef statement
  * Entering an infinite loop because I didn't put the edited lines back into the main string (git commit bcff5a7)
  * Changed the regex because the \n at the end was causing lineof() not to match
  * Debugging progress shows it pretty much works, except the #define and #undef lines need deleting
  * Made the definition lines get safely deleted. Moved to a more complex example. It didn't work.
  * lineof() didn't find the #undef statement, making it return -1, and causing the wrong thing to be deleted.
    Solved by running .strip() on the pattern to match first.
  * Sneaky problem, when removing the line corresponding to #undef, I forgot its index has been shifted 1 up by the
    deletion of the #define line
  * It now works with the more complicated example :)
* Went back and made better comments
* When commenting, found a logical error (only looked for first #undef statement, not the one that matches)
* After beginning to write if statement code, realised that what is defined where has been lost. So I went back and
  added defined_lines (git commit 5f875c5)
* Completed first attempt at logic for the if statements
* Added in code to act on the constraints
* Added in logging
* Was getting stuck because the loop looking for #undef matches has no way to continue, so fixed that
* Then getting an error from popping from an empty stack, because it was never added to (5f875c5)
* The if statements now work but not yet removing the if statements from the text
* Removes all lines beginning with # and all duplicate newlines
* Created tree_layout.md to describe how the tree from pycparser works
* Created the globals parser, which was not too difficult
* Created compile.py which joins each of the other modules together
* Tried to include a lexer then a parser, to show the list of tokens simply, but then found that a CLexer is only meant
  to be made inside a CParser and decided against it. May program my own system later.
* Created variable_traversal.py and found it surprisingly simple to make
  * Looks for each Decl and adds it to the locals
  * Goes through any sub-blocks and runs itself on them
  * Goes through any expressions looking for variables and making sure they exist
  * This last one used "yield from" in a recursive generator, an astonishingly elegant Python solution
* Wrote the code block generation so that it has a skeleton and can deal with very basic things
* Wrote init section for a code block
* Wrote code for pushing values, then did some fiddling about for debugging
* Implemented printf
* Wrote returning code
* First apparently successful run
* Wrote code that *should* work to evaluate initial values of variables
* Implemented binary operations, then assignment. Got a bug to do with the type on top of the stack.
* Traced output with pen and paper, and it should be working.