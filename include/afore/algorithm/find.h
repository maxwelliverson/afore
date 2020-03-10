//
// Created by maxwell on 2020-02-23.
//

#ifndef AFORE_FIND_H
#define AFORE_FIND_H

#include "afore/primitives.h"

namespace afore
{
    namespace detail
    {
        template <typename List, typename Output, typename Obj>
        struct FindImpl;
        template <auto ...output, typename Obj>
        struct FindImpl<empty_list, ValueList<output...>, Obj> :
            AFORE_RESULT_T(ValueList<output...>);
        template <auto ...output, typename Obj>
        struct FindImpl<empty_type_list, ValueList<output...>, Obj> :
            AFORE_RESULT_T(ValueList<output...>);
        template <typename head, typename ...tail, auto ...output, typename Obj>
        struct FindImpl<TypeList<head, tail...>, ValueList<output...>, Obj> :
            AFORE_NEXT(FindImpl<TypeList<tail...>, ValueList<output..., std::is_same_v<head, Obj>>, Obj>);
        template <auto head, auto ...tail, auto ...output, auto Obj>
        struct FindImpl<ValueList<head, tail...>, ValueList<output...>, Value<Obj>> :
            AFORE_NEXT(FindImpl<ValueList<tail...>, ValueList<output..., head == Obj>, Value<Obj>>);


        template <Index Pos, typename List, typename Output, auto Obj>
        struct FindIndicesImpl;
        template <Index Pos, typename Indices, auto key>
        struct FindIndicesImpl<Pos, empty_type_list, Indices, key> :
            AFORE_RESULT_T(Indices);
        template <Index Pos, typename ...tail, Index ...indices, typename KeyType, typevalue_t<KeyType> key>
        struct FindIndicesImpl<Pos, TypeList<KeyType, tail...>, IndexList<indices...>, key> :
            AFORE_NEXT(FindIndicesImpl<Pos + 1, TypeList<tail...>, IndexList<indices..., Pos>, key>);
        template <Index Pos, typename head, typename ...tail, Index ...indices, auto key>
        struct FindIndicesImpl<Pos, TypeList<head, tail...>, IndexList<indices...>, key> :
            AFORE_NEXT(FindIndicesImpl<Pos + 1, TypeList<tail...>, IndexList<indices...>, key>);
        template <Index Pos, Index ...output, auto Obj>
        struct FindIndicesImpl<Pos, empty_list, IndexList<output...>, Obj> :
            AFORE_RESULT_T(IndexList<output...>);
        template <Index Pos, auto ...tail, Index ...output, auto Obj>
        struct FindIndicesImpl<Pos, ValueList<Obj, tail...>, IndexList<output...>, Obj> :
            AFORE_NEXT(FindIndicesImpl<Pos + 1, ValueList<tail...>, IndexList<output..., Pos>, Obj>);
        template <Index Pos, auto head, auto ...tail, Index ...output, auto Obj>
        struct FindIndicesImpl<Pos, ValueList<head, tail...>, IndexList<output...>, Obj> :
            AFORE_NEXT(FindIndicesImpl<Pos + 1, ValueList<tail...>, IndexList<output...>, Obj>);

        template <typename FindMaskResults>
        struct ContainsImpl;
        template <auto ...args>
        struct ContainsImpl<ValueList<args...>> : AFORE_RESULT_V((args || ...));
    }

    template <typename List, auto Obj>
    afore_type find = result_t<detail::FindImpl<List, empty_list, detail::Value<Obj>>>;

    template <typename List, typename T>
    afore_type find_type = result_t<detail::FindImpl<List, empty_list, T>>;

    template <typename List, auto Obj>
    afore_type find_indices = result_t<detail::FindIndicesImpl<0, List, empty_index_list, Obj>>;

    template <typename List, auto Obj>
    afore_var contains = result_v<detail::ContainsImpl<find<List, Obj>>>;

    template <typename List, typename T>
    afore_var contains_type = result_v<detail::ContainsImpl<find_type<List, T>>>;
}

#endif //AFORE_FIND_H
