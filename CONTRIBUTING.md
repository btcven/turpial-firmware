# Contributing to Turpial firmware

Gracias por tomarte el tiempo de contribuir!.

El siguiente es un conjunto de pautas para contribuir a  [**Locha**](), [**Turpial**]().

#### Table de contenido

- [Contributing to Turpial firmware](#contributing-to-turpial-firmware)
      - [Table de contenido](#table-de-contenido)
  - [Antes de empezar](#antes-de-empezar)
  - [Como puedo contribuir?](#como-puedo-contribuir)
    - [Tengo una pregunta](#tengo-una-pregunta)
    - [Sugerencia de mejoras](#sugerencia-de-mejoras)
    - [Reportando un bug](#reportando-un-bug)
  - [Como hacer un Pull Request](#como-hacer-un-pull-request)
    - [Empezar un nuevo cambio](#empezar-un-nuevo-cambio)
    - [Subir tus cambios y hacer Pull Request](#subir-tus-cambios-y-hacer-pull-request)
- [Guia de estilos para el codigo fuente](#guia-de-estilos-para-el-codigo-fuente)
  - [Objetivos de la guia de estilos](#objetivos-de-la-guia-de-estilos)
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
      - [Ventajas](#ventajas)
      - [Desventajas](#desventajas)
      - [Decision](#decision)
      - [Orden en la declaracion](#orden-en-la-declaracion)
    - [Funciones](#funciones)
    - [Parametros de salida](#parametros-de-salida)
    - [Escribir funciones cortas](#escribir-funciones-cortas)
    - [Argumentos como referencia](#argumentos-como-referencia)
      - [Ventajas](#ventajas-1)
      - [Desventajas](#desventajas-1)
      - [Decision](#decision-1)
    - [Donde poner la palabra clave **const**](#donde-poner-la-palabra-clave-const)
    - [Indentation](#indentation)
    - [Documentation styleguide](#documentation-styleguide)
    
    - [Orden en los que se deben incluir los archivos header](#orden-en-los-que-se-deben-incluir-los-archivos-header)
  - [Namespaces](#namespaces)
  - [Variables locales](#variables-locales)
  - [Trabajando con clases](#trabajando-con-clases)
    - [Trabajando en constructores](#trabajando-en-constructores)
    - [estructuras vs clases](#estructuras-vs-clases)
    - [Herencia](#herencia)
      - [Ventajas](#ventajas)
      - [Desventajas](#desventajas)
      - [Decision](#decision)
      - [Orden en la declaracion](#orden-en-la-declaracion)
  - [Funciones](#funciones)
    - [Parametros de salida](#parametros-de-salida)
    - [Escribir funciones cortas](#escribir-funciones-cortas)
    - [Argumentos como referencia](#argumentos-como-referencia)
      - [Ventajas](#ventajas-1)
      - [Desventajas](#desventajas-1)
      - [Decision](#decision-1)
  - [Preincremento y Predecremento](#preincremento-y-predecremento)
    - [Indentation](#indentation)
    - [Documentation styleguide](#documentation-styleguide)
  -[Orden en los que se deben incluir los archivos header](#orden-en-los-que-se-deben-incluir-los-archivos-header)
  - [Namespaces](#namespaces)
  - [Variables locales](#variables-locales)
  - [Trabajando con clases](#trabajando-con-clases)
    - [Trabajando en constructores](#trabajando-en-constructores)
    - [estructuras vs clases](#estructuras-vs-clases)
    - [Herencia](#herencia)
      - [Ventajas](#ventajas)
      - [Desventajas](#desventajas)
      - [Decision](#decision)
      - [Orden en la declaracion](#orden-en-la-declaracion)
  - [Funciones](#funciones)
    - [Parametros de salida](#parametros-de-salida)
    - [Escribir funciones cortas](#escribir-funciones-cortas)
    - [Argumentos como referencia](#argumentos-como-referencia)
      - [Ventajas](#ventajas-1)
      - [Desventajas](#desventajas-1)
      - [Decision](#decision-1)
  - [Preincremento y Predecremento](#preincremento-y-predecremento)
    - [Indentation](#indentation)
    - [Documentation styleguide](#documentation-styleguide)
     [Orden en los que se deben incluir los archivos header](#orden-en-los-que-se-deben-incluir-los-archivos-header)
  - [Namespaces](#namespaces)
  - [Variables locales](#variables-locales)
  - [Trabajando con clases](#trabajando-con-clases)
    - [Trabajando en constructores](#trabajando-en-constructores)
    - [estructuras vs clases](#estructuras-vs-clases)
    - [Herencia](#herencia)
      - [Ventajas](#ventajas)
      - [Desventajas](#desventajas)
      - [Decision](#decision)
      - [Orden en la declaracion](#orden-en-la-declaracion)
  - [Funciones](#funciones)
    - [Parametros de salida](#parametros-de-salida)
    - [Escribir funciones cortas](#escribir-funciones-cortas)
    - [Argumentos como referencia](#argumentos-como-referencia)
      - [Ventajas](#ventajas-1)
      - [Desventajas](#desventajas-1)
      - [Decision](#decision-1)
  - [Preincremento y Predecremento](#preincremento-y-predecremento)
    - [Indentation](#indentation)
    - [Documentation styleguide](#documentation-styleguide)




## Antes de empezar 
Please read our [code of conduct](CODE_OF_CONDUCT.md)

## Como puedo contribuir?
Nos encantaría aceptar sus parches y contribuciones a este proyecto. Solo hay algunas pautas pequeñas que debes seguir.

### Tengo una pregunta
Para alguna pregunta o duda puede escribirnos via Twitter @Locha_io 
a traves del formulario que encontrara en nuestro sitio web **locha.io**

### Sugerencia de mejoras
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

## Objetivos de la guia de estilos

Los objetivos de la guía de estilo tal como los vemos actualmente se basan en optimizar para el lector y no para el escritor; Se espera que nuestra base de codigo (y la mayoria de los componentes individuales) continuen durante bastante tiempo

- Optimizar para el lector, no para el escritor
Se espera que nuestra base de código (y la mayoría de los componentes individuales que se le envíen) continúe durante bastante tiempo. Como resultado, se dedicará más tiempo a leer la mayor parte de nuestro código que a escribirlo. Elegimos explícitamente optimizar la experiencia de nuestro ingeniero de software promedio leyendo, manteniendo y depurando código en nuestra base de código en lugar de facilitar la escritura de dicho código. "Dejar un rastro para el lector" es un subpunto particularmente común de este principio: cuando algo sorprendente o inusual está sucediendo en un fragmento de código (por ejemplo, transferencia de propiedad del puntero), dejando pistas textuales para el lector en el punto de uso es valioso (std :: unique_ptr demuestra la transferencia de propiedad sin ambigüedades en el sitio de la llamada).

- Sea consistente con el código existente
El uso de un estilo consistentemente a través de nuestra base de código nos permite enfocarnos en otros problemas (más importantes). La consistencia también permite la automatización: las herramientas que formatean su código o ajustan sus #incluye solo funcionan correctamente cuando su código es consistente con las expectativas de las herramientas. En muchos casos, las reglas que se atribuyen a "Sea coherente" se reducen a "Simplemente elija una y deje de preocuparse por eso"; El valor potencial de permitir flexibilidad en estos puntos se ve compensado por el costo de hacer que las personas discutan sobre ellos.



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

- Siempre ponga llaves alrededor de la instruccion **if-else** o cuando este anidada en otra instruccion **if**
- Puonga espacion entre  **`if`** y **`()`**


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

- Poner espacios entre `while` y `()`
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

Las variables necesarias para las declaraciones **if**, **while** y **for** normalmente deberían declararse dentro de esas declaraciones, de modo que dichas variables se limiten a esos ámbitos. P.ej.:

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

#### Ventajas
La herencia de implementación reduce el tamaño del código al reutilizar el código de la clase base, ya que se especializa en un tipo existente. Como la herencia es una declaración en tiempo de compilación, usted y el compilador pueden comprender la operación y detectar errores. La herencia de interfaz se puede utilizar para exigir mediante programación que una clase exponga una API particular. Nuevamente, el compilador puede detectar errores, en este caso, cuando una clase no define un método necesario de la API.

#### Desventajas
Para la herencia de implementación, debido a que el código que implementa una subclase se distribuye entre la base y la subclase, puede ser más difícil entender una implementación. La subclase no puede anular funciones que no son virtuales, por lo que la subclase no puede cambiar la implementación.

La herencia múltiple es especialmente problemática, ya que a menudo impone una sobrecarga de rendimiento más alta (de hecho, la caída del rendimiento de la herencia única a la herencia múltiple a menudo puede ser mayor que la caída del rendimiento del despacho ordinario a virtual), y porque corre el riesgo de llevar a "diamante" "patrones de herencia, que son propensos a la ambigüedad, la confusión y los errores directos.

#### Decision 
Toda herencia debe ser pública. Si desea hacer una herencia privada, debe incluir una instancia de la clase base como miembro en su lugar.

No abusar de la herencia de implementación. La composición es a menudo más apropiada. Trate de restringir el uso de la herencia.

Limite el uso de protegido a las funciones miembro a las que se deba acceder desde subclases. Tenga en cuenta que los miembros de datos deben ser privados.

#### Orden en la declaracion

- Agrupe declaraciones similares juntas, colocando partes públicas antes.

- Una definición de clase generalmente debe comenzar con una sección  pública: seguida de protegida: luego privada Omita las secciones que estarían vacías.

- Dentro de cada sección, generalmente prefieren agrupar tipos similares de declaraciones, y generalmente prefieren el siguiente orden: tipos (incluidos typedef, using y estructuras anidadas y clases), constantes, funciones de fábrica, constructores, operadores de asignación, destructor, todos los demás métodos, miembros de datos.

- No coloque definiciones de métodos grandes **inline** en la definición de clase. Por lo general, solo los métodos triviales o críticos para el rendimiento, y muy cortos, se pueden definir **inline**. Ver Funciones [Inline](#funciones-inline) para más detalles.


### Funciones

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

- Todos los parámetros pasados por referencia deben etiquetarse **const**.
- En C, si una función necesita modificar una variable, el parámetro debe usar un puntero, por ejemplo, ```int foo (int * pval); ```
En C ++, la función puede declarar alternativamente un parámetro de referencia: int foo (int & val).

#### Ventajas
Definir un parámetro como referencia evita un código feo como ```(* pval) ++```. Necesario para algunas aplicaciones, como los constructores de copias. Deja en claro, a diferencia de los punteros, que un puntero nulo no es un valor posible.

#### Desventajas
Las referencias pueden ser confusas, ya que tienen una sintaxis de valor pero una semántica de puntero.

#### Decision
Dentro de las listas de parámetros de funciones, todas las referencias deben ser constantes:
```void Foo(const std::string &in, std::string *out);```
De hecho, es una convención muy fuerte en el código de Google que los argumentos de entrada son valores o referencias constantes, mientras que los argumentos de salida son punteros. Los parámetros de entrada pueden ser punteros constantes, pero nunca permitimos parámetros de referencia no constantes, excepto cuando lo requiera la convención, por ejemplo, swap ().

Sin embargo, hay algunos casos en los que es preferible usar const T * a const T & para los parámetros de entrada. Por ejemplo:

     Desea pasar un puntero nulo.
     La función guarda un puntero o referencia a la entrada.

Recuerde que la mayoría de las veces los parámetros de entrada se especificarán como const T &. El uso de const T * en su lugar comunica al lector que la entrada se trata de alguna manera de manera diferente. Entonces, si elige const T * en lugar de const T &, hágalo por una razón concreta; de lo contrario, probablemente confundirá a los lectores haciéndolos buscar una explicación que no existe.

### Sobre carga de funciones

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


### Casting

Use conversiones de estilo C++ como 
```cpp 
static_cast <float> (double_value)
```
O inicialice llaves para la conversión de tipos aritméticos como 

```cpp 
int64 y = int64 {1} << 42
```
No use formatos de conversión como 
```cpp
int y = (int) x 
```

```cpp
int y = int (x) //pero este último está bien cuando se invoca un constructor de un tipo de clase
```

- Use la inicialización de llaves para convertir tipos aritméticos (por ejemplo, ```int64 {x})```. Este es el enfoque más seguro porque el código no se compilará si la conversión puede provocar la pérdida de información. La sintaxis también es concisa.

- Use static_cast como el equivalente de una conversión de estilo C que realiza la conversión de valor, cuando necesita convertir explícitamente un puntero desde una clase a su superclase, o cuando necesita emitir explícitamente un puntero desde una superclase a una subclase. En este último caso, debe asegurarse de que su objeto sea realmente una instancia de la subclase.

- Use const_cast para eliminar el calificador const.


### Preincremento y Predecremento

Utilice la forma de prefijo ``` (++ i) ``` de los operadores de incremento y decremento con iteradores y otros objetos de plantilla.

Cuando una variable se incrementa (++ i o i ++) o disminuye (--i o i--) y el valor de la expresión no se usa, uno debe decidir si preincremento (decremento) o postincremento (decremento).

Cuando se ignora el valor de retorno, la forma "pre" (++ i) nunca es menos eficiente que la forma "post" (i ++) y, a menudo, es más eficiente. Esto se debe a que el incremento posterior (o decremento) requiere que se haga una copia de i, que es el valor de la expresión. Si soy un iterador u otro tipo no escalar, copiarlo podría ser costoso. Dado que los dos tipos de incremento se comportan de la misma manera cuando se ignora el valor, ¿por qué no siempre pre-incrementar?

La tradición se desarrolló, en C, de usar post-incremento cuando no se usa el valor de expresión, especialmente en for loops. Algunos encuentran que el incremento posterior es más fácil de leer, ya que el "asunto" (i) precede al "verbo" (++), al igual que en inglés.

Para valores escalares simples (no objeto) no hay razón para preferir una forma y permitimos cualquiera. Para iteradores y otros tipos de plantillas, use pre-incremento.

### No miembros, miembros estaticos y funciones globales

- Se prefiere colocar funciones que no sean miembros en un espacio de nombres; usar funciones completamente globales raramente. 

- No use una clase simplemente para agrupar funciones estáticas. Los métodos estáticos de una clase generalmente deben estar estrechamente relacionados con las instancias de la clase o los datos estáticos de la clase.

#### Ventajas
Las funciones de miembro no miembro y estático pueden ser útiles en algunas situaciones. Poner funciones que no sean miembros en un espacio de nombres evita contaminar el espacio de nombres global.

#### Desventajas
- Las funciones de miembro no miembro y estático pueden tener más sentido como miembros de una nueva clase, especialmente si acceden a recursos externos o tienen dependencias significativas.

#### Decision
- A veces es útil definir una función no vinculada a una instancia de clase. Tal función puede ser un miembro estático o una función no miembro. 

- Las funciones que no son miembros no deben depender de variables externas, y casi siempre deben existir en un espacio de nombres. 

- No cree clases solo para agrupar funciones miembro estáticas; Esto no es diferente a simplemente dar a los nombres de funciones un prefijo común, y tal agrupación generalmente es innecesaria de todos modos.

Si define una función que no es miembro y solo es necesaria en su archivo .cc, use un enlace interno para limitar su alcance.


## Control de acceso
Haga que los miembros de datos de las clases sean privados, a menos que sean constantes. Esto simplifica el razonamiento sobre invariantes.


### Uso de constantes

- En las API, use **const** siempre que tenga sentido. **constexpr** es una mejor opción para algunos usos de const.

- Las variables y parámetros declarados pueden ir precedidos por la palabra clave **const** para indicar que las variables no se modifican por ejemplo, ```const int foo```. 
- Las funciones de clase pueden tener el calificador const para indicar que la función no cambia el estado de las variables miembro de la clase por ejemplo, 
```cpp
class Foo {
    int Bar (char c) const;
};
```
- Es más fácil para las personas entender cómo se utilizan las variables. 
- Permite al compilador hacer una mejor verificación de tipo y posiblemente, generar un mejor código.
- Ayuda a las personas a convencerse de la corrección del programa porque saben que las funciones que llaman están limitadas en cómo pueden modificar sus variables. 
- Ayuda a las personas a saber qué funciones son seguras de usar sin bloqueos en programas multiproceso.

**const** es viral: si pasa una variable const a una función, esa función debe tener const en su prototipo (o la variable necesitará un const_cast). Esto puede ser un problema particular al llamar a funciones de biblioteca.

Recomendamos encarecidamente utilizar const en las API (es decir, en parámetros de función, métodos y variables no locales) siempre que sea significativo y preciso. Esto proporciona documentación consistente, mayormente verificada por el compilador, de qué objetos puede mutar una operación. Tener una forma consistente y confiable de distinguir las lecturas de las escrituras es fundamental para escribir código seguro para subprocesos, y también es útil en muchos otros contextos. En particular:

- Si una función garantiza que no modificará un argumento pasado por referencia o por puntero, el parámetro de función correspondiente debe ser una referencia a const (const T &) o un puntero a const (const T *), respectivamente.

- Para un parámetro de función pasado por valor, const no tiene efecto en el llamador, por lo tanto, no se recomienda en las declaraciones de función. 

- Declare que los métodos son constantes a menos que alteren el estado lógico del objeto (o permitan que el usuario modifique ese estado, por ejemplo, devolviendo una referencia no constante, pero eso es raro), o no se pueden invocar de manera segura al mismo tiempo.

- El uso de const en variables locales no se recomienda ni se desaconseja.

### Donde poner la palabra clave **const**

Algunas personas prefieren la forma ```int const * foo``` a ```const int * foo```. Argumentan que esto es más legible porque es más consistente: mantiene la regla de que const siempre sigue el objeto que está describiendo. Sin embargo, este argumento de coherencia no se aplica en bases de código con pocas expresiones de puntero profundamente anidadas, ya que la mayoría de las expresiones constantes tienen solo una constante, y se aplica al valor subyacente. En tales casos, no hay consistencia para mantener. Poner el const primero es posiblemente más legible, ya que sigue el inglés al poner el "adjetivo" (const) antes del "sustantivo" (int).

Dicho esto, si bien alentamos a poner const primero, no lo requerimos. ¡Pero sea consistente con el código que lo rodea!


### Indentation

- Do not use tabs
- Use 4 spaces as default identation

### Documentation styleguide
ToDo


