//
// Created by maxwell on 2020-03-02.
//

#ifndef AFORE_COMPARE_H
#define AFORE_COMPARE_H

#include "afore/primitives.h"


namespace afore
{
    namespace detail
    {
        template<typename A, typename B>
        struct Compare
        {
            afore_var equals = is_same<A, B>;
            afore_var less_than = sizeof(A) < sizeof(B);
        };
        template <auto A, auto B>
        struct Compare<Value<A>, Value<B>>
        {
            afore_var less_than = A < B;
            afore_var equals = []{
                if constexpr(detail::is_same<decltype(A), decltype(B)>)
                    return A == B;
                else
                    return false;}();
        };
    }

    namespace compare
    {

        template <typename A, typename B>
        afore_var equals = detail::Compare<A, B>::equals;
        template <typename A, typename B>
        afore_var not_equals = !equals<A, B>;
        template <typename A, typename B>
        afore_var less_than = detail::Compare<A, B>::less_than;
        template <typename A, typename B>
        afore_var less_than_or_equals = less_than<A, B> || equals<A, B>;
        template <typename A, typename B>
        afore_var greater_than = !less_than_or_equals<A, B>;
        template <typename A, typename B>
        afore_var greater_than_or_equals = !less_than<A, B>;
    }

}

#endif //AFORE_COMPARE_H
