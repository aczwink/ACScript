# Strong type system
ACScript uses a strong type system.
Strong typing requires excessive boilerplate code in traditional programming languages.
Amongst other newer programming languages, ACScript also makes use of type inference by context to reduce boilerplate as much as possible.
Consider this simple function:
```
let square := x -> x * x;
```

What does this function specify?
Some might say it is a function accepting some kind of number value and returning its square.
Some might say that the function declaration is missing.

In ACScript, only the part `x -> x * x` is a function, i.e. it doesn't have a name by itself.
You can assign it to a variable (as did in the example), but you don't need to.
The function itself has a type, which is generic, since the type of `x` could be anything.
How can this be a generic, one might ask.
This is achieved by "automatic constraint deduction" i.e. the constraints are deduced from the function definition.
So the type of the function `x -> x * x` is `T -> U` with `T = { *: (T x T) -> U }`.
In other words, the binary `*`-operator must exist on the type of the parameter.

You could further limit the type of the function by specifying a type for the variable you assign it to.
Example:
```
let square: float64 -> float64 := x -> x * x;
```
Note again that this does not change the signature of the function, but it assigns the function to a variable.
The variable has a specified type (`float64 -> float64`) and `T -> U` with `T = { *: (T x T) -> U }` can be assigned to `float64 -> float64`.
Obviously, it only can be assigned, because the `*`-operator is defined on `float64`.
So, while the function itself would accept any type that suffices its constraints, the variable `square` only accepts float64 although referring to the same function.


# Functions
Functions accept exactly one parameter and return exactly one value - always.
Multiple values can be passed to a function by passing a tuple and like this also multiple values can be returned.

Functions are called like this:
```
let sum := (x, y) -> x + y;

let test := sum( (3, 5) );
```

For convenience, when passing multiple values, the tuple parentheses can be omitted.
I.e. the following is equivalent to the previous example:
```
let sum := (x, y) -> x + y;

let test := sum( 3, 5 );
```

As said before, exactly one parameter is always required, even if the function does not need one.
In such a case, we suggest to use `null` as parameter, but this is not enforced.
However, when calling a function and omitting a value, it implicitly calls it with value `null`.
Example:
```
let someFunc := null -> 13;

let test := someFunc(); //someFunc() is equivalent to someFunc(null)
```

## Pattern matching

Functions contain at least one rule.
When having more than one rule, braces are needed.
Every rule consists of a pattern matching on the parameter, an optional predicate and the function body.
Rules are evaluated in order from top to bottom until a matching one is found.
The function body is either an expression or a statement block surrounded by braces.

Example of two rules with only pattern matching:
```
let fac := {
    0 -> 1,
    n -> n * fac(n-1)
};
```

Example of two rules with a predicate:
```
let fac := {
    n | n = 0 -> 1,
    n -> n * fac(n-1)
};
```

## Recursion

Since functions are just values that don't have names, recursion requires the special keyword `self`, that refers to the function it is used in.
The previous faculty-example can be written without the need of a variable like this:
```
{
    0 -> 1,
    n -> n * self(n-1)
};
```



# Closure
```
type ModifyFunc := float64 -> float64;

let square: ModifyFunc := x -> x + x;

let c := 10;
let square_plus_c: ModifyFunc := x -> square(x) + c; //both c and square are captured by reference 
```


# Objects
```
let obj := {
    x: 3,
    y: 5
};
```

## Methods

There really is no special difference between a method and a normal function.
A method, since it is defined within an object, can access the object through the `this` keyword, since it is in the methods' closure.

```
let obj := {
    x: 3,
    
    sum: y -> this.x + y
};

let test := obj.sum(5);
```

This is roughly equivalent to:
```
let sum := ({x}, y) -> x + y;

let obj := {
    x: 3,
};

let test := sum( (obj, 5) );
```

Infix notation is always resolved the following way.
The expression `x + y` will be translated to `x.+(y)` in case the type of `x` defines that operator.
If not, the expression is will be resolved as `+(x, y)`.
By the way, in ACScript `+` is just a function name, like any other identifier.



## Object types

Objects can suffice object types:
```
type Test := {
    x: float64;
    
    func: (Test, Test) -> float64;
    func2: Test -> float64;
};

let obj: Test := {
    x: 3,
    
    func: (a, b) -> a.x + b.x,
    func2: a -> this.func(this, a)
};
```





# Braces

Braces are used for grouping multiple things in ACScript.
Depending on whats inside the braces they have a different meaning.

A statement-block:
```
{
    stmt1;
    stmt2;
    //and so on...
}
```

A set:
```
{
    expression1,
    expression2,
    //...
}
```

An object:
```
{
    identifier1: expression1,
    identifier2: expression2,
    //...
}
```

A type:
```
{
    declaration1;
    declaration2;
    //...
}
```