#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
#include <stddef.h> //to use size_t type

// Interface class
/*
This class is used as interface to be implemented by other 
classes type data transfer object or DTO
*/
class Serializable
{
public:
    virtual void serialize_size() const = 0;
    virtual void serialize(char* dataOut) const = 0;
    virtual void deserialize(const char* dataIn) = 0;
};
#endif