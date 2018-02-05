* Fiddled around with pycparser
* Started down one way of doing preprocessor. Eventually settled on system of going to first #include match, making the
  replacement, then restarting the search.
* Programmed this and made REPL at end, but it had no effect (git: da26fca)
* Some fiddling later, the include statement is added in. Also changed to mean that any number of includes should work.
* Wrote an initial attempt at the #define management and made define.c to test it