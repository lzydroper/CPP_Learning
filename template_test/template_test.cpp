#include "template_test.hpp"

template<typename T>
T Vec<T>::dot(const Vec<T>& other) const
{
    return 0;
}

template<>
float Vec<float>::dot(const Vec<float>& other) const
{
    return 0.0f;
}