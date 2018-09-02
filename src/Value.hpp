#pragma once

#include "Concepts.hpp"

#include <julia.h>
#include <type_traits>

namespace jl
{

class value
{
public:
    value(jl_value_t* boxed_value_) noexcept : _boxed_value{boxed_value_} {}

    value() noexcept = default;

    template<typename TargT,
             std::enable_if_t<std::is_fundamental<TargT>{}
                              || impl::is_array<TargT>{}>* = nullptr>
    TargT get()
    {
        if constexpr (std::is_integral_v<TargT>)
            return static_cast<TargT>(impl::unbox<long>(_boxed_value));
        else if constexpr (std::is_floating_point_v<TargT>)
            return static_cast<TargT>(impl::unbox<double>(_boxed_value));
        else
            return *this;
    }

    template<typename TargT,
             std::enable_if_t<!std::is_fundamental<TargT>{}>* = nullptr>
    TargT get() noexcept
    {
        if constexpr (std::is_same_v<TargT, jl_value_t*>)
            return static_cast<jl_value_t*>(*this);
        else if constexpr (std::is_pointer_v<TargT>)
            return reinterpret_cast<TargT>(_boxed_value);
        else
            return *reinterpret_cast<std::decay_t<TargT>*>(_boxed_value);
    }

    template<
        typename TargT,
        typename = std::enable_if_t<std::is_fundamental<TargT>{}
                                    && !std::is_same_v<TargT, jl_value_t*>>>
    operator TargT()
    {
        return impl::unbox<TargT>(_boxed_value);
    }

    template<typename TargT,
             std::enable_if_t<!std::is_fundamental<TargT>{}>* = nullptr>
    operator TargT()
    {
        return get<TargT>();
    }

    operator jl_value_t*() { return _boxed_value; }

    template<typename ElemT>
    explicit operator array<ElemT>() noexcept
    {
        return reinterpret_cast<jl_array_t*>(_boxed_value);
    }

private:
    jl_value_t* _boxed_value;
};

} // namespace jl
