#pragma once

#include <vector>

#include "Components.hpp"

class ComponentPoolBase
{
public:
    virtual ~ComponentPoolBase() = 0;
};

template<class T>
class ComponentPool : ComponentPoolBase
{
    std::vector<T> components;
};

std::vector<ComponentPoolBase> componentPools;

template<class T>
std::vector<T>* getComponentPool(int index)
{
    return reinterpret_cast<ComponentPool<T>>(&componentPools[index]);
}