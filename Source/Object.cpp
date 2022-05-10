#include "Object.hpp"

#include "WDK/System/Memory.hpp"

Object::Object()
{

}

Object::~Object()
{

}

PVOID Object::operator new(SIZE_T size)
{
	return Memory::Allocate(size, TRUE);
}

VOID Object::operator delete(PVOID ptr)
{
	Memory::Free(ptr);
}
