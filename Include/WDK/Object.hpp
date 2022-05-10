#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Defines.hpp"

class Object
{
public:
	Object();
	~Object();

	PVOID operator new(SIZE_T size);
	VOID  operator delete(PVOID ptr);
};

#endif // OBJECT_HPP