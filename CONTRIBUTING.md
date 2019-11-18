# Contributing to Turpial firmware

Thanks for taking the time to contribute!

The following is a set of guidelines for contributing to [**Locha**](), [**Turpial**]() or .

#### Table of contents

* [Before starting](#before-starting)

* [How can i contribute?](#how-can-i-contribute)
    * [I just have a question](#i-just-have-a-question)
    * [Suggesting **enhancements**](#suggesting-enhancements)
    * [Reporting **bugs**](#reporting-bugs)
    * [Como hacer un Pull Request](#Como-hacer-un-Pull-Request)

- [Contributing to Turpial firmware](#contributing-to-turpial-firmware)
      - [Table of contents](#table-of-contents)
  - [Before starting](#before-starting)
  - [How can i contribute?](#how-can-i-contribute)
    - [I just have a question](#i-just-have-a-question)
    - [Suggesting enhancements](#suggesting-enhancements)
    - [Reportando un bug](#reportando-un-bug)
  - [Como hacer un Pull Request](#como-hacer-un-pull-request)
    - [Empezar un nuevo cambio](#empezar-un-nuevo-cambio)
    - [Subir tus cambios y hacer Pull Request](#subir-tus-cambios-y-hacer-pull-request)
  - [Styleguide](#styleguide)
    - [Commit messages](#commit-messages)
    - [Code styleguide](#code-styleguide)
      - [Include guards](#include-guards)
      - [Include statements](#include-statements)
      - [Naming rules](#naming-rules)
    - [Common statements](#common-statements)
      - [if / if-else](#if--if-else)
      - [while / do-while loops](#while--do-while-loops)
    - [Indentation](#indentation)
    - [Documentation styleguide](#documentation-styleguide)




## Before starting
Please read our [code of conduct](CODE_OF_CONDUCT.md)

## How can i contribute?

ToDo

### I just have a question
For any question you can send us a message via Twitter @Locha_io and soon through the form you will find on our website **locha.io**

### Suggesting enhancements
ToDo

### Reportando un bug

Puedes abrir un nuevo issue o bug desde una linea o lineas especificas de codigo en un archivo o pull request.

Cuando abrimos un issue o bug desde el codigo , el issue contiene una linea o porcion de codigo que usted seleccione.

Para hacer esto tan solo debera seguir estos pasos:

1. Dentro de GitHub ir hasta la paguina principal del proyecto
2. abrir el branch y archivo al que quiere hacer referencia 
3. para referenciar la linea o lineas de codigo tan solo mantenga presionada la tecla ctrl + click en la linea que desea seleccionar.
4. Cuando termine de seleccionar el codigo que quiere referenciar, click sobre cualquier numero de linea, lo que hara visible un menu de tres puntos, luego seleccionamos referenciar en una nueva issue
5. Le asignamos un titulo al issue y hacemos submit al issue.


## Como hacer un Pull Request

Para cada Pull Request que vayas a realizar tienes que hacer lo siguiente.
### Empezar un nuevo cambio
Antes de empezar a hacer modificaciones ejecuta estos comandos para crear una nueva rama que esté sincronizada con dev:

    git fetch --all # descarga los branch en el repositorio.
    git checkout dev # te cambia a la rama dev en caso de no estar en ella.
    git pull origin dev # para sincronizar el branch dev en local.
    git checkout -b nombredelfeaturequequiereshacer # crea una branch nueva sincronizada con dev.
    git push pr nombredelfeaturequequiereshacer # avienta tu cambio a GitHub

hacer algunos cambios

### Subir tus cambios y hacer Pull Request
Una vez que hiciste los cambios que quieres proponer, realiza los siguientes pasos:
    
    git add .
    git commit -m "Aqui una descripcion de tus cambios".

Configura tus datos de usuario (Sólo necesitas ingresar esta información la primera vez que haces un push):

    git config --global user.email "tu-email@gmail.com"
    git config --global user.name "<tu-usuario>"

Haz push de tu commit a tu fork del repositorio: 
    
    git push nombredelfeaturequequiereshacer # avienta tu cambio a GitHub
    git push -u pr

Ve a la página de GitHub, si vas a tu fork del repositorio, haz clic en el botón Branch y selecciona la rama en que realizaste los cambios nombredelfeaturequequiereshacer. Después presiona el botón Pull request.


## Styleguide

### Commit messages
A commit message must be short, clear and a general description of the changes or improvements.
If a commit includes changes in several files or sections, we can include after the initial message a more extended description of each change.

### Code styleguide

Actuually this project uses the [C++ bitcoin Style Guide](https://github.com/bitcoin/bitcoin/blob/master/doc/developer-notes.md#coding-style-c) for simplicty, it's already established and works very well there and doesn't change very often.

We use the [.clang-format](https://github.com/bitcoin/bitcoin/blob/master/src/.clang-format) file that they provide for automatic code formatting. It's very recommended that you run `clang-format` before you submit a pull request, to make code reviews more easy.

#### Include guards
Local header files must contain an distinctly named include guard to avoid problems with including the same header multiple times, for example:
```cpp
// file: foo.h
#ifndef FOO_H
#define FOO_H
...
#endif // FOO_H
```

#### Include statements

Include statements must be located at the top of the file **only**. By default this statement will go in the .cpp files, not in header files (.h), except when necessary and it should be sorted and grouped.

#### Naming rules

- Use a descriptive name and be consistent in style when write code
- All names should be written in English

**Macros** Use uppercase and underscore
```cpp

#define LOW_NIBBLE(x) (x & 0xf)
```

**Variable names** Use underscore, dont't group variables by types
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

**Methods or functions**  Use descriptive verbs and mixed case starting with lower case.

 ```cpp
 int getTotalNearestNodes();
```

**Classes** Use CamelCase
```cpp
class SomeClass { 
public:
    ...
private:
    int m_foo; // Private variables have the m_ prefix
};
```
### Common statements

#### if / if-else

- Always put braces around the if-else statement or when is nested in another if statement
- Put space between `if` and `()`


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
#### while / do-while loops

- Put space between `while` and `()`
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
### Indentation

- Do not use tabs
- Use 4 spaces as default identation

### Documentation styleguide
ToDo
