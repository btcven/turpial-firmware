#include <iostream>
#include "Callback.h"

/*
This class is a static one to be used inside data transfer objects to reimplement
the functionality inside DTO methids to serialized and deserialize the information inside
*/
class TestClass
{
public:

  TestClass(unsigned int id) : id_(id) { }

  bool AMethod(const std::string& str)
  {
     std::cout << "AClass[" << id_ << "]: " << str << std::endl;
     std::cout.flush();
     return true;
  }

  bool method4(int a, int b, int c, int d)
  {
     std::cout << "Method - 4" << std::endl;
     return true;
  }

  bool method3(int a, int b, int c)
  {
     std::cout << "Method - 3" << std::endl;
     return true;
  }

  bool method2(int a, int b)
  {
     std::cout << "Method - 2" << std::endl;
     return true;
  }

  bool method1(int a)
  {
     std::cout << "Method - 1" << std::endl;
     return true;
  }

  bool method0()
  {
     std::cout << "Method - 0" << std::endl;
     return true;
  }

  void methodv()
  {
     std::cout << "Method - v" << std::endl;
  }

private:

 unsigned int id_;
};
 