# Stygian Engine Contribution Guidelines

## Styleguide 

* [Formatting](#formatting)

* [Classes](#classes)

* [Includes](#includes)

* [Functions](#functions)

* [Comments](#comments)

* [Namespace](#namespace)

***

### Formatting
Tabs equivalent to four spaces are used for indents. The first curly brace denoting the scope of a function or loop/if statement, for example, should be on the same line as the function name and/or statement. The closing brace should be on a new line by itself. There should be exactly one newline in-between functions. Functions and data members can be bunched together in the class if they are believed to go together (getters, setters, etc.). 

***

## Classes
A .cpp is not necessary if the header is lightweight enough while including all necessary functions and data members.

### Naming
Classes and are to be named in camel case starting with a uppercase letter. Both header and class have to be the same name. Private data members should have the prefix "m_" before their variable name. The following variable name should be camel case but start with a lowercase letter (ex: m_oneTwo). 

In the case where storage classes and storage qualifiers can be interchanged, put them in alphabetical order. Common occurence: use "const static" instead of "static const".

### Filters & Subfolders
When you are adding a new class and/or header, make sure they are in the right filters. If you are unsure as to where to put it, ask other contributors. If you feel like it does not belong in any existing filter, discuss with other contributors and if they feel the same way, you can create a new filter. While creating a new filter, make sure you create a subfolder and mirror the reference from Visual Studio to the folder itself.

***

## Includes

### Fixed-Width Datatypes
Numerical data types are to be replaced by our own included in "Types.h". Please include and use this header in like almost everything that has an int or a float.

Includes should be in this order: 

Standard library, Windows API, External Includes (not written by us), Local Headers (written by us) 

***

## Functions
Function names should be written in camel case starting with a lowercase letter. If a function has "input" and "output" parameters, the input ones are to be put first and output last. Following from the previous convention, parameters that are references should be put first.

***

## Comments
Comments are written with the standard "// " (note the space). Comments are to be written if the function or variable is not straightforward. This is left to the individual programmer to judge. For example, getters and setters do NOT need comments. If a function is large and confusing, put a multiline comment before the function name explaining what it does and its different variables and parameters.

***

## Namespace
