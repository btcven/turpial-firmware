#include <stdio.h>
#include <iostream>
#include <cstring>

/*
This class is a static one to be used inside data transfer objects to reimplement
the functionality inside DTO methids to serialized and deserialize the information inside
*/
template <typename POD>
class SerializablePOD
{
public:

    static size_t serialize_size(POD str)
    {
        std::cout<<"the size is:  "<<sizeof(str)<<std::endl;
        return sizeof(str);
    }

    static char* serialize( char* target, POD value )
    {  
        char* temp;
        std::cout<<"Going to serialized"<<std::endl;
        temp = (char*)memcpy( target, &value, serialize_size(value) );
        return target + sizeof(value); //move the pointer to the next addres
    } 

    static const char* deserialize( const char* source, POD& target )
    {
        std::cout<<"Going to deserialized"<<std::endl;
        memcpy( &target, source, serialize_size(target) );
        return source + serialize_size(target); // //move the pointer to the next addres
    }
};

template<>
size_t SerializablePOD<char*>::serialize_size(char* str)
{
    std::cout<<"The length of char* "<<sizeof(size_t) + strlen(str)<<std::endl;
    return sizeof(size_t) + strlen(str);
}

template<>
const char* SerializablePOD<char*>::deserialize( const char* source, char*& target )
{
    size_t length;
    memcpy( &length, source, sizeof(size_t));
    std::cout<<std::endl<<"--------------The length is :"<<length<<std::endl;
    memcpy( &target, source , length);
    return source + sizeof(size_t) + length; //point to the next data
}

template <>
char* SerializablePOD<char*>::serialize( char* target, char* value ){
    size_t len = (size_t)strlen(value);
    std::cout<<"Going to Serialized char*->"<<len<<std::endl;
    memcpy( target,&len , sizeof(size_t) );
    target = target + sizeof(size_t);
    std::cout<<target<<std::endl;
    return target + len;
} 