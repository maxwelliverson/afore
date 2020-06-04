//
// Created by maxwell on 2020-03-03.
//

#ifndef AFORE_TUPLE_H
#define AFORE_TUPLE_H

#include "afore/primitives.h"

namespace afore
{
    template <typename T>
    struct TupleElement{
        T val_;
        constexpr TupleElement() = default;
        template <typename U>
        constexpr explicit TupleElement(U&& v) : val_(std::forward<U>(v)){}
    };
    template <typename T>
    struct TupleElement<T&>{
        T& val_;
        template <typename U>
        constexpr explicit TupleElement(U&& v) : val_(std::forward<U>(v)){}
    };
    template <typename T>
    struct TupleElement<T&&>{
        T&& val_;
        template <typename U>
        constexpr explicit TupleElement(U&& v) : val_(std::forward<U>(v)){}
    };

    template <typename ...Types>
    struct Tuple;
    template <>
    struct Tuple<>
    {
        constexpr Tuple() = default;
        constexpr bool operator==(const Tuple& other) const noexcept{
            return true;
        }
        constexpr bool operator!=(const Tuple& other) const noexcept{
            return false;
        }
        template <typename T, typename ...U>
        constexpr bool operator==(const Tuple<T, U...>& other) const noexcept{
            return false;
        }
        template <typename T, typename ...U>
        constexpr bool operator!=(const Tuple<T, U...>& other) const noexcept{
            return true;
        }
    };
    template <typename head_t, typename ...tail_t>
    struct Tuple<head_t, tail_t...> : TupleElement<head_t>, public Tuple<tail_t...>
    {
    private:
        using tuple_element = TupleElement<head_t>;
        using tuple_element::val_;

    public:
        using base = Tuple<tail_t...>;
        using tuple_element::tuple_element;

        template <typename head_arg_t, typename ...tail_arg_t>
        constexpr //explicit(!std::is_same_v<head_arg_t, head_t> || (!std::is_same_v<tail_arg_t, tail_t> || ...))
        Tuple(head_arg_t&& h, tail_arg_t&& ...t) :
            tuple_element(std::forward<head_arg_t>(h)),
            base(std::forward<tail_arg_t>(t)...){}

        inline constexpr head_t& value() noexcept{
            return tuple_element::val_;
        }
        inline constexpr const head_t& value() const noexcept{
            return tuple_element::val_;
        }
    };

    inline constexpr bool operator==(const Tuple<>&, const Tuple<>&) noexcept{
        return true;
    }
    template <typename T, typename ...U>
    inline constexpr bool operator==(const Tuple<T, U...>&, const Tuple<>&) noexcept{
        return false;
    }
    template <typename T, typename ...U>
    inline constexpr bool operator==(const Tuple<>&, const Tuple<T, U...>&) noexcept{
        return false;
    }
    template <typename T1, typename ...U1, typename T2, typename ...U2>
    inline constexpr bool operator==(const Tuple<T1, U1...>& a, const Tuple<T2, U2...>& b) noexcept{
        return (a.value() == b.value()) &&
        (static_cast<typename Tuple<T1, U1...>::base>(a) == static_cast<typename Tuple<T2, U2...>::base>(b));
    }
    template <typename ...T1, typename ...T2>
    inline constexpr bool operator!=(const Tuple<T1...>& a, const Tuple<T2...>& b) noexcept{
        return !(a == b);
    }

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
            return get<I-1>(static_cast<Tuple<T...>&>(tup));
        }
        else
        {
            return tup.value();
        }
    }
    template <Index I, typename H, typename ...T> requires(static_cast<std::size_t>(I) <= sizeof...(T))
    afore_var get(const Tuple<H, T...>& tup)
    {
        if constexpr(I != 0ul)
        {
            return get<I-1>(static_cast<const Tuple<T...>&>(tup));
        }
        else
        {
            return tup.value();
        }
    }
    template <Index I, typename H, typename ...T> requires(static_cast<std::size_t>(I) <= sizeof...(T))
    afore_var get(Tuple<H, T...>&& tup)
    {
        if constexpr(I != 0ul)
        {
            return get<I-1>(static_cast<Tuple<T...>&&>(tup));
        }
        else
        {
            return tup.value();
        }
    }
    template <Index I, typename H, typename ...T> requires(I <= sizeof...(T))
    afore_var get(const Tuple<H, T...>&& tup)
    {
        if constexpr(I != 0ul)
        {
            return get<I-1>(static_cast<const Tuple<T...>&&>(tup));
        }
        else
        {
            return tup.value();
        }
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
    namespace detail{
        template <typename T1, typename T2, Index ...Is, Index ...Js>
        afore_var tuple_cat_helper_fn(T1&& t1, T2&& t2, std::index_sequence<Is...>, std::index_sequence<Js...>){
            return afore::Tuple(get<Is>(t1)..., get<Js>(t2)...);
        }

        afore_var tuple_cat_helper = []<Index ...Is, Index ...Js>
                (auto&& t1, auto&& t2, std::index_sequence<Is...>, std::index_sequence<Js...>) constexpr
        {return afore::Tuple(std::forward<tuple_element_t<Is, decltype(t1)>>(get<Is>(t1))...,
                             std::forward<tuple_element_t<Js, decltype(t2)>>(get<Js>(t2))...);};
    }
    template <typename T, typename T2, typename ...Tup>
    afore_var tuple_cat(T&& tup, T2&& tup2, Tup&& ...tuples)
    {
        decltype(auto) cat = tuple_cat(detail::tuple_cat_helper_fn(
                std::forward<T>(tup),
                std::forward<T2>(tup2),
                std::make_index_sequence<tuple_size_v<T>>(),
                std::make_index_sequence<tuple_size_v<T2>>()),
                std::forward<Tup>(tuples)...);
        return cat;
    }
}

#endif //AFORE_TUPLE_H
