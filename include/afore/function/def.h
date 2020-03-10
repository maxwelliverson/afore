//
// Created by maxwell on 2020-03-03.
//

#ifndef AFORE_DEF_H
#define AFORE_DEF_H

namespace afore
{
    template <typename FuncList, typename BoundArgs>
    template <typename ...Args>
    afore_mem_func function<FuncList, BoundArgs>::operator()(Args&&... args) const noexcept
    {
        afore_type arg_list = TypeList<std::remove_cvref_t<Args>...>;
        if constexpr(contains_type<arg_list, placeholder>)
            return bind_function(std::forward<Args>(args)...);
        else
            return dispatch_function(std::forward<Args>(args)...);
    }

    template <typename FuncList, typename BoundArgs>
    template <typename ...Args>
    afore_mem_func function<FuncList, BoundArgs>::dispatch_function(Args&&... args) const noexcept
    {
        if constexpr(detail::is_same<BoundArgs, empty_type_list>)
        {
            afore_type arg_types = arg_list<Args...>;
            to_tuple_t<arg_types> arg_tuple{std::forward<Args>(args)...};
            return call_function<arg_types>(arg_tuple);
        }
        else
        {
            afore_type arg_types = detail::splice_args<BoundArgs, arg_list<Args...>>;
            to_tuple_t<arg_types> arg_tuple = detail::combine_args(bound_args, to_tuple_t<arg_list<Args...>>{std::forward<Args>(args)...});
            return call_function<arg_types>(arg_tuple);
        }
    }

    template <typename Func, typename BoundArgs>
    template <typename ...Args>
    afore_mem_func function<Func, BoundArgs>::bind_function(Args&&... args) const noexcept
    {
        if constexpr(detail::is_same<BoundArgs, empty_type_list>)
        {
            afore_type arg_types = TypeList<Args...>;
            afore_type valid_calls = typename overload::template valid_overloads<arg_list<Args...>>;
            static_assert(valid_calls::size != 0, "Attempted an argument binding such that no viable overloads remain possible.");
            return function<valid_calls, arg_types>(to_tuple_t<arg_types>{std::forward<Args>(args)...});
        }
        else
        {
            afore_type arg_types = detail::splice_args<BoundArgs, arg_list<Args...>>;
            afore_type valid_calls = typename overload::template valid_overloads<arg_types>;
            static_assert(valid_calls::size != 0, "Attempted an argument binding such that no viable overloads remain possible.");
            to_tuple_t<arg_types> arg_tup = detail::combine_args(bound_args, std::tuple{args...});
            return function<valid_calls, arg_types>(std::move(arg_tup));
        }
    }

    template <typename FuncList, typename BoundArgs>
    template <typename arg_types, typename ArgTuple>
    afore_mem_func function<FuncList, BoundArgs>::call_function(ArgTuple&& args) const noexcept
    {
        static_assert(detail::is_same<to_tuple_t<arg_types>, ArgTuple>, "arg_types and ArgTuple were different");
        afore_type valid_calls = typename overload::template valid_overloads<arg_types>;
        static_assert(valid_calls::size != 0, "No viable overload exists.");
        if constexpr(valid_calls::size == 1)
            return std::apply(unwrap_t<valid_calls>{}, args);
        else
        {
            afore_type filter_by_conversions = detail::filter_funcs_by_converted<valid_calls, arg_types>;
            if constexpr(filter_by_conversions::size == 1)
                return std::apply(unwrap_t<filter_by_conversions>{}, args);
            else
            {
                afore_type filter_by_auto = detail::filter_funcs_by_auto<valid_calls, arg_types>;
                if constexpr(filter_by_auto::size == 1)
                    return std::apply(unwrap_t<filter_by_auto>{}, args);
                else
                    return filter_by_auto::size;
            }
        }
    }
}

#endif //AFORE_DEF_H
