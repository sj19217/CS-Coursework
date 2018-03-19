# Potential improvements for Iteration C

## Assembler

* GUI - Add an actual button to step through assembly

## Interpreter

## Compiler

* Cases with two end* lines can be merged
* If an if statement contains a simple binary operation, convert it to native assembler jumping rather than evaluating the expression, reading from the stack and doing the comparison with 1.
* Look for cases like "SUB uint esp 4, SUB uint esp 2" and replace them with merged versions, e.g. "SUB uint esp 6"
* GUI - Add an actual button to step through compilation