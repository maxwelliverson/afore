//
// Created by maxwell on 2020-02-22.
//

#ifndef AFORE_PRIMITIVES_H
#define AFORE_PRIMITIVES_H

#include <tuple>
#include <type_traits>
#include "concepts.h"
#include "detail/primitives.inl"
#include "detail/stl.inl"

namespace afore
{
    struct true_type :
        AFORE_RESULT_TRUE;
    struct false_type :
        AFORE_RESULT_FALSE;


    template <typename R>
    afore_type result_t = typename detail::result_t<R>;
    template <typename V>
    afore_var result_v = detail::result_v<V>;

    /////

    template <typename Type>
    afore_var is_ct_list = result_v<detail::IsCtList<Type>>;
    template <typename Type>
    afore_var is_value_list = result_v<detail::IsValueListImpl<Type>>;
    template <typename Type>
    afore_var is_type_list = result_v<detail::IsTypeListImpl<Type>>;

    //Concatenate a number of CT lists.
    template <typename ...Lists>
    afore_type concatenate_lists = result_t<detail::ConcatenateListsImpl<Lists...>>;

    //Conditional type that can be used for SFINAE pruning. Equivalent to std::enable_if_t
    template <bool cond, typename A>
    afore_type if_t = result_t<detail::IfImpl<cond, A>>;

    //Conditional type that is either A or B depending on the boolean value. Currently instantiates both types unfortunately.
    template <bool cond, typename A, typename B>
    afore_type if_else_t = result_t<typename detail::IfImpl<cond, A>::template ElseImpl<B>>;


    //Query whether or not a CT list is empty.
    template <typename List>
    afore_var is_empty = result_v<detail::IsEmptyImpl<List>>;


    //Returns the qualifier stripped type of Arg, and will also unwrap TypeValue objects.
    template <auto Arg>
    afore_type type_of       = result_t<typename detail::TypeOfImpl<Arg>>;
    template <typename T>
    afore_type types_of_list = result_t<typename detail::TypesOfListImpl<T>>;

    template <typename T>
    afore_var typevalue = detail::TypeValue<type_of<detail::TypeValue<T>{}>>{};
    template <typename T>
    afore_type typevalue_t = std::remove_cvref_t<decltype(typevalue<T>)>;
    template <typename T>
    afore_var exact_typevalue = detail::TypeValue<T>{};
    template <typename T>
    afore_type exact_typevalue_t = std::remove_cvref_t<decltype(exact_typevalue<T>)>;


    template <auto N, Index Initial = 0ul, Index Step = 1ul>
    afore_type make_sequence = result_t<
        detail::GenerateIndexListImpl<
            Index{N},
            0ul,
            Initial,
            Step,
            empty_index_list
        >
    >;

    namespace detail
    {
        template <auto t>
        constexpr auto _tuple_to_list = []<auto ...index>(ValueList<index...>){return ValueList<std::get<index>(t)...>{};};
    }


    template <auto p>
    afore_type list_from_pair = ValueList<p.first, p.second>;
    template <auto t>
    afore_type list_from_tuple = decltype(detail::_tuple_to_list<t>(make_sequence<std::tuple_size_v<decltype(t)>>()));

    template <ValueListType List>
    afore_var to_tuple = List::values;
    template <TypeListType List>
    afore_type to_tuple_t = std::remove_const_t<typename List::tuple_type>;

    template <PairType List>
    afore_var to_pair = std::pair(to_tuple<List>);


    template <SizedList<1> solo>
    afore_var unwrap = solo::template get<0ul>();
    template <SizedList<1> solo>
    afore_type unwrap_t = typename solo::template type<0ul>;


    template <NonEmptyList List>
    afore_type head = result_t<detail::HeadImpl<List>>;

    template <NonEmptyList List>
    afore_type tail = result_t<detail::TailImpl<List>>;

    template <typename F, AforeListType List>
    inline constexpr result_t<detail::ApplyImpl<F, List>> apply();
    template <auto F, AforeListType List>
    afore_var apply()
    {
        return result_v<detail::ApplyImpl<detail::Value<F>, List>>;
    }


    template <TypeListType List>
    struct AsValues;
    template <auto ...Values>
    struct AsValues<TypeList<detail::Value<Values>...>> :
        AFORE_RESULT_T(ValueList<Values...>);

    template <TypeListType List>
    afore_type as_values = result_t<AsValues<List>>;


    /*template <typename List, AFORE_ENABLE_IF_MSG(is_ct_list<List>, "Testing testing one two three...")>
    struct Test{};*/
}

#endif //AFORE_PRIMITIVES_H
