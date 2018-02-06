# pycparser tree layout

## Top level
The top level of a tree is either a FileAST node or a list (if done from a string). If it is a FileAST from a file,
then this object .ext contains the list that will function as the top level of the tree.

## Node types

### FuncCall - Function call
A node representing the calling of a function.
* `obj.name` - An ID object whose name attribute contains the string name of the function
* `obj.args` - An ExprList object, whose exprs attribute is a list of the things being handed into the function.

### Decl - Declaration
This statement is used to describe a variable or function declaration. Its relevant aspects are:
* `obj.name` - The name of the variable or function
* `obj.type` - An object that describes the type of the object being defined
  * `obj.type.declname` - The name (again)
  * `obj.type.type` - An IdentifierType object describing the type of object (e.g. int)
    * `obj.type.type.names` - A list containing the designations of this type. For example, 'int' or 'unsigned'.
* `obj.init` - Contains an object describing what this was initialised as. It could be a Constant object, or any
  other type of tree entry.

### Assignment - Assigning a value to a variable
This is a fairly simple statement which will always contain two subnodes and will perform an assignment.
* `obj.lvalue` - A tree object, possibly an ID object, showing the thing being assigned to.
* `obj.rvalue` - A tree object showing what goes on the right hand side. Could be a Constant or BinaryOp, but could be
    anything that counts as an rvalue in the C sense.
* `obj.op` - The operation used to do the assignment. Probably an = sign.

### Constant
A simple thing representing a constant value.
* `obj.type` - The type of constant. Realistically either "string", "int" or "float".
* `obj.value` - A string holding the value of the constant as written. For a string, the quotes are included in this.

### UnaryOp - Unary operation
An operation involving only one object.
* `obj.op` - A string containing the operation. Examples include "-" (for a negative number) or "!" (for negation)
* `obj.expr` - A node representing what is being acted on. For "-50", this is a Constant, but it could be more complex.

### BinaryOp
### ID
### Cast
### If
### Compound
### For
### While