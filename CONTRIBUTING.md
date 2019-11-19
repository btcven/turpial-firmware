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
    - [Herencia](#herencia)
      - [Pros](#pros)
      - [Cons](#cons)
      - [decision](#decision)
      - [Orden en la declaracion](#orden-en-la-declaracion)
  - [Funciones](#funciones)
    - [Parametros de salida](#parametros-de-salida)
    - [Escribir funciones cortas](#escribir-funciones-cortas)
    - [Argumentos como referencia](#argumentos-como-referencia)
      - [Pros](#pros-1)
      - [Cons](#cons-1)
      - [Decision](#decision)
  - [](#)
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


# Guia de estilos para el codigo fuente

Actualmente este proyecto utiliza la guia de stilos de [google](https://google.github.io/styleguide/cppguide.html) y [.clang-format](https://github.com/bitcoin/bitcoin/blob/master/src/.clang-format) para formatear el documento.

Es altamente recomendado que ejecute [.clang-format] para dar formato al documento antes de hacer un pull request, para hacer la revision del codigo mas facil.

## Commit messages
Un mensaje de commit debe ser corto, claro y con una descripcion de los cambios o mejoras propuestos.
Si el commit incluye cambios en varios archivos o secciones, deberemos incluir un mensaje extendido con una descripcion de los cambios propuestos uno a uno.


## Include guards
Todos los archivos de encabezado deben tener **#define** guardias para evitar la inclusión múltiple. El formato del nombre del símbolo debe ser:
```cpp
// file: foo.h
#ifndef FOO_H
#define FOO_H
...
#endif // FOO_H
```

## Las declaraciones de inclusion
Las declaraciones de inclusion deben ubicarse **unicamente** en la parte superior del archivo de manera predeterminada, esta declaración irá en los archivos .cpp, no en los archivos de encabezado (.h), excepto cuando sea necesario y debe clasificarse y agruparse.


## Reglas de nomenclatura

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
## Sentencias de control

### if / if-else

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
## while / do-while loops

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
## Forward Declarations
Evitar usar declaraciones forward, en lo posible incluya los headers que necesite.

- cuando utilice una funcion declarada dentro de un archivo header, siempre incluya este header
- Cuando utilice una clase template, preferiblemente incluya su archivo header.

## Funciones Inline
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

## Namespaces
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

## Variables locales

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

## Trabajando con clases

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

### Herencia

La composicion es mas apropiada que la herencia. Cuando use la herencia hagala publico

Cuando una subclase hereda de una clase base, incluya las definiciones de todos los datos y operaciones que define la clase base. 
"Herencia de interfaz" es herencia de una clase base abstracta pura (una sin estado o métodos definidos); toda otra herencia es "herencia de implementación".

#### Pros
La herencia de implementación reduce el tamaño del código al reutilizar el código de la clase base, ya que se especializa en un tipo existente. Como la herencia es una declaración en tiempo de compilación, usted y el compilador pueden comprender la operación y detectar errores. La herencia de interfaz se puede utilizar para exigir mediante programación que una clase exponga una API particular. Nuevamente, el compilador puede detectar errores, en este caso, cuando una clase no define un método necesario de la API.

#### Cons
Para la herencia de implementación, debido a que el código que implementa una subclase se distribuye entre la base y la subclase, puede ser más difícil entender una implementación. La subclase no puede anular funciones que no son virtuales, por lo que la subclase no puede cambiar la implementación.

La herencia múltiple es especialmente problemática, ya que a menudo impone una sobrecarga de rendimiento más alta (de hecho, la caída del rendimiento de la herencia única a la herencia múltiple a menudo puede ser mayor que la caída del rendimiento del despacho ordinario a virtual), y porque corre el riesgo de llevar a "diamante" "patrones de herencia, que son propensos a la ambigüedad, la confusión y los errores directos.

#### decision 
Toda herencia debe ser pública. Si desea hacer una herencia privada, debe incluir una instancia de la clase base como miembro en su lugar.

No abusar de la herencia de implementación. La composición es a menudo más apropiada. Trate de restringir el uso de la herencia.

Limite el uso de protegido a las funciones miembro a las que se deba acceder desde subclases. Tenga en cuenta que los miembros de datos deben ser privados.

#### Orden en la declaracion

- Agrupe declaraciones similares juntas, colocando partes públicas antes.

- Una definición de clase generalmente debe comenzar con una sección  pública: seguida de protegida: luego privada Omita las secciones que estarían vacías.

- Dentro de cada sección, generalmente prefieren agrupar tipos similares de declaraciones, y generalmente prefieren el siguiente orden: tipos (incluidos typedef, using y estructuras anidadas y clases), constantes, funciones de fábrica, constructores, operadores de asignación, destructor, todos los demás métodos, miembros de datos.

- No coloque definiciones de métodos grandes **inline** en la definición de clase. Por lo general, solo los métodos triviales o críticos para el rendimiento, y muy cortos, se pueden definir **inline**. Ver Funciones [Inline](#funciones-inline) para más detalles.


## Funciones

### Parametros de salida

- La salida de una función C ++ se proporciona naturalmente a través de un valor de retorno y, a veces, a través de parámetros de salida.

- se Prefiere usar valores de retorno sobre los parámetros de salida: mejoran la legibilidad y, a menudo, proporcionan el mismo o mejor rendimiento. Si se utilizan parámetros de solo salida, deben aparecer después de los parámetros de entrada.

- Los parámetros son entrada a la función, salida de la función o ambos. Los parámetros de entrada suelen ser valores o referencias constantes, mientras que los parámetros de salida y entrada / salida serán punteros a no constantes.

- Al ordenar parámetros de función, coloque todos los parámetros de solo entrada antes de cualquier parámetro de salida. En particular, no agregue nuevos parámetros al final de la función solo porque son nuevos; coloque nuevos parámetros de solo entrada antes de los parámetros de salida.

Esta no es una regla estricta. Los parámetros que son tanto de entrada como de salida (a menudo clases / estructuras) enturbian las aguas y, como siempre, la coherencia con las funciones relacionadas puede requerir que doble la regla.

### Escribir funciones cortas 

- Se prefiere funciones pequeñas y enfocadas.

- Reconocemos que las funciones largas a veces son apropiadas, por lo que no se establece un límite estricto en la longitud de las funciones. Si una función supera las 40 líneas, piense si se puede dividir sin dañar la estructura del programa.

- Incluso si su función larga funciona perfectamente ahora, alguien que la modifique en unos meses puede agregar un nuevo comportamiento. Esto podría provocar errores que son difíciles de encontrar. Mantener sus funciones cortas y simples hace que sea más fácil para otras personas leer y modificar su código. Las funciones pequeñas también son más fáciles de probar.

- Podrías encontrar funciones largas y complicadas cuando trabajas con algún código. No se deje intimidar modificando el código existente: si trabajar con una función de este tipo resulta difícil, encuentra que los errores son difíciles de depurar, o si desea usar una parte de él en varios contextos diferentes, considere dividir la función en una más pequeña y piezas más manejables.

### Argumentos como referencia

- Todos los parámetros pasados por referencia deben etiquetarse const.
- En C, si una función necesita modificar una variable, el parámetro debe usar un puntero, por ejemplo, ```cpp int foo (int * pval); ```
En C ++, la función puede declarar alternativamente un parámetro de referencia: int foo (int & val).

#### Pros
Definir un parámetro como referencia evita un código feo como ```cpp (* pval) ++```. Necesario para algunas aplicaciones, como los constructores de copias. Deja en claro, a diferencia de los punteros, que un puntero nulo no es un valor posible.

#### Cons
Las referencias pueden ser confusas, ya que tienen una sintaxis de valor pero una semántica de puntero.

#### Decision
Dentro de las listas de parámetros de funciones, todas las referencias deben ser constantes:
```cpp void Foo(const std::string &in, std::string *out);```
De hecho, es una convención muy fuerte en el código de Google que los argumentos de entrada son valores o referencias constantes, mientras que los argumentos de salida son punteros. Los parámetros de entrada pueden ser punteros constantes, pero nunca permitimos parámetros de referencia no constantes, excepto cuando lo requiera la convención, por ejemplo, swap ().

Sin embargo, hay algunos casos en los que es preferible usar const T * a const T & para los parámetros de entrada. Por ejemplo:

     Desea pasar un puntero nulo.
     La función guarda un puntero o referencia a la entrada.

Recuerde que la mayoría de las veces los parámetros de entrada se especificarán como const T &. El uso de const T * en su lugar comunica al lector que la entrada se trata de alguna manera de manera diferente. Entonces, si elige const T * en lugar de const T &, hágalo por una razón concreta; de lo contrario, probablemente confundirá a los lectores haciéndolos buscar una explicación que no existe.

## Sobre carga de funciones

- Use funciones sobrecargadas (incluidos los constructores) solo si un lector que mira un sitio de llamada puede tener una buena idea de lo que está sucediendo sin tener que averiguar primero qué sobrecarga se está llamando.

```cpp
class MyClass {
 public:
  void Analyze(const std::string &text);
  void Analyze(const char *text, size_t textlen);
}; 

```
- La sobrecarga puede hacer que el código sea más intuitivo al permitir que una función con nombre idéntico tome diferentes argumentos. Puede ser necesario para el código con plantilla, y puede ser conveniente para los visitantes.

- La sobrecarga basada en la calificación constante o de referencia puede hacer que el código de utilidad sea más usable, más eficiente o ambos.

- Si una función está sobrecargada solo por los tipos de argumento, un lector puede tener que comprender las complejas reglas de coincidencia de C ++ para saber qué está sucediendo. Además, muchas personas están confundidas por la semántica de la herencia si una clase derivada anula solo algunas de las variantes de una función.

Puede sobrecargar una función cuando no hay diferencias semánticas entre las variantes. Estas sobrecargas pueden variar en tipos, calificadores o recuento de argumentos. Sin embargo, un lector de dicha llamada no necesita saber qué miembro del conjunto de sobrecarga se elige, solo que se está llamando a algo del conjunto. Si puede documentar todas las entradas en el conjunto de sobrecarga con un solo comentario en el encabezado, es una buena señal de que es un conjunto de sobrecarga bien diseñado.


## 
### Indentation

- Do not use tabs
- Use 4 spaces as default identation

### Documentation styleguide
ToDo


