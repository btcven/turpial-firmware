/**
 * @file SerializablePOD.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <iostream>
#include <cstring>
#include <cstdio>

/**
 * @brief This class is a static one to be used inside data transfer objects to reimplement 
 * the functionality inside DTO methods to serialized and deserialize the information inside
 * 
 * @tparam POD 
 */
template <typename POD>
class SerializablePOD
{
public:
    /**
     * @brief 
     * 
     * @param str 
     * @return size_t 
     */
    static std::size_t serialize_size(POD str)
    {
        return sizeof(str);
    }

    /**
     * @brief 
     * 
     * @param target 
     * @param value 
     * @return char* 
     */
    static char *serialize(char *target, POD value)
    {
        std::memcpy(target, &value, serialize_size(value));
        std::cout << "verificado" << serialize_size(value) << std::endl;
        return target + sizeof(size_t);
    }

    /**
     * @brief 
     * 
     * @param source 
     * @param target 
     * @return const char* 
     */
    static const char *deserialize(const char *source, POD &target)
    {
        std::cout << "Vamos a deserializar" << std::endl;
        std::memcpy(&target, source, serialize_size(target));
        return source + sizeof(target);
    }
};


template <>
std::size_t SerializablePOD<char *>::serialize_size(char *str)
{
    std::cout << "el TAMANO del char* " << sizeof(std::size_t) + std::strlen(str) << std::endl;
    return sizeof(std::size_t) + std::strlen(str);
}

template<>
const char* SerializablePOD<char*>::deserialize( const char* source, char*& target )
{
    std::size_t length;
    std::cout <<"Vamos a deserializar" << std::endl;
    std::cout <<":----------address of char field in structure------------------------------->"<< static_cast<const void*>(&target)<<std::endl;
    std::memcpy( &length, source, sizeof(size_t));
    std::cout << "el length of string.."<< (size_t)length << std::endl;
    source = source +sizeof(size_t);
    //target = (char*)std::malloc(sizeof(char)*length); //error
    std::memcpy( target, source, length);
    std::cout << "salida: " << std::string(source,length) << std::endl;
    std::cout << "dato en el field despues de deserializar: : " << target << std::endl;
    std::cout <<":----------address->"<< static_cast<const void*>(target)<<std::endl;

    return source + length;  

}

template<>
char* SerializablePOD<char*>::serialize( char* target, char* value )
{  
    
    std::size_t l = strlen(value);
    
    std::cout << ": dir buffer ---------->" << static_cast<const void*> (&target) << std::endl;
    std::cout << ": dir inside pointer to put data ---------->" << static_cast<const void*> (value) << std::endl;
    std::cout << "vamos a serializar char*->" << l << std::endl;
    
    std::memcpy( target, &l , sizeof(size_t) );
    target = target + sizeof(size_t);
    std::memcpy( target, value, std::strlen(value) );
    
    std::cout << std::string(target,l) << std::endl;
    return target + l;

} 