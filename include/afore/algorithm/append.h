//
// Created by maxwell on 2020-02-25.
//

#ifndef AFORE_APPEND_H
#define AFORE_APPEND_H

#include "afore/primitives.h"

namespace afore
{
    namespace detail
    {
        namespace zero
        {
            inline constexpr static std::size_t value = 0;
        }

        template <auto Obj, typename List>
        struct AppendImpl;
        template <auto Obj, auto ...list>
        struct AppendImpl<ValueList<list...>>
        {
            using result = ValueList<list..., Obj>;
        };



        template <std::size_t Index, auto Obj, typename List, typename OutputList>
        struct InsertImpl;
        template <auto Obj, auto ...list, auto ...output>
        struct InsertImpl<zero::value, Obj, ValueList<list...>, ValueList<output...>>
        {
            using result = ValueList<output..., Obj, list...>;
        };
        template <std::size_t Index, auto Obj, auto head, auto ...tail, auto ...output>
        struct InsertImpl<Index, Obj, ValueList<head, tail...>, ValueList<output...>>
        {
            using result = typename InsertImpl<Index - 1, Obj, ValueList<tail...>, ValueList<output..., head>>::result;
        };


        template <std::size_t Index, auto Obj, typename List, typename OutputList>
        struct SwapImpl;
        template <auto Obj, auto head, auto ...tail, auto ...output>
        struct SwapImpl<zero::value, Obj, ValueList<head, tail...>, ValueList<output...>>
        {
            using result = ValueList<output..., Obj, tail...>;
        };
        template <std::size_t Index, auto Obj, auto head, auto ...tail, auto ...output>
        struct SwapImpl<Index, Obj, ValueList<head, tail...>, ValueList<output...>>
        {
            using result = typename SwapImpl<Index - 1, Obj, ValueList<tail...>, ValueList<output..., head>>::result;
        };
    }

    template <auto Obj, typename List>
    using append = typename detail::AppendImpl<Obj, List>::result;
    template <auto Index, auto Obj, typename List, std::enable_if_t<Index <= List::size, int> = 0>
    using insert = typename detail::InsertImpl<static_cast<std::size_t>(Index), Obj, List, empty_list>::result;
    template <auto Index, auto Obj, typename List, std::enable_if_t<Index <= List::size, int> = 0>
    using swap = typename detail::SwapImpl<static_cast<std::size_t>(Index), Obj, List, empty_list>::result;

}


#endif //AFORE_APPEND_H
