//
// Created by maxwell on 2020-03-03.
//

#ifndef AFORE_TUPLE_H
#define AFORE_TUPLE_H

#include "afore/primitives.h"

namespace afore
{
    template <typename ...Types>
    struct Tuple;
    template <>
    struct Tuple<>
    {

    };
    template <typename head_t, typename ...tail_t>
    struct Tuple<head_t, tail_t...> : public Tuple<tail_t...>
    {
        using base = Tuple<tail_t...>;
        constexpr Tuple() = default;
        template <typename head_arg_t, typename ...tail_arg_t>
        constexpr explicit(!std::is_same_v<head_arg_t, head_t> || (!std::is_same_v<tail_arg_t, tail_t> || ...))
        Tuple(head_arg_t&& h, tail_arg_t&& ...t) :
            val_(std::forward<head_arg_t>(h)),
            base(std::forward<tail_arg_t>(t)...){}

        head_t val_;
    };

    template <typename ...Args>
    Tuple(Args&&...) -> Tuple<Args...>;

    template <typename T>
    struct tuple_size;
    template <template <typename...> typename TT, typename ...Types>
    struct tuple_size<TT<Types...>> :
        AFORE_RESULT_V(Index{sizeof...(Types)});

    template <typename T>
    afore_var tuple_size_v = result_v<tuple_size<std::remove_cvref_t<T>>>;

    template <Index I, typename T>
    struct tuple_element;
    template <template <typename...> typename TT, typename H, typename ...T>
    struct tuple_element<0ul, TT<H, T...>> :
        AFORE_RESULT_T(H);
    template <Index I, template <typename...> typename TT, typename H, typename ...T>
    requires (static_cast<std::size_t>(I) <= sizeof...(T))
    struct tuple_element<I, TT<H, T...>> :
        AFORE_NEXT(tuple_element<I-1, TT<T...>>);
    template <Index I, template <typename...> typename TT, typename ...T>
    struct tuple_element<I, TT<T...>>
    {
        static_assert(static_cast<std::size_t>(I) < sizeof...(T), "Index out of range error");
    };

    template <Index I, typename T>
    afore_type tuple_element_t = result_t<tuple_element<I, std::remove_cvref_t<T>>>;

    template <Index I, typename H, typename ...T> requires(static_cast<std::size_t>(I) <= sizeof...(T))
    afore_var get(Tuple<H, T...>& tup)
    {
        if constexpr(I != 0ul)
        {
            return get<I-1, T...>(tup);
        }
        else
        {
            return tup.val_;
        }
    }
    template <Index I, typename H, typename ...T> requires(static_cast<std::size_t>(I) <= sizeof...(T))
    afore_var get(const Tuple<H, T...>& tup)
    {
        if constexpr(I != 0ul)
        {
            return get<I-1>(dynamic_cast<const Tuple<T...>&>(tup));
        }
        else
        {
            return tup.val_;
        }
    }
    template <Index I, typename H, typename ...T> requires(static_cast<std::size_t>(I) <= sizeof...(T))
    afore_var get(Tuple<H, T...>&& tup)
    {
        if constexpr(I != 0ul)
        {
            return get<I-1, T...>(std::forward<Tuple<T...>>(tup));
        }
        else
        {
            return tup.val_;
        }
    }
    template <Index I, typename H, typename ...T> requires(static_cast<std::size_t>(I) <= sizeof...(T))
    afore_var get(const Tuple<H, T...>&& tup)
    {
        return get<I-1, T...>(std::forward<Tuple<T...>>(tup));
    }
    template <Index ...I, typename Tup>
    afore_var scatter(Tup&& tuple)
    {
        return Tuple(std::forward<tuple_element_t<I, Tup>...>(get<I>(std::forward<Tup>(tuple))...));
    }
    template <IndexListType IL, typename Tup>
    afore_var scatter(Tup&& tuple)
    {
        return []<Index ...I>(Tup&& tup,const IndexList<I...>&&) mutable
            {return scatter<I...>(std::forward<Tup>(tup));}(std::forward<Tup>(tuple), IL{});
    }

    template <typename T>
    afore_var tuple_cat(T&& tup)
    {
        return std::forward<T>(tup);
    }
    template <typename T, typename T2, typename ...Tup>
    afore_var tuple_cat(T&& tup, T2&& tup2, Tup&& ...tuples)
    {
        return tuple_cat([]<Index ...Is, Index ...Js>
            (auto&& t1, auto&& t2, const IndexList<Is...>&&, const IndexList<Js...>&&) constexpr
        {return Tuple(std::forward<tuple_element_t<Is, decltype(t1)>>(get<Is>(t1))...,
                      std::forward<tuple_element_t<Js, decltype(t2)>>(get<Js>(t2))...);}
            (std::forward<T>(tup), std::forward<T2>(tup2),
            make_sequence<tuple_size_v<T>>(), make_sequence<tuple_size_v<T2>>()),
            std::forward<Tup>(tuples)...);
    }
}

#endif //AFORE_TUPLE_H
