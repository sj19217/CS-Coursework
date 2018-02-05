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
  added defined_lines.