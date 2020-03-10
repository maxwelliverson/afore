//
// Created by maxwell on 2020-02-23.
//

#ifndef AFORE_SORT_H
#define AFORE_SORT_H

#include "afore/primitives.h"
#include "afore/algorithm/reduce.h"

namespace afore
{
    namespace detail
    {
        template <auto Comp, typename Front, typename Back, typename List, auto pivot>
        struct SortImpl;
        template <auto Comp, typename List>
        struct SortDispatch;

        template <auto Comp>
        struct SortDispatch<Comp, empty_list> :
            AFORE_RESULT_T(empty_list);
        template <auto Comp, typename List>
        struct SortDispatch :
            AFORE_NEXT(SortImpl<Comp, empty_list, empty_list, remove<List, List::pivot>, List::pivot>);

        template <auto Comp, auto pivot>
        struct SortImpl<Comp, empty_list, empty_list, empty_list, pivot> :
            AFORE_RESULT_T(ValueList<pivot>);

        template <auto Comp, auto head, auto pivot>
        struct SortImpl<Comp, empty_list, empty_list, ValueList<head>, pivot> :
            AFORE_RESULT_T(std::conditional_t<Comp(head, pivot), ValueList<head, pivot>, ValueList<pivot, head>>);
        template <auto Comp, auto pivot, auto head, auto ...tail, auto ...front, auto ...back>
        struct SortImpl<Comp, ValueList<front...>, ValueList<back...>, ValueList<head, tail...>, pivot> :
            AFORE_NEXT(std::conditional_t<Comp(head, pivot),
                SortImpl<Comp, ValueList<front..., head>, ValueList<back...>, ValueList<tail...>, pivot>,
                SortImpl<Comp, ValueList<front...>, ValueList<back..., head>, ValueList<tail...>, pivot>>);

        template <auto Comp, typename List>
        using sort_list = result_t<SortDispatch<Comp, List>>;

        template <auto Comp, typename front, typename back, auto pivot>
        struct SortImpl<Comp, front, back, empty_list, pivot> :
            AFORE_RESULT_T(concatenate_lists<sort_list<Comp, front>, ValueList<pivot>, sort_list<Comp, back>>);
    }

    template <auto Comp, typename List>
    using sort_list = detail::sort_list<Comp, List>;
    template <auto Comp, auto ...Values>
    using sort_values = detail::sort_list<Comp, ValueList<Values...>>;

}

#endif //AFORE_SORT_H
