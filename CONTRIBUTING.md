# Contributing to Turpial Firmware guidelines.

First of all thanks for taking time to contribute to this project!

Here is a list of guidelines to contribute to **Locha Mesh**, and this time
to the turpial-firmware.

## Table of Contents. 

[Contributing to Turpial Firmware guidelines.](#contributing-to-turpial-firmware-guidelines)

[Before starting.](#before-starting)

- [Contributing to Turpial Firmware guidelines.](#contributing-to-turpial-firmware-guidelines)
  - [Table of Contents.](#table-of-contents)
  - [Before starting.](#before-starting)
  - [How can I contribute?](#how-can-i-contribute)
    - [I have a question.](#i-have-a-question)
    - [Suggesting features and reporting bugs.](#suggesting-features-and-reporting-bugs)
    - [Making a Pull-Request.](#making-a-pull-request)
    - [Start a new change](#start-a-new-change)
    - [Static functions and global functions.](#static-functions-and-global-functions)
    - [const keyword.](#const-keyword)
    - [Indentation.](#indentation)

- [Contributing to Turpial Firmware guidelines.](#contributing-to-turpial-firmware-guidelines)
  - [Table of Contents.](#table-of-contents)
  - [Before starting.](#before-starting)
  - [How can I contribute?](#how-can-i-contribute)
    - [I have a question.](#i-have-a-question)
    - [Suggesting features and reporting bugs.](#suggesting-features-and-reporting-bugs)
    - [Making a Pull-Request.](#making-a-pull-request)
    - [Start a new change](#start-a-new-change)
    - [Static functions and global functions.](#static-functions-and-global-functions)
    - [const keyword.](#const-keyword)
    - [Indentation.](#indentation)

* [Documentation styleguide.](#documentation-styleguide)

## Before starting.

Please read our [code of conduct.](CODE_OF_CONDUCT.md)

## How can I contribute?

We'll pleased to accept your patches and contributions to this project. There 
are some guidelines you need to follow.

### I have a question.

If you have a question you can write to use using Twitter at
[@Locha_io](https://twitter.com/Locha_io), or through our website
[locha.io](https://locha.io).

### Suggesting features and reporting bugs.

You can use our issue tracker to share your idea, it will be discussed by the
Locha Mesh team members. If we all agree and makes sense to implement this
feature, it will be kept opened.

You can open a new issue reporting a bug in our repository, please provide
detailed information about the issues you're expecting.

### Making a Pull-Request.

You can contribute making a Pull-Request with the code you want to fix, or with
the features that you would like to implement. It will follow a review process
first, and after that it can be merged.

### Start a new change
Before you start making modifications, run these commands to create a new branch that is synchronized with dev:

Download the branches in the repository.

``` git fetch --all``` 
***

It changes you to the dev branch in case you are not in it.

```git checkout dev``` 
***

To synchronize the dev branch locally.

```git pull origin dev``` 
***

Create a new branch synchronized with dev.

```git checkout -b featureNameYouWantToDo```  
***

```git push pr featureNameYouWantToDo```  upload your change to github
***

make some changes

### Upload your changes and make Pull Request
Once you made the changes you want to propose, perform the following steps:

```git add -A```.

```git commit -m "Here a description of your changes"```.
***
    
### Configure your user data (You only need to enter this information the first time you push):

```git config --global user.email "tu-email@gmail.com"```

```git config --global user.name "<tu-usuario>"```
***

### Push your commit to your repository fork: 
    
```git push featureNameYouWantToDo # upload your change to github```

```git push -u pr```
***

Go to the GitHub page, if you go to your repository fork, click on the Branch button and select the branch in which you made the name changes of the nature you want to do. Then press the Pull request button.

## C++ code style guide.

Actually this project uses some parts of the
[Google C++ styleguide](https://google.github.io/styleguide/cppguide.html) and
we use a `.clang-format` file to format the documents.

It's recommended to run `clang-format` on the code you modify, so the review is
more easy.

### Guideline goals.

The goals as we see them actually are based to optimize the code to be readed
efficiently; We expect the codebase to last long and we prioritize reading over
writing.

- We choose to explicitly optimize the user experience of the programmers
reading the source code, mantaining and debugging the code in our codebase.
We "leave a trace to the reader" to give a hint to who is reading by making
the code very explicit about the intentions.

- Be consistent with existing code. Actually having a common set of guidelines
allows us to focus on other problems more important.

### Commit messages.

Commit messages should be always short, describing the changes. If the commit
changes various files a more detailed (in other lines) description should be
done describing the changes each one.

### Include guards.

All header files should use include guards, there aren't exceptions to this
rule. The format to create include guards is as follows:

```cpp
// Example file: foo.h

#ifndef FOO_H
#define FOO_H
...
#endif // FOO_H
```

### `#include` declarations.

The `#include` declarations should be positioned **only** in the top of the file
by default.

The header files should be included in the following order:

1. C system headers.
2. C++ standard library headers.
3. Library headers.
4. Project headers.

They should be put in descending order.

### Naming rules.

1. Use a descriptive name and coherent with the code.
2. All names should be in english.

#### Macros.

Please don't define macros if you don't need really to. If you do it you can
name them like this:

`MY_MACRO_THAT_SCARES_SMALL_CHILDREN_AND_ADULTS_ALIKE`

They should use uppercase letters and underscores.

```cpp
// Examples:

#define ROUND(x) ...
#define PI_ROUNDED 3.0
#define LOW_NIBBLE(x) (x & 0xf)
```

#### Variables.

1. To separate words use underscores, variables should use `snake_case` naming.
2. Don't group variables by type.
3. Private variables use a `m_` prefix.
4. Global variables use the `g_` prefix.

```cpp
// GOOD, underscore
int rssi_level;
int snr_level;

// BAD, Grouped
int rssi_level, snr_level;

// BAD, CamelCase
int RssiLevel;
int SnrLevel;

// BAD, mixed case
int rssiLevel;
int snrLevel;
```

Local variables should be located the more closest possible to their usage
location. Initialize the variables on their declaration.

```cpp
int i;
i = f();   // Bad -- initialization separate from declaration.
```

```cpp
int j = g();  // Good -- declaration has initialization.
```

```cpp
std::vector<int> v;
v.push_back(1);  // Prefer initializing using brace initialization.
v.push_back(2);
```

```cpp
std::vector<int> v = {1, 2};  // Good -- v starts initialized.

```

Variables necessary for `if`, `while` and `for` should be declared inside these statements. Example:

```cpp
while (const char* p = strchr(str, '/')) str = p + 1;
```

#### Methods or functions.

1. Use descriptive verbs.
2. Names start with lowercase letters.
3. If the name has various words the first letter of the word should be
uppercase.

 ```cpp
int getTotalNearestNodes();
void getData();
int getFirstItem();
```
#### Classes.

1. We use uppercase letters for every word of the class (`SnakeCase`).

```cpp
class SomeClass { 
public:
    ...
private:
    int m_foo; // Private variables have the m_ prefix
};
```
### Control flow statements.

#### if / if-else.

1. Always use brackets in the `if`/`else`.
2. Put a space between `if` and `()`.

```cpp
// if-else or nested if-else statements ALWAYS put braces
if (foo)
{
    if(foo == 1)
    {
        bar = UP;
    }
    else 
    {
        bar = DOWN;
    }

}
else 
{
    bar = MIDDLE;
}

// only if statement
if (foo) bar = UP;
```

#### while / do-while loops.

- Put a space between `while` and `()`.

```cpp
// while statement
while (foo > 0)
{
    ++bar;
}

// do-while statement
do
{
    ++bar;
}
while (foo > 0);

```
### Forward declarations.

Avoid using forward declarations, include the headers when possible.

1. If using a function declared on a header file, include that file.
2. When using template classes, obligatorily include the header file.

### Inline functions.

Only define inline functions when it's 10 lines or less.

### Namespaces.

Namespace provide a method to avoid name collisions in big codebases.

Namespaces should be used following these rules:

1. All namespaces should be lowercase.
2. Avoid colliding with other namespaces.
3. Finish namespaces with a comment as shown below.

```cpp
// In the .h file
namespace mynamespace {

// All declarations are within the namespace scope.
// Notice the lack of indentation.
class MyClass {
public:
    ...
    void Foo();
};

}  // namespace mynamespace

```

```cpp
// In the .cc file
namespace mynamespace {

// Definition of functions is within scope of the namespace.
void MyClass::Foo() {
    ...
}

} // namespace mynamespace
```

### Working with classes.

#### Structures vs classes.

Use structures only for objects that only have plain-old-data, everything else
is a class.

1. Structures should be used for object that only carry data and have associated
constants, but don't have any other functionality that isn't getting/setting the
members. All fields should be public. Methods should only change the members
value and not behaviour.

2. If you require more functionality, use a class.

3. Naming member variables on classes and structures have different rules.
Structures don't use the `m_` prefix.

#### Functions.

##### Return parameters.

1. Use the return value when possible, otherwise return through paramters using
pointers.
2. The input parameters go first, returns parameters go at the last position.

##### Write short functions.

1. We prefer short and concise functions.
2. Somethings more large functions are needed and appropiate, if a function
is over 40 lines, think if it can be broken in smaller pieces. Divide and
conquer.
3. Some functions are complex and big, don't be afraid to modify them if you
need to suit your needs or to refactor it.

##### Arguments by reference.

1. All parameters passed by reference should use `const`.
2. If the parameter is modified use pointers instead.

##### Function overloading.

1. Use overloaded functions (constructors too) only if the reader looking at
the call site can have a good idea of what is happening.

```cpp
class MyClass {
public:
    void analyze(const std::string &text);
    void analyze(const char *text, size_t textlen);
};

```

3. You can overload a function if the semantics between the variants are the
same.

### Casting.

1. Use C++ style castings:

```cpp
static_cast<float>(double_value)
```

2. Use brackets for arithmetic type conversions.

```cpp 
int64 y = int64 {1} << 42
```

3. Don't use C style casts:

```cpp
int y = (int) x 
```


### Static functions and global functions.

1. Is preferred to put functions that aren't member functions on a namespace.
2. Don't use a class to group static functions.

### `const` keyword.

1. We prefer to use the `const int *`, this form is used through all the
codebase.

### Indentation.

1. Do not use tabs.
2. Use 4 spaces as default identation.

