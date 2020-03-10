//
// Created by maxwell on 2020-02-26.
//

#ifndef AFORE_STL_INL
#define AFORE_STL_INL

namespace std
{
    template<auto ...Args>
    class tuple_size<afore::ValueList<Args...>>
    : public integral_constant<size_t, sizeof...(Args)> { };
    template <typename ...Types>
    class tuple_size<afore::TypeList<Types...>>
    : public integral_constant<size_t, sizeof...(Types)> { };

    template <size_t I, auto ...Args>
    class tuple_element<I, afore::ValueList<Args...>>
    {
        using type = typename afore::ValueList<Args...>::template type<afore::Index{I}>;
    };

    template <size_t I, typename ...Types>
    struct tuple_element<I, afore::TypeList<Types...>>
    {
        using type = typename afore::TypeList<Types...>::template type<afore::Index{I}>;
    };

    template <size_t I, auto ...Args>
    afore_var get(const afore::ValueList<Args...>&)
    {
        return afore::ValueList<Args...>::template get<afore::Index{I}>();
    }
    template <size_t I, auto ...Args>
    afore_var get(const afore::ValueList<Args...>&&)
    {
        return afore::ValueList<Args...>::template get<afore::Index{I}>();
    }

    template <size_t I, typename ...Types>
    afore_var get(const afore::TypeList<Types...>&)
    {
        return afore::TypeList<Types...>::template get<afore::Index{I}>();
    }
    template <size_t I, typename ...Types>
    afore_var get(const afore::TypeList<Types...>&&)
    {
        return afore::TypeList<Types...>::template get<afore::Index{I}>();
    }

    template <afore::Index I, typename T>
    afore_var get(T&& arg)
    {
        return get<static_cast<std::size_t>(I)>(std::forward<T>(arg));
    }

    template <afore::concepts::ValueListType List, typename T>
    afore_var get(T&& arg)
    {
        constexpr auto dispatch = []<auto ...index>(T&& tup_like, const afore::ValueList<index...>&&)
        {
            return tuple{get<index>(forward<T>(tup_like))...};
        };
        return dispatch(forward<T>(arg), List{});
    }

}

#endif //AFORE_STL_INL
