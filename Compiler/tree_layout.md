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
* `obj.type` - An object that describes the type of the object being defined. Could be a TypeDecl, PtrDecl or ArrayDecl.
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
This is an operation, most likely a mathematical one, which has two operands.
* `obj.op` - A string containing the operation, like "+" or "<<"
* `obj.left` - A node containing the left hand side of the operation
* `obj.right` - A node containing  the right hand side of the operation

### ID
This is the node used to hold the name of a variable, typically.
* `obj.name` - A string containing the name of the item

### Cast
Represents a cast from one type to another.
* `obj.to_type` - A Typename object holding the type that the object is being cast into
* `obj.expr` - The object to be cast

### If
Represents an if statement.
* `obj.cond` - Contains the expression whose truth will determine whether to run the statement or not
* `obj.iftrue` - Contains a Compound object which is the code block to be run if the expression was true
* `obj.iffalse` - Contains a Compound object which is the code block to be run if the expression was false

### Compound
A node which contains a sequence of other nodes. Possibly where local variables will be attached to.
* `obj.block_items` - A list containing all of the elements in their sequence

### For
Represents a for loop.
* `obj.init` - A DeclList object, whose decls attribute contains a list of Decl objects, representing the content of the
  first part of a for loop's initialisation
* `obj.cond` - An expression object showing the comparison. Typically would be a BinaryOp.
* `obj.next` - Represents what to do after each iteration. In the case of "i++", it would be a UnaryOp with the "p++" op
* `obj.stmt` - A Compound object containing the code block in the for loop

### While
Represents a while loop. 
* `obj.cond` - A node, possibly a BinaryOp in the case of a comparison, for the expression that has to be checked for
  each loop round.
* `obj.stmt` - A Compound object which contains the block of code 

### Typename
A seemingly unnecessarily complex object that defines a type, like an int, for the purposes of something like a cast or
a call to sizeof().
* `obj.name` - If in an assignment, this is the name of the variable being assigned to. If elsewhere, like in a cast,
  this will just be None.
* `obj.type` - A TypeDecl object:
  * `obj.type.type` - An IdentifierType object
    * `obj.type.type.names` - Finally, this is the one you want. It is a list containing all of the declared specifics
      of this type. The data type will be one, like "int", but also "unsigned" could be in there.

### TypeDecl
Used to state a type within a declaration (under `decl.type`), either directly under the `Decl` object, or further under
a PtrDecl or ArrayDecl. It has only one attribute of use:
* `obj.type` - An instance of IdentifierType, whose `.names` attribute holds a list of things the type is declared as
  (such as "int" and "unsigned").

### PtrDecl
Used within the `type` attribute of Decl objects to state that this is a declaration of a pointer, not just a normal
type.
* `obj.type` - A TypeDecl object for what type this is a pointer to.

### ArrayDecl
Used within a Decl object to show that this is an array declaration. 
* `obj.type` - A TypeDecl object for what type of array this is.
* `obj.dim` - Contains the length of the array. Probably a Constant (though only if it was defined by a constant).

### ArrayRef
Means that an array is being subscripted, e.g. at an occurrence of myArray\[0\].
* `obj.name` - The name of the array variable
* `obj.subscript` - The subscript, i.e. the number in the square brackets. Could be any kind of expression.

## Compound and non-compound statements
Some statements are compound statements, meaning they have sub-blocks. Others are not, though these non-compound blocks
may still have sub-nodes. These sub-nodes are simply something that will probably be evaluated on the stack directly,
rather than spawning a new block of code with its own local variables.