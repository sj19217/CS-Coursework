## Test Data For Development
* **Think of early ones**
* Tested with fibonacci program; unpacking error due to wring tabbing on return of divide_and_contextualise
* Tested with fibonacci program; unpacking error due to line in meta section of asm file using space instead of =
* Tested with fibonacci program; error hashing list. Removed LRU cache since it could not track arguments.
* AttributeError: 'DataInstruction' object has no attribute 'operand1' - Checked to make sure that no attempt is made
    to replace a DataInstruction's operands (which do not exist)