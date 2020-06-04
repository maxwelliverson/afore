//
// Created by maxwell on 2020-02-22.
//

#ifndef AFORE_ALGORITHM_H
#define AFORE_ALGORITHM_H

#include "afore/primitives.h"
//#include "function_objs.h"

namespace afore
{
    namespace detail::impl{

        template<auto Func, auto state, typename List>
        struct UnaryFoldLeft;
        template<auto Func, auto state>
        struct UnaryFoldLeft<Func, state, empty_list> :
            AFORE_RESULT_V(state);
        template<auto Func, auto state, auto head, auto ...tail>
        struct UnaryFoldLeft<Func, state, ValueList <head, tail...>> :
            AFORE_NEXT(UnaryFoldLeft<Func, Func(state, head), ValueList <tail...>>);


        template<auto Func, auto state, typename List>
        struct BinaryFoldLeft;
        template<auto Func, auto state, auto last>
        struct BinaryFoldLeft<Func, state, ValueList < last>> :
            AFORE_RESULT_V(state);
        template<auto Func, auto state, auto head, auto neck, auto ...tail>
        struct BinaryFoldLeft<Func, state, ValueList < head, neck, tail...>> :
            AFORE_NEXT(BinaryFoldLeft<Func, Func(state, head, neck), ValueList < neck, tail...>>);


        template <auto Func, typename List, typename Output>
        struct ForEach;
        template <auto Func, EmptyList empty, typename Output>
        struct ForEach<Func, empty, Output> :
            AFORE_RESULT_T(Output);
        template <auto Func, NonEmptyList list, ValueListType output>
        struct ForEach<Func, list, output> :
            AFORE_NEXT(ForEach<Func, tail<list>, concatenate_lists<output, ValueList<Func(unwrap<head<list>>)>>>);
        template <auto Func, NonEmptyList list, TypeListType output>
        struct ForEach<Func, list, output> :
            AFORE_NEXT(ForEach<Func, tail<list>, concatenate_lists<output, TypeList<decltype(Func(unwrap_t<head<list>>{}))>>>);

        template <auto Func, typename>
        struct ForEachDispatch;
        template <auto Func, TypeListType List>
        struct ForEachDispatch<Func, List> :
            AFORE_RESULT_T(ForEach<Func, List, empty_type_list>);
        template <auto Func, ValueListType List>
        struct ForEachDispatch<Func, List> :
            AFORE_RESULT_T(ForEach<Func, List, empty_list>);
    }
    template <auto Func, auto state, typename List>
    afore_var unary_fold_left = result_v<detail::impl::UnaryFoldLeft<Func, state, List>>;
    template <auto Func, auto state, typename List>
    afore_var binary_fold_left = result_v<detail::impl::BinaryFoldLeft<Func, state, List>>;

    template <auto Func, AforeListType List>
    afore_type for_each = result_t<detail::impl::ForEachDispatch<Func, List>>;

    template <auto Func, TypeListType List>
    afore_type for_each_val = result_t<detail::impl::ForEach<Func, List, empty_list>>;

