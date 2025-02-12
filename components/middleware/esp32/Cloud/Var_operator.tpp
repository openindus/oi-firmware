#pragma once

template <typename Type>
Type Var<Type>::operator+=(Type value)
{
    this->set(this->get() + value);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator-=(Type value)
{
    this->set(this->get() - value);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator*=(Type value)
{
    this->set(this->get() * value);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator/=(Type value)
{
    this->set(this->get() / value);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator%=(Type value)
{
    this->set(this->get() % value);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator--()
{
    this->set(this->get()--);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator=(Type value)
{
    this->set(value);
    return (this->get());
}

template <typename Type>
Type Var<Type>::operator=(Var<Type> &variable)
{
    this->set(variable.get());
    return (this->get());
}

template <typename Type>
bool Var<Type>::operator==(Type value)
{
    return (this->get() == value);
}

template <typename Type>
bool Var<Type>::operator!=(Type value)
{
    return (this->get() != value);
}

template <typename Type>
bool Var<Type>::operator&&(Type value)
{
    return (this->get() && value);
}

template <typename Type>
bool Var<Type>::operator||(Type value)
{
    return (this->get() || value);
}

template <typename Type>
bool Var<Type>::operator!()
{
    return (! this->get());
}
