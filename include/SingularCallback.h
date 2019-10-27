#include <iostream>

/*
This solution is designed to take only one parameter
Usage of the template is very simple. The template itself can be instantiated as an object pointer or just as a simple class.

instantiating the SingularCallBack as an object pointer will allow you to have a vector of callbacks, 
or any other kind of grouping which is highly desirable in event driven programming
*/
template <class Class, typename ReturnType, typename Parameter>
class SingularCallBack
{
public:

   typedef ReturnType (Class::*Method)(Parameter);

   SingularCallBack(Class* class_instance, Method method) : class_instance_(class_instance), method_(method) { }

   
   ReturnType operator()(Parameter parameter)
   {
      return (class_instance_->*method_)(parameter);
   }
   //When being used as an object pointer, 
   //C++ has another painful limitation and that is that the operator() can not be invoked without dereferencing the object pointer
   //a quick and dirty solution was to place an execute() method within the template, 
   //which calls the operator() from within the template itself
   ReturnType execute(Parameter parameter)
   {
      return operator()(parameter);
   }

private:

   Class* class_instance_;
   Method method_;
};
