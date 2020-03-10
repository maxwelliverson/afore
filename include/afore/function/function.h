//
// Created by maxwell on 2020-02-27.
//

#ifndef AFORE_FUNCTION_H
#define AFORE_FUNCTION_H

#include "afore/primitives.h"
#include "afore/algorithm/algorithm.h"
#include "afore/algorithm/find.h"
#include "afore/algorithm/scatter_gather.h"
#include "afore/algorithm/reduce.h"
#include "afore/detail/function.inl"
#include "decl.h"
#include "def.h"
#include "functors.h"
#include "operators.h"
#include "afore/unit_tests/function.inl"

/*namespace afore
{
    /*template <typename ...Args>
    afore_type arg_list = detail::ArgsList<Args...>;

    template <typename ...Funcs>
    afore_type function_list = detail::FunctionList<Funcs...>;

    template <typename FuncList, typename BoundArgs>
    struct function
    {
        afore_type overload = detail::OverloadedFunction<FuncList>;

        template <typename ...Func>
        constexpr function(Func&&...){}

        template <typename ...Args>
        afore_mem_func operator()(Args&&... args) const noexcept;

        afore_mem_func operator ==(const function& other) const noexcept {return bound_args == other.bound_args;}
        template <typename OtherFList, typename OtherBArgs>
        afore_mem_func operator ==(const function<OtherFList, OtherBArgs>&) const noexcept {return false;}

    protected:

        template <typename ...Args>
        afore_mem_func bind_function(Args&&... args) const noexcept;
        template <typename ...Args>
        afore_mem_func dispatch_function(Args&&... args) const noexcept;
        template <typename arg_types, typename ArgTuple>
        afore_mem_func call_function(ArgTuple&& args) const noexcept;

        to_tuple_t<BoundArgs> bound_args;

        template <typename ...Args>
        constexpr function(std::tuple<Args...>&& args) : bound_args(args){}
        template <typename, typename>
        friend struct function;
    };

    template <Function ...Func>
    function(const Func&...) -> function<function_list<Func...>, empty_type_list>;
    template <Function ...Func>
    function(Func&&...) -> function<function_list<Func...>, empty_type_list>;/*


    inline namespace functors
    {
        namespace lambdas
        {
            constexpr auto identity = []<typename T>(T&& a){return std::forward<T>(a);};
            constexpr auto always_true = [](auto&&...){return true;};
            constexpr auto always_false = [](auto&&...){return false;};
            constexpr auto less_than = [](const auto& a, const auto& b){return a < b;};
            constexpr auto greater_than = [](const auto& a, const auto& b){return a > b;};
            constexpr auto equals = [](const auto& a, const auto& b){return a == b;};
            constexpr auto knot = [](const auto& a){return not a;};

            constexpr auto plus = [](const auto& a, const auto& b){return a + b;};
            constexpr auto minus = [](const auto& a, const auto& b){return a - b;};

            constexpr auto first = [](const auto& p){return p.first;};
            constexpr auto second = [](const auto& p){return p.second;};

            constexpr auto head = []<auto head_, auto ...tail_>(const ValueList<head_, tail_...>){return head_;};
            constexpr auto tail = []<auto head_, auto ...tail_>(const ValueList<head_, tail_...>){return ValueList<tail_...>{};};

            constexpr auto max = [](const auto& a, const auto& b){return (a > b) ? a : b;};
            constexpr auto min = [](const auto& a, const auto& b){return (a < b) ? a : b;};




            /*constexpr auto in =
                []<typename funclist, typename boundargs>
                (const function<funclist, boundargs>& fun) {
                    constexpr auto pack_expander =
                        [](const auto& f, auto&& arg) constexpr {
                            return [&f, &arg]<auto ...indices>(const ValueList<indices...>&&){
                                return f(std::forward<std::tuple_element_t<indices, decltype(arg)>>(std::get<indices>(arg))...);
                            }(make_sequence<std::tuple_size_v<decltype(arg)>>());
                        };
                    using tuple_type = std::tuple<std::remove_cvref_t<decltype(fun)>>;
                    return function<decltype(pack_expander()), ValueList<0>, ValueList<1>, tuple_type>{std::tuple{fun}};
                };/*

            afore_var group_value_list = []<auto ...list>
                (auto&& func, const ValueList<list...>)
                {

                };

            afore_var group_type_list = []<typename ...list>
                (auto&& func, const TypeList<list...>)
                {

                };
        }

        afore_var _ = placeholder{};

        afore_var identity = function(lambdas::identity);
        afore_var always_true = function(lambdas::always_true);
        afore_var always_false = function(lambdas::always_false);
    }
}*/
#endif //AFORE_FUNCTION_H
