# Contributing to Turpial firmware

Gracias por tomarte el tiempo de contribuir!.

El siguiente es un conjunto de pautas para contribuir a  [**Locha**](), [**Turpial**]().

#### Table of contents

* [Antes de empezar](#antes-de-empezar)

* [How can i contribute?](#how-can-i-contribute)
    * [I just have a question](#i-just-have-a-question)
    * [Suggesting **enhancements**](#suggesting-enhancements)
    * [Reporting **bugs**](#reporting-bugs)
    * [Como hacer un Pull Request](#Como-hacer-un-Pull-Request)

- [Contributing to Turpial firmware](#contributing-to-turpial-firmware)
      - [Table of contents](#table-of-contents)
  - [Antes de empezar](#antes-de-empezar)
  - [Como puedo contribuir?](#como-puedo-contribuir)
    - [I just have a question](#i-just-have-a-question)
    - [Suggesting enhancements](#suggesting-enhancements)
    - [Reportando un bug](#reportando-un-bug)
  - [Como hacer un Pull Request](#como-hacer-un-pull-request)
    - [Empezar un nuevo cambio](#empezar-un-nuevo-cambio)
    - [Subir tus cambios y hacer Pull Request](#subir-tus-cambios-y-hacer-pull-request)
  - [Guia de estilos para el codigo fuente](#guia-de-estilos-para-el-codigo-fuente)
    - [Commit messages](#commit-messages)
    - [Include guards](#include-guards)
      - [Las declaraciones de inclusion](#las-declaraciones-de-inclusion)
      - [Reglas de nomenclatura](#reglas-de-nomenclatura)
    - [Sentencias de control](#sentencias-de-control)
      - [if / if-else](#if--if-else)
      - [while / do-while loops](#while--do-while-loops)
    - [Forward Declarations](#forward-declarations)
    - [Funciones Inline](#funciones-inline)
    - [Orden en los que se deben incluir los archivos header](#orden-en-los-que-se-deben-incluir-los-archivos-header)
    - [Namespaces](#namespaces)
    - [Variables locales](#variables-locales)
    - [Trabajando con clases](#trabajando-con-clases)
    - [Trabajando en constructores](#trabajando-en-constructores)
    - [estructuras vs clases](#estructuras-vs-clases)
    - [Indentation](#indentation)
    - [Documentation styleguide](#documentation-styleguide)




## Antes de empezar 
Please read our [code of conduct](CODE_OF_CONDUCT.md)

## Como puedo contribuir?


Nos encantaría aceptar sus parches y contribuciones a este proyecto. Solo hay algunas pautas pequeñas que debes seguir.



### I just have a question
Para alguna pregunta o duda puede escribirnos via Twitter @Locha_io 
a traves del formulario que encontrara en nuestro sitio web **locha.io**

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


## Guia de estilos para el codigo fuente

Actualmente este proyecto utiliza la guia de stilos de [google](https://google.github.io/styleguide/cppguide.html) y [.clang-format](https://github.com/bitcoin/bitcoin/blob/master/src/.clang-format) para formatear el documento.

Es altamente recomendado que ejecute [.clang-format] para dar formato al documento antes de hacer un pull request, para hacer la revision del codigo mas facil.

### Commit messages
Un mensaje de commit debe ser corto, claro y con una descripcion de los cambios o mejoras propuestos.
Si el commit incluye cambios en varios archivos o secciones, deberemos incluir un mensaje extendido con una descripcion de los cambios propuestos uno a uno.


### Include guards
Todos los archivos de encabezado deben tener **#define** guardias para evitar la inclusión múltiple. El formato del nombre del símbolo debe ser:
```cpp
// file: foo.h
#ifndef FOO_H
#define FOO_H
...
#endif // FOO_H
```

#### Las declaraciones de inclusion
Las declaraciones de inclusion deben ubicarse **unicamente** en la parte superior del archivo de manera predeterminada, esta declaración irá en los archivos .cpp, no en los archivos de encabezado (.h), excepto cuando sea necesario y debe clasificarse y agruparse.


#### Reglas de nomenclatura

- Use un nombre descriptivo y que sea coherente con el estilo cuando escriba código.
- todos los nombres deberan ser escritos en **ingles**.

**Macros** Realmente no vas a definir una macro, ¿verdad? Si lo hace, son así: MY_MACRO_THAT_SCARES_SMALL_CHILDREN_AND_ADULTS_ALIKE.

Por favor vea la descripción de macros; en general no se deben usar macros. Sin embargo, si son absolutamente necesarios, deben nombrarse con todas las mayúsculas y guiones bajos.


```cpp
#define ROUND(x) ...
#define PI_ROUNDED 3.0
#define LOW_NIBBLE(x) (x & 0xf)
```

**Nombres de variable** Usar guion bajo, no agrupar variables por tipo

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

**Metodos o funciones** Use verbos descriptivos y sus nombres empezando con minusculas, si el nombre es compuesto se continua con mayusculas.

 ```cpp
 int getTotalNearestNodes();
 void getData();
 int getFirstItem();
```
**Classes** Para el nombre de las clases usamos unicamente mayusculas en el inicio de cada palabra, si el nombre es compuesto.

```cpp
class SomeClass { 
public:
    ...
private:
    int m_foo; // Private variables have the m_ prefix
};
```
### Sentencias de control

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

- Poner espacios entre `while` and `()`
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
### Forward Declarations
Evitar usar declaraciones forward, en lo posible incluya los headers que necesite.

- cuando utilice una funcion declarada dentro de un archivo header, siempre incluya este header
- Cuando utilice una clase template, preferiblemente incluya su archivo header.

### Funciones Inline
Defina funciones inline solo cuando sean de 10 o menos lineas de codigo.

### Orden en los que se deben incluir los archivos header
Incluya los archivos header en el siguiente orden: 
- C system headers
- C++ standard library headers
- headers de librerias
- headers del proyecto

Todos los archivos headers deben aparecer de forma descendente 

```cpp
    C system headers //(para ser precisos archivos que se llaman con < >), <stdlib.h>.
    A blank line
    C++ standard library headers // (sin la extension del archivo), e.g. <algorithm>, <cstddef>.
    A blank line
    Otras librerias .h // archivos dentro de lib e include
    el proyecto  .h files. 

```

### Namespaces
Los namespaces proveen un metodo para prevenir conflictos de nombres en programas grandes.

Los namespaces deberan ser usados siguiendo las siguientes reglas: 

- Los nombres de los espacios de nombres son en minúsculas. 
- Los nombres de espacios de nombres de nivel superior se basan en el nombre del proyecto. 
- Evite colisiones entre espacios de nombres anidados y espacios de nombres de nivel superior bien conocidos.
- El nombre de un espacio de nombres de nivel superior generalmente debe ser el nombre del proyecto o equipo cuyo código está contenido en ese espacio de nombres. 
- Terminar los namespaces con comentarios como se muestra a continuacion.

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

}  // namespace mynamespace
```

### Variables locales

Coloque las variables de una función en el alcance más estrecho posible e inicialice las variables en la declaración.


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

Las variables necesarias para las declaraciones if, while y for normalmente deberían declararse dentro de esas declaraciones, de modo que dichas variables se limiten a esos ámbitos. P.ej.:

```cpp
while (const char* p = strchr(str, '/')) str = p + 1;
```

Hay una advertencia: si la variable es un objeto, su constructor se invoca cada vez que entra en el ámbito y se crea, y su destructor se invoca cada vez que sale del ámbito.
```cpp
for(int i = 0; i < 10000000; i++) {
    Foo f;  // My ctor and dtor get called 1000000 times each.
    f.DoSomething(i);
}
```
Puede ser más eficiente declarar dicha variable utilizada fuera del ciclo

```cpp
Foo f;  // My ctor and dtor get called once each.
for (int i = 0; i < 1000000; ++i) {
  f.DoSomething(i);
}
```

### Trabajando con clases

Las clases son la unidad fundamental de código en C ++; Naturalmente, los usamos ampliamente;Esta sección enumera los principales y qué no debe hacer al escribir una clase.

### Trabajando en constructores

- Evite llamadas a métodos virtuales en constructores.

- Es posible realizar una inicialización arbitraria en el cuerpo del constructor.

- No hay que preocuparse por si la clase se ha inicializado o no; Los objetos que se inicializan completamente mediante una llamada de constructor pueden ser constantes y también pueden ser más fáciles de usar con contenedores o algoritmos estándar.

- No puede tomar la dirección de un constructor, por lo que cualquier trabajo que se realice en el constructor no puede transferirse fácilmente a, por ejemplo, otro hilo.

### estructuras vs clases
Use estructuras solo para objetos pasivos que portan datos, todo lo demas es una clase.

- Las palabras clave struct y class se comportan casi de manera idéntica en C ++. Agregamos nuestros propios significados semánticos a cada palabra clave, por lo que debe usar la palabra clave adecuada para el tipo de datos que está definiendo.

- Las estructuras deben usarse para objetos pasivos que transportan datos y pueden tener constantes asociadas, pero carecen de cualquier otra funcionalidad que no sea acceder / configurar los miembros de datos. Todos los campos deben ser públicos y acceder directamente en lugar de a través de métodos getter / setter. La estructura no debe tener invariantes que impliquen relaciones entre diferentes campos, ya que el acceso directo del usuario a esos campos puede romper esos invariantes. Los métodos no deberían proporcionar comportamiento, sino que solo deberían usarse para configurar los miembros de datos, por ejemplo, constructor, destructor, Initialize (), Reset ().

- Si se requiere más funcionalidad o invariantes, una clase es más apropiada. En caso de duda, conviértalo en una clase.

- Para mantener la coherencia con STL, puede usar struct en lugar de class para tipos sin estado, como rasgos, metafunciones de plantilla.

- Tenga en cuenta que las variables miembro en estructuras y clases tienen diferentes reglas de nomenclatura.
### Indentation

- Do not use tabs
- Use 4 spaces as default identation

### Documentation styleguide
ToDo


