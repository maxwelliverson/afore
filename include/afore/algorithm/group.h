//
// Created by maxwell on 2020-02-25.
//

#ifndef AFORE_GROUP_H
#define AFORE_GROUP_H

#include "afore/primitives.h"
#include "append.h"
#include "find.h"

namespace afore
{
    namespace detail
    {
        template <auto func, typename List, typename OutputKeys, typename OutputList>
        struct GroupImpl;
        template <typename OutputKeys, typename OutputList, auto func>
        struct GroupImpl<func, empty_list, OutputKeys, OutputList>
        {
            using result = OutputList;
        };
        template <auto func, auto head, auto ...list, auto ...keys, auto ...lists>
        struct GroupImpl<func, ValueList<head, list...>, ValueList<keys...>, ValueList<lists...>>
        {
            inline constexpr static auto key = func(head);
            using key_list = ValueList<keys...>;
            using output_lists = ValueList<lists...>;
            using l = ValueList<lists...>;
            inline constexpr static auto is_new_key = !contains<key_list, key>;
            using result = decltype([]
                    {
                        if constexpr(is_new_key)
                        {
                            constexpr auto list_index = sizeof...(keys);
                            using new_keys = append<key, key_list>;
                            using new_output = append<ValueList<head>{}, l>;
                            return typename GroupImpl<func, ValueList<list...>, new_keys, new_output>::result{};
                        }
                        else
                        {
                            constexpr auto list_index = std::get<0>(find_indices<key_list, key>{});
                            using new_output = swap<list_index, append<head, type_of<std::get<list_index>(l)>>, l>;
                            return typename GroupImpl<func, ValueList<list...>, key_list, new_output>::result{};
                        }
                    }());
        };
    }

    template <typename List, auto func>
    using group = typename detail::GroupImpl<func, List, empty_list, empty_list>::result;
}

#endif //AFORE_GROUP_H
