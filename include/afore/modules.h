//
// Created by maxwell on 2020-02-27.
//

#ifndef AFORE_MODULES_H
#define AFORE_MODULES_H

#include "core.h"
#include "concepts.h"

namespace afore
{

    template <Char C, auto N>
    struct string;
    template <typename, typename>
    struct function;

    inline namespace concepts{}
    inline namespace literals
    {
        inline namespace string_literals
        {
            template <Char T, T ...args>
            inline constexpr static string<T, sizeof...(args) + 1> operator""_A();
        }

        inline namespace index_literals
        {
            afore_var operator"" _i(unsigned long long val);
        }
    }
    inline namespace operators
    {
        inline namespace string_operators
        {
            template <Char C1, auto N1, Char C2, auto N2>
            afore_var operator+(const string<C1, N1>& s1, const string<C2, N2>& s2);
        }
        inline namespace function_operators{}
    }
    inline namespace functors{}


    namespace nameof
    {
        template <typename T>
        constexpr auto type_name();

        template<Enum E>
        constexpr auto enum_name();
    }
    namespace data_structures
    {

    }
    namespace regex{}
}

namespace afore_module
{
    namespace core
    {
        using afore::ValueList;
        using afore::TypeList;
        using afore::detail::Value;
    }
    namespace string
    {
        using afore::string;
        using namespace afore::string_literals;
        using namespace afore::string_operators;
    }
    namespace functional
    {
        using afore::function;
        using namespace afore::functors;
        using namespace afore::function_operators;
    }
    namespace index
    {
        using afore::Index, afore::IndexList;
        using namespace afore::index_literals;
    }
    namespace utils
    {
        using namespace afore::nameof;
        using namespace afore::regex;
    }
}

#endif //AFORE_MODULES_H
