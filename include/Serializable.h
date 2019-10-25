#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
#include <stddef.h> //to use size_t type

// Interface class
class Serializable
{
public:
    virtual size_t serialize_size() const = 0;
    virtual void serialize(char* dataOut) const = 0;
    virtual void deserialize(const char* dataIn) = 0;
};
#endif