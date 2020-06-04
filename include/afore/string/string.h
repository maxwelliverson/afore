//
// Created by maxwell on 2020-02-22.
//

#ifndef AFORE_STRING_H
#define AFORE_STRING_H

#include "afore/primitives.h"

namespace afore
{
    template <Char C, auto N>
    struct string
    {
        constexpr static int size = N;
        const std::array<C, N> str;

        constexpr string(const C* c) : string(c, std::make_index_sequence<N-1>()){}
        constexpr string(std::array<C, N>&& arr) : str(std::move(arr)){}

        constexpr const C* c_str() const noexcept
        {
            return str.data();
        }
        constexpr operator const C*() const noexcept
        {
            return str.data();
        }
        explicit operator std::string() const noexcept
        {
            return std::string(str.data());
        }
        template <auto M = N>
        constexpr explicit operator std::array<C, M>() const noexcept
        {
            constexpr auto helper = []<auto ...index>(const std::array<C, N>& str_arr, [[maybe_unused]] std::index_sequence<index...>)
                                    {
                                        return std::array<C, M>{str_arr[index]...};
                                    };
            const std::array<C, M> result = helper(str, std::make_index_sequence<N>());
            return result;
        }

        template <Integer I>
        inline constexpr const C& operator [](I&& i) const noexcept
        {
            return str[i];
        }

    private:
        template <Index ...index>
        constexpr string(const C* s, std::index_sequence<index...>) : str{s[index]...}{}
    };

    template <typename C, auto N>
    string(const C (&)[N]) -> string<C, N>;

    inline namespace literals
    {
        inline namespace string_literals
        {
            template <Char T, T ...args>
            inline constexpr static string<T, sizeof...(args) + 1> operator""_A()
            {
                constexpr auto size = sizeof...(args);
                T str[size + 1]{args...};
                return {str};
            }
        }
    }
    inline namespace operators
    {
        inline namespace string_operators
        {
            template <Char C1, auto N1, Char C2, auto N2>
            afore_var operator+(const string<C1, N1>& s1, const string<C2, N2>& s2)
            {
                afore_type C = std::remove_pointer_t<std::common_type_t<C1, C2>>;
                constexpr auto size = N1 + N2 - 1;
                return [&]<auto ...I, auto ...J>(const std::index_sequence<I...>&&, const std::index_sequence<J...>&&)
                {
                    return string<C, size>({s1[I]..., s2[J]...});
                }(std::make_index_sequence<N1-1>(), std::make_index_sequence<N2-1>());
            }
        }
    }
}

#endif //AFORE_STRING_H
