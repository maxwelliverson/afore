//
// Created by maxwell on 2020-02-23.
//

#ifndef AFORE_TIE_H
#define AFORE_TIE_H

#include "afore/primitives.h"

namespace afore
{
    namespace detail
    {
        template <bool flag, typename ...Lists>
        struct TieImpl;
        template <auto ...list_a, auto ...list_b>
        struct TieImpl<true, ValueList<list_a...>, ValueList<list_b...>>
        {
            using result = ValueList<std::tuple_cat(list_a, std::tie(list_b))...>;
        };
        template <auto ...list_a, auto ...list_b, typename ...tail>
        struct TieImpl<true, ValueList<list_a...>, ValueList<list_b...>, tail...>
        {
            using result = typename TieImpl<true, ValueList<std::tuple_cat(list_a, std::tie(list_b))...>, tail...>::result;
        };
        template <auto ...list_a, auto ...list_b, typename ...tail>
        struct TieImpl<false, ValueList<list_a...>, ValueList<list_b...>, tail...>
        {
            using result = typename TieImpl<true, ValueList<std::make_tuple(list_a, list_b)...>, tail...>::result;
        };


        template <typename ...Lists>
        struct TieDispatch;
        template <auto ...list_a, auto ...list_b>
        struct TieDispatch<ValueList<list_a...>, ValueList<list_b...>>
        {
            using result = ValueList<ValueList<list_a, list_b>{}...>;
        };
        /*template <typename ...Lists>
        struct TieDispatch
        {
            using result = typename TieImpl<false, Lists...>::result;
        };*/


    }

    template <typename ...Lists>
    using tie = typename detail::TieDispatch<Lists...>::result;
}

#endif //AFORE_TIE_H
