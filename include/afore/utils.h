//
// Created by maxwell on 2020-02-18.
//

#ifndef AFORE_UTILS_H
#define AFORE_UTILS_H

#include <magic_enum.hpp>
#include <variant>

namespace valkyrie
{
    template <auto ...values>
    struct IntegerSequence{};

    template <int size, auto initial, auto ...values>
    constexpr auto make_integer_sequence_helper()
    {
        if constexpr(size == 0)
            return IntegerSequence<values...>{};
        else
            return make_integer_sequence_helper<size - 1, initial, size + initial - 1, values...>();
    }
    template <int size, auto initial = 0>
    constexpr auto make_integer_sequence()
    {
        return make_integer_sequence_helper<size, initial>();
    }

    template <auto ...values>
    struct enum_properties;
    template <auto head>
    struct enum_properties<head>
    {
        constexpr static auto max = head;
        constexpr static auto min = head;
    };
    template <auto head, auto ...tail>
    struct enum_properties<head, tail...>
    {
        constexpr static auto max = head > enum_properties<tail...>::max ? head : enum_properties<tail...>::max;
        constexpr static auto min = head < enum_properties<tail...>::min ? head : enum_properties<tail...>::min;
    };

    template <typename E>
    constexpr auto enum_sequence()
    {
        using namespace magic_enum;

        constexpr auto helper_func = []<auto ...indices>(IntegerSequence<indices...>)
                {
                        return IntegerSequence<enum_value<E>(indices)...>{};
                };

        return helper_func(make_integer_sequence<enum_count<E>()>());
    }

    template <typename E>
    constexpr auto get_enum_properties()
    {
        using namespace magic_enum;

        constexpr auto helper_func = []<E ...vals>(IntegerSequence<vals...>){return enum_properties<vals...>{};};

        return helper_func(enum_sequence<E>());
    };

    template <typename E, E default_value, E ...enum_values, bool ...flags>
    constexpr auto conditional_max(IntegerSequence<enum_values...>, IntegerSequence<flags...>)
    {
        return enum_properties<(flags ? enum_values : default_value)...>::max;
    }

    template <typename ...Types>
    struct UniqueTypes;
    template <typename T>
    struct UniqueTypes<T>
    {
        using type = T;

        constexpr static int count = 1;

        template <typename ...OtherTypes>
        constexpr auto operator&&(const UniqueTypes<OtherTypes...>& other) const
        {
            if constexpr((std::is_same_v<T, OtherTypes> || ...))
            {
                return UniqueTypes<OtherTypes...>{};
            }
            else
            {
                return UniqueTypes<T, OtherTypes...>{};
            }
        }
    };
    template <typename ...Types>
    struct UniqueTypes
    {
        using type = UniqueTypes;
        using tuple_type = std::tuple<Types...>;
        constexpr static int count = sizeof...(Types);
    };

    template <typename ...Types>
    using GetUniqueTypes = typename decltype((UniqueTypes<Types>{} && ...))::type;

    template <typename T, typename ...Types>
    inline constexpr int count_instances = ((std::is_same_v<T, Types>) + ...);
    template <typename T>
    inline constexpr int count_unique_types = 1;
    template<typename ...Types>
    inline constexpr int count_unique_types<UniqueTypes<Types...>> = sizeof...(Types);

    template <typename UTypes, typename ...Types>
    struct TypeDistribution;
    template <typename ...UTypes, typename ...Types>
    struct TypeDistribution<UniqueTypes<UTypes...>, Types...>
    {
        using type = IntegerSequence<count_instances<UTypes, Types...>...>;
    };
    template <typename T, typename ...Types>
    struct TypeDistribution
    {
        using type = IntegerSequence<sizeof...(Types)>;
    };

    template <typename T, typename I>
    struct TupleOfArrays;
    template <typename ...Types, int ...sizes>
    struct TupleOfArrays<UniqueTypes<Types...>, IntegerSequence<sizes...>>
    {
        using type = std::tuple<Types [sizes]...>;
    };
    template <typename T, int size>
    struct TupleOfArrays<T, IntegerSequence<size>>
    {
        using type = T[size];
    };

    template <typename T>
    struct make_variant;
    template <typename ...Types>
    struct make_variant<UniqueTypes<Types...>>
    {
        using type = std::variant<Types...>;
    };
    template <typename T>
    using make_variant_t = typename make_variant<T>::type;

    template <typename ...Types>
    using typeIDs = IntegerSequence<typeid(Types).hash_code()...>;

    template <typename ...Types>
    struct SeparateTypes
    {
        using unique_types = GetUniqueTypes<Types...>;
        using distribution = typename TypeDistribution<unique_types, Types...>::type;
        using variant = make_variant_t<unique_types>;
        using tuple_of_arrays = TupleOfArrays<unique_types, distribution>;
        using typeids = typeIDs<unique_types>;

        constexpr static int unique_count = count_unique_types<unique_types>;
    };
}

#endif //AFORE_UTILS_H
