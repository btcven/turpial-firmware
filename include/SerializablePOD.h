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
        std::cout << "-->Serializando"<<std::endl;
        (char*)memcpy( target, &value, serialize_size(value) );
        return target + sizeof(value); //move the pointer to the next addres
    }
    
    static const char* deserialize( const char* source, POD& target ) {
        std::cout << "deserializando"<<std::endl;
        memcpy( &target, source, sizeof(target) );
         std::cout << "el length es " <<sizeof(target)<<std::endl;
        //return source + serialize_size(target); // //move the pointer to the next addres
        return source + sizeof(target);
    }
};

template<>
size_t SerializablePOD<char*>::serialize_size(char* str)
{
    return sizeof(size_t) + strlen(str);
}

template<>
const char* SerializablePOD<char*>::deserialize( const char* source, char*& target )
{
    size_t length;
    memcpy( &length, source, sizeof(size_t));
    memcpy( &target, source , length);
    std::cout << "el tamamno es " << length <<std::endl;  
    return source + sizeof(size_t) + length; //point to the next data
}

template<>
char* SerializablePOD<char*>::serialize( char* target, char* value ){
    size_t len = (size_t)strlen(value);
    memcpy( target,&len , sizeof(size_t) );
    memcpy(target,value,len);
    //just for printing in stdout
    //fwrite(target,len,1,stdout);
    std::cout << std::string(target,len) << std::endl;
    target = target + sizeof(size_t);
    return target + len;
}