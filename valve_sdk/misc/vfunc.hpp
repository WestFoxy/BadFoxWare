#pragma once

template<typename FuncType>
__forceinline static FuncType CallVFunction(void* ppClass, int index)
{
    int* pVTable = *(int**)ppClass;
    int dwAddress = pVTable[index];
    return (FuncType)(dwAddress);
}

template <typename t>
static t GetVFuncF(void* class_pointer, size_t index) {
	return (*(t * *)class_pointer)[index];
}
