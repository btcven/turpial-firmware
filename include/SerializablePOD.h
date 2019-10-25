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

    static size_t serialize_size(POD str);
    static char* serialize( char* target, POD value );
    static const char* deserialize( const char* source, POD& target ); 
};
 