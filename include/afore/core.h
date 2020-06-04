//
// Created by maxwell on 2020-02-26.
//

#ifndef AFORE_CORE_H
#define AFORE_CORE_H

#include "macros.h"
namespace afore
{
    template <typename ...>
    struct TypeList;
    template <auto ...>
    struct ValueList;

    using Index = std::size_t;
    afore_var IndexNegOne = static_cast<Index>(-1);

    namespace detail
    {
        template<typename T>
        struct TypeValue;

        template<decltype(auto) Obj>
        struct Value{
            template <decltype(auto) OtherObj>
            afore_mem_func operator<=>(const Value<OtherObj>&) const noexcept{
                return Obj <=> OtherObj;
            }
        };

        //Core ResultType that acts as the final return type for all metafunctions.
        template<typename R>
        struct ResultType {afore_type result = R;};
        template <decltype(auto) V>
        struct ResultType<Value<V>>{afore_var result = V;};

        //Empty Base type to maintain a consistent syntax
        struct ResultBase{};

        //Alias for accessing the result of a type computation as a type.
        template<typename T>
        afore_type result_t = typename T::result;
        //Alias for accessing the result of a type computation as a value.
        template<typename T>
        afore_var result_v = T::result;

    }

    template <typename ...Types>
    struct TypeList
    {
        constexpr TypeList() = default;
        template <typename ...T>
        constexpr TypeList(T&&...){}

        inline constexpr bool operator ==(const TypeList&) const noexcept {return true;}
        template <typename ...OtherTypes>
        inline constexpr bool operator ==(const TypeList<OtherTypes...>&) const noexcept {return false;}

        afore_var size = Index{sizeof...(Types)};
        afore_type tuple_type = std::tuple<Types...>;
        template <Index I>
        afore_type type = std::tuple_element_t<I, tuple_type>;

        template <Index I>
        afore_var get() noexcept {return detail::TypeValue<type<I>>{};}
    };
    template <auto ...Vals>
    struct ValueList
    {
        afore_var values = std::tuple{Vals...};
        template <Index I>
        afore_var get(){return std::get<I>(values);}
        afore_var size = sizeof...(Vals);
        afore_var front = get<0>();
        afore_var back = get<size-1>();
        afore_var pivot = get<size / 2>();

        template <Index I>
        afore_type type = std::remove_cvref_t<std::decay_t<decltype(ValueList::template get<I>())>>;
    };

    template <>
    struct ValueList<>
    {
        template <Index I>
        afore_var get(){ static_assert(I == 94857398, "get was called on an empty ValueList.");};
        afore_var size = 0;

        template <Index I>
        afore_type type = void;
    };

    template <Index ...I>
    afore_type IndexList = ValueList<I...>;

    struct Nothing{};

    using empty_list = ValueList<>;
    using empty_type_list = TypeList<>;
    using empty_index_list = IndexList<>;
    using placeholder = Nothing;
    using placeholder_t = const placeholder&;

    template <template <typename...> typename TT>
    TypeList(TT<>) -> TypeList<>;
    template <template <auto...> typename TT>
    TypeList(TT<>) -> TypeList<>;
    template <template <auto...> typename TT, typename ...Types, detail::TypeValue<Types> ...args>
    TypeList(TT<args...>) -> TypeList<Types...>;
}

#endif //AFORE_CORE_H
