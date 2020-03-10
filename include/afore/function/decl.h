//
// Created by maxwell on 2020-03-03.
//

#ifndef AFORE_DECL_H
#define AFORE_DECL_H

namespace afore
{
    template <typename ...Args>
    afore_type arg_list = detail::ArgsList<Args...>;

    template <typename ...Funcs>
    afore_type function_list = detail::FunctionList<Funcs...>;

    template <typename FuncList, typename BoundArgs>
    struct function
    {
        afore_type overload = detail::OverloadedFunction<FuncList>;

        constexpr function(const function& other) : bound_args(other.bound_args){}
        constexpr function(function&& other) : bound_args(std::move(other.bound_args)){}
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
    function(Func&&...) -> function<function_list<Func...>, empty_type_list>;



    template <typename FuncList, typename BoundArgs>
    struct delegate : function<FuncList, BoundArgs>
    {
        afore_type base = function<FuncList, BoundArgs>;
        using base::function;

        template <typename ...Args>
        afore_mem_func operator()(Args&& ...args) const noexcept
        {
            return base::operator()(*this, std::forward<Args>(args)...);
        }
    };



    template <Function ...Funcs>
    delegate(const Funcs&...) -> delegate<function_list<Funcs...>, empty_type_list>;
    template <Function ...Funcs>
    delegate(Funcs&&...) -> delegate<function_list<Funcs...>, empty_type_list>;
}

#endif //AFORE_DECL_H
