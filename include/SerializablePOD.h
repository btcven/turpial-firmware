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
    size_t length ;

    std::cout<<"Vamos a deserializar" << std::endl;
    std::cout <<": ---------->"<< static_cast<const void*>(source)<<std::endl;
    memcpy( &length, source, sizeof(size_t));
    std::cout<<"el length jajaj.."<< (size_t)length<<std::endl;
    source = source +sizeof(size_t);
    memcpy( &target, source , length );
    std::cout<<"salida: "<<std::string(source,length)<<std::endl;
    return source + length;  
}

template<>
char* SerializablePOD<char*>::serialize( char* target, char* value ) {
    size_t l = strlen(value);
    std::cout <<": ---------->"<< static_cast<const void*>(target)<<std::endl;
    std::cout<<"vamos a serializar char*->"<<l<<std::endl;
    memcpy( target, &l , sizeof(size_t) );
    target = target + sizeof(size_t);
    std::cout <<": ---------->"<< static_cast<const void*>(target)<<std::endl;
    memcpy( target, value, strlen(value) );
    std::cout <<": ---------->"<< static_cast<const void*>(target)<<std::endl;  
     std::cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<std::string(target,l)<<std::endl;
    return target + l;
}