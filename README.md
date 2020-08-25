# HolyC Interpreted:
A scripting language named after HolyC, a language authored by Terrence RIP.

## What is HolyC Interpreted?
It is a strongly and statically typed scripting language that closely resembles C, the language it is written in.

## Notable features
* Functions written in C can be used with HolyC-I
* Many built-in types
* Relatively fast because the HolyC-I interpreter is written in C.

## Usable datatypes:
* list
* bool
* int
* float
* char
* string
* object
* source

## Examples:

### Linking C programs to a HolyC-I program:
HolyC-I can execute functions written in C.
To load a function written in C, you can use the `dload` method, which is used to load C code from a shared object file(.so).
Example:
```C
# Load a hypothetical .so file called "helloworld.so".
dload("helloworld.so", helloworld);

# call the helloworld function located in "helloworld.so"
helloworld();
```
###  Lists and list methods:
Here is an example of a list:
```C
list colors = [
    "blue",
    "green",
    "red"
];
```
HolyC-I comes with two built in list methods `add()` and `remove()`.
#### Adding to a list:
```C
list colors = [];

colors.add("blue");
```

#### Removing from a list:
```C
list colors = ["blue"];

# Remove item by index
colors.remove(0);
```

### Built in methods:
* print
* aprint
* include
* wad
* lad
* visit
* fopen
* fputs
* fclose
* input
* time
* free

More documentation coming soon!
