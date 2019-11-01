#include <stdio.h>
#include <iostream>
#include <cstring>


//This class is a static one to be used inside data transfer objects to reimplement
//the functionality inside DTO methods to serialized and deserialize the information inside

template <typename POD>
class SerializablePOD
{
public:

    static size_t serialize_size(POD str) {
        return sizeof(str);
    }

    static char* serialize( char* target, POD value ) {
        memcpy( target, &value, serialize_size(value) );
        std::cout<<"verificado"<<serialize_size(value)<<std::endl;
        return target + sizeof(size_t);
    }
    
    static const char* deserialize( const char* source, POD& target ) {
        std::cout<<"Vamos a deserializar"<<std::endl;
        memcpy( &target, source, serialize_size(target) );
        return source + sizeof(target);
    }
};

template<>
size_t SerializablePOD<char*>::serialize_size(char* str)
{
    /* return sizeof(size_t) + strlen(str); */
    std::cout<<"el TAMANO del char* "<<sizeof(size_t) + strlen(str)<<std::endl;
    return sizeof(size_t) + strlen(str);
}

template<>
const char* SerializablePOD<char*>::deserialize( const char* source, char*& target )
{
      //source = source + 8;

     //size_t length;
     //memcpy( &length, source, sizeof(size_t));
    //size_t t = *(size_t*)length; 
    //std::cout<<std::endl<<"--------------el tamano es :"<<length<<std::endl;
    //std::cout << target << std::endl;
   // memcpy( &target, source , length );
    return source + sizeof(size_t) + 10;  //init 8 + 4
}

template<>
char* SerializablePOD<char*>::serialize( char* target, char* value ) {
    size_t l = strlen(value);
    std::cout<<"vamos a serializar char*->"<<l<<std::endl;
    memcpy( target,&l , sizeof(size_t) );
    target = target + sizeof(size_t);
    memcpy( target, value, strlen(value) );
     std::cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<std::string(target,l)<<std::endl;
    return target + l;
}