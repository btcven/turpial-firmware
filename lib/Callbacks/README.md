## El problema

A diferencia de otros lenguajes como Object Pascal, Ada, Java y C #, C ++ no ofrece una solución nativa al problema de pasar el método de una clase como una función de devolución de llamada. En el lenguaje C, estos se conocen como functores y existen muy comúnmente en muchas aplicaciones controladas por eventos. 

El problema principal se centra en el hecho de que múltiples instancias de una clase particular resultarán en diferentes ubicaciones de memoria para cada instanciación. Esto lleva a la necesidad de tener no solo el puntero del método sino también un puntero a la instancia misma. La definición del problema genera una solución intuitiva que se enmarca en el ámbito de las plantillas y la creación de instancias de tiempo y la especialización.


## La solución

La solución presentada aquí está diseñada para tomar solo un parámetro. Sin embargo, a medida que más compiladores comiencen a implementar correctamente el futuro estándar de C ++, los argumentos de plantilla variados, como las listas de plantillas "..." (Plantillas C ++, La Guía Completa) se convertirán en una realidad que permitirá marcos que puedan atender fácilmente a conjuntos de parámetros arbitrarios. Dicho esto, una vez que se haya presentado la solución de parámetro único, también se discutirá una solución cruda para múltiples parámetros.

```CPP
template <class Class, typename ReturnType, typename Parameter>
class SingularCallBack
{
public:

   typedef ReturnType (Class::*Method)(Parameter);

   SingularCallBack(Class* class_instance, Method method)
   : class_instance_(class_instance),
     method_(method)
   {}

   ReturnType operator()(Parameter parameter)
   {
      return (class_instance_->*method_)(parameter);
   }

   ReturnType execute(Parameter parameter)
   {
      return operator()(parameter);
   }

private:

   Class* class_instance_;
   Method method_;
};

```




El uso de la plantilla es muy simple. La plantilla en sí puede ser instanciada como un puntero de objeto o simplemente como una clase simple. Cuando se usa como puntero de objeto, C ++ tiene otra limitación dolorosa y es que el **operador ()** no se puede invocar sin dereferenciar el puntero de objeto, una solución rápida y sucia fue colocar un método **execute ()** dentro de la plantilla, que llama el **operador ()** desde la propia plantilla. Aparte de ese pequeño problema, instanciar SingularCallBack como un puntero de objeto le permitirá tener un vector de devoluciones de llamada, o cualquier otro tipo de agrupación que sea altamente deseable en la programación controlada por eventos.

Supongamos que existen las siguientes 2 clases, y que queremos tener el ```método B ()``` como nuestro método de devolución de llamada.

```C++
class A
{
public:

   void output()
   {
      std::cout << "I am class A :D" << std::endl;
   }
};

class B
{
public:

   bool methodB(A a)
   {
      a.output();
      return true;
   }
};
```



Del código podemos ver que cuando se invoca el método B () con el parámetro de la clase A, el método B () invocará el método output () en la clase A. La prueba de que la devolución de llamada realmente funcionó es si vemos la línea "I Soy clase A: D "en el stdout.

Hay dos formas en que se puede invocar la devolución de llamada desde un puntero de objeto, el método inicial es desreferenciar el puntero de objeto y ejecutar el método de devolución de llamada (es decir: () operador) la segunda opción es ejecutar el método execute ().