    /*namespace detail::impl {
        template<auto, auto, auto, typename, bool, typename>
        struct GenerateList;
        template<auto NextState, auto Predicate, auto Transform, typename state, typename Result>
        struct GenerateList<NextState, Predicate, Transform, state, false, Result> :
            AFORE_RESULT_T(Result);
        template<auto Next, auto Predicate, auto Transform, auto state, auto ...Vals>
        struct GenerateList<Next, Predicate, Transform, Value < state>, true, ValueList<Vals...>> :
            AFORE_NEXT(GenerateList<Next, Predicate, Transform, Value < Next(state)>, Predicate(Next(state)),
               ValueList < Vals..., Transform(state) >>);
        template<auto Next, auto Predicate, auto Transform, typename state, typename ...types>
        struct GenerateList<Next, Predicate, Transform, state, true, TypeList < types...>> :
        AFORE_NEXT(GenerateList<Next, Predicate, Transform, result_t <ApplyImpl<Value<Next>, TypeList <state>>>,
            result_t < ApplyImpl < Value<Predicate>, result_t < ApplyImpl < Next, TypeList < state >> >> >,
            TypeList < types..., result_t < ApplyImpl < Transform, TypeList < state >> >> >);
    }



    template <auto initial_state, auto NextFunc, auto PredicateFunc, auto TransformFunc = func::identity>
    using generate_list = result_t<GenerateList<NextFunc, PredicateFunc, TransformFunc, initial_state, PredicateFunc(initial_state), empty_list>>;
*/
    /*template <typename, typename, auto ...>
    struct TransformList;
    template <auto Transform, auto Predicate, auto ...results>
    struct TransformList<empty_list, ValueList<results...>, Transform, Predicate>
    {
        using result = ValueList<results...>;
    };
    template <auto Transform, auto Predicate, auto head, auto ...tail, auto ...results>
    struct TransformList<ValueList<head, tail...>, ValueList<results...>, Transform, Predicate>
    {
        using result = typename TransformList<
                                    ValueList<
                                        tail...
                                    >,
                                    std::conditional_t<
                                        Predicate(head),
                                        ValueList<
                                            results...,
                                            Transform(head)
                                        >,
                                        ValueList<
                                            results...,
                                            head
                                        >
                                    >,
                                    Transform,
                                    Predicate
                                >::result;
    };

    template <typename List>
    struct ToArrayImpl;
    template <typename T, T ...args>
    struct ToArrayImpl<ValueList<args...>>
    {};
    template <auto ...args>
    struct ToArrayImpl<ValueList<args...>>
    {
        static_assert(binary_fold_left<[](const auto& state, const auto& A, const auto& B)
                    {
                        return state && std::is_same_v<decltype(A), decltype(B)>;
                    }, true, ValueList<args...>>, "to_array is only defined on homogeneous ConstantSequences.");
    };
    template <typename T>
    struct ToArrayImpl
    {
        static_assert(is_value_list<T>, "to_array is only defined on ConstantSequences.");
    };


    template <typename List>
    inline constexpr static bool is_homogeneous = binary_fold_left<[](const auto& state, const auto& A, const auto& B)
    {
        return state && std::is_same_v<decltype(A), decltype(B)>;
    }, true, List>;


    template <typename Seq>
    constexpr auto to_array = [](Seq s)
    {
        static_assert(is_value_list<Seq>, "to_array is only defined on ConstantSequences.");
        static_assert(is_homogeneous<Seq>, "to_array is only defined on homogeneous ConstantSequences.");
    }();

    template <typename T, T ...vals>
    constexpr auto to_array<ValueList<vals...>> = [] < auto ...index>(ValueList<index...>)
    {
        return std::array{ValueList<vals...>::template get<index>()...};
    }(make_sequence<sizeof...(vals)>{});
*/
    /*template <auto Transform, auto ...list>
    using transform_list = typename TransformList<Transform>*/
    namespace detail::impl
    {
        template <auto Obj, auto size, typename Result>
        struct FilledList;

        template <>
        struct FilledList<true, 0, ValueList<>>{
            using result = ValueList<>;
        };

        template <auto Obj, auto ...rest>
        struct FilledList<Obj, 0, ValueList<rest...>> :
            AFORE_RESULT_T(ValueList<rest...>);

        template <auto Obj, auto size, auto ...rest> requires (size > 0)
        struct FilledList<Obj, size, ValueList<rest...>> :
            AFORE_NEXT(FilledList<Obj, size - 1, ValueList<rest..., Obj>>);





        template <ValueListType list, Index size>
        struct ToArray;
        template <typename T, T ...vals>
        struct ToArray<ValueList<vals...>, IndexNegOne> :
            AFORE_RESULT_V(std::array{vals...});
        template <typename T, T ...vals, Index I>
        struct ToArray<ValueList<vals...>, I> :
            AFORE_RESULT_V(std::array<T, static_cast<std::size_t>(I)>{vals...});
        template <template <auto...> typename TT, auto ...vals, Index I>
        struct ToArray<TT<vals...>, I>
        {
            static_assert(TT<vals...>::failure, "Attempted to make an array out of a heterogeneous sequence.");
        };
    }
    template <auto Obj, auto size>
    afore_type filled_list = result_t<detail::impl::FilledList<Obj, size, empty_list>>;

    template <ValueListType homogeneous_list, Index size = IndexNegOne>
    afore_var to_array = result_v<detail::impl::ToArray<homogeneous_list, size>>;
    template <ValueListType homogeneous_list, Index size, auto default_obj>
    afore_var to_array_with_default = result_v<detail::impl::ToArray<concatenate_lists<homogeneous_list, filled_list<default_obj, size - size_t(homogeneous_list::size)>>, size>>;
}

#endif //AFORE_ALGORITHM_H
