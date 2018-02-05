* Fiddled around with pycparser
* Started down one way of doing preprocessor. Eventually settled on system of going to first #include match, making the
  replacement, then restarting the search.
* Programmed this and made REPL at end, but it had no effect (git: da26fca)
* Some fiddling later, the include statement is added in. Also changed to mean that any number of includes should work.
* Wrote an initial attempt at the #define management and made define.c to test it. Problems were:
  * Referencing m.start() and m.end(), but m was left as None from the #include stuff. Changed to m_def and m_undef.
  * Forgot to write regex for #undef and put logic in to check for the right undef statement
  * Entering an infinite loop because I didn't put the edited lines back into the main string