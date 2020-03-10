//
// Created by maxwell on 2020-03-03.
//

#ifndef AFORE_OPERATORS_H
#define AFORE_OPERATORS_H

namespace afore
{
    inline namespace operators
    {
        inline namespace function_operators
        {
            template <typename FuncListA, typename BoundArgsA, typename FuncListB, typename BoundArgsB>
            afore_var operator|(const function<FuncListA, BoundArgsA>& func_a, const function<FuncListB, BoundArgsB>& func_b)
            {
                return pipe(func_a, func_b, _);
            }
        }
    }
}

#endif //AFORE_OPERATORS_H
