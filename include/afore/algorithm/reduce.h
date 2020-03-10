//
// Created by maxwell on 2020-02-23.
//

#ifndef AFORE_REDUCE_H
#define AFORE_REDUCE_H

#include "afore/primitives.h"
#include "foreach.h"
#include "find.h"

namespace afore::detail
{

    template <auto Predicate, typename List, typename Result>
    struct ReduceImpl;

    template <auto Predicate, auto ...output>
    struct ReduceImpl<Predicate, empty_list, ValueList<output...>> :
        AFORE_RESULT_T(ValueList<output...>);

    template <auto Predicate, auto head, auto ...tail, auto ...output>
    struct ReduceImpl<Predicate, ValueList<head, tail...>, ValueList<output...>> :
        AFORE_NEXT(ReduceImpl<Predicate,ValueList<tail...>, std::conditional_t<Predicate(head), ValueList<output...>, ValueList<output..., head>>>);

    template <auto Position, typename List, typename Output, typename Indices>
    struct RemoveAtImpl;
    template <auto pos, typename output, auto ...indices>
    struct RemoveAtImpl<pos, empty_list, output, ValueList<indices...>> :
        AFORE_RESULT_T(output);
    template <Index pos, auto head, auto ...tail, auto ...output, typename Indices>
    struct RemoveAtImpl<pos, ValueList<head, tail...>, ValueList<output...>, Indices> :
        AFORE_NEXT(RemoveAtImpl<pos + 1,ValueList<tail...>,if_else_t<contains<Indices, pos>,ValueList<output...>,ValueList<output..., head>>,Indices>);

    template <typename List, typename Output, auto Obj, int Count>
    struct RemoveImpl;
    template <typename Output, auto obj, int count>
    struct RemoveImpl<empty_list, Output, obj, count> :
        AFORE_RESULT_T(Output);
    template <auto head, auto ...list, auto ...output, auto obj>
    struct RemoveImpl<ValueList<head, list...>, ValueList<output...>, obj, 0> :
        AFORE_RESULT_T(ValueList<output..., head, list...>);
    template <auto ...tail, auto ...output, auto obj, int count>
    struct RemoveImpl<ValueList<obj, tail...>, ValueList<output...>, obj, count> :
        AFORE_NEXT(RemoveImpl<ValueList<tail...>, ValueList<output...>, obj, count - 1>);
    template <auto head, auto ...tail, auto ...output, auto obj, int count>
    struct RemoveImpl<ValueList<head, tail...>, ValueList<output...>, obj, count> :
        AFORE_NEXT(RemoveImpl<ValueList<tail...>, ValueList<output..., head>, obj, count>);
}

namespace afore
{
    template <auto Predicate, typename List>
    using reduce = result_t<detail::ReduceImpl<Predicate, List, empty_list>>;

    template <typename List, Index I>
    using remove_at = result_t<detail::RemoveAtImpl<0, List, empty_list, IndexList<I>>>;
    template <typename List, typename Indices>
    using remove_at_indices = result_t<detail::RemoveAtImpl<0, List, empty_list, Indices>>;

    template <typename List, auto Obj, int N>
    using remove_n = result_t<detail::RemoveImpl<List, empty_list, Obj, N>>;
    template <typename List, auto Obj>
    using remove = remove_n<List, Obj, 1>;
    template <typename List, auto Obj>
    using remove_all = remove_n<List, Obj, -1>;
}

#endif //AFORE_REDUCE_H
