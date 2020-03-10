//
// Created by maxwell on 2020-02-23.
//

#ifndef AFORE_FOREACH_H
#define AFORE_FOREACH_H

#include "afore/primitives.h"

namespace afore
{
    namespace detail
    {
        template <auto Func, typename List>
        struct ForEachImpl;
        template <auto Func, auto ...values>
        struct ForEachImpl<Func, ValueList<values...>>
        {
            using result = ValueList<Func(values)...>;
        };
    }

    template <auto Func, typename List>
    using for_each_in_list = typename detail::ForEachImpl<Func, List>::result;
    template <auto Func, auto ...Values>
    using for_each_value = typename detail::ForEachImpl<Func, ValueList<Values...>>::result;
}

#endif //AFORE_FOREACH_H
