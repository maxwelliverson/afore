//
// Created by maxwell on 2020-03-03.
//

#ifndef AFORE_FUNCTORS_H
#define AFORE_FUNCTORS_H

template <typename T>
struct std::common_type<T, const afore::detail::GenArgType&>
{
    using type = T;
};
template <typename T>
struct std::common_type<const afore::detail::GenArgType&, T>
{
    using type = T;
};
template <>
struct std::common_type<const afore::detail::GenArgType&, const afore::detail::GenArgType&>
{
    using type = afore::detail::GenArgType;
};


namespace afore
{
    inline namespace functors
    {
        namespace lambdas
        {
            constexpr auto identity = []<typename T>(T&& a){return std::forward<T>(a);};

            constexpr auto conditional =
                [](bool cond, auto&& a, auto&& b) -> std::common_type_t<decltype(a), decltype(b)>
                {
                    return cond ? PERF_FWD(a) : PERF_FWD(b);
                };

            constexpr auto always_true = [](auto&&...){return true;};
            constexpr auto always_false = [](auto&&...){return false;};
            constexpr auto less_than = [](const auto& a, const auto& b){return a < b;};
            constexpr auto greater_than = [](const auto& a, const auto& b){return a > b;};
            constexpr auto equals = [](const auto& a, const auto& b){return a == b;};
            constexpr auto knot = [](const auto& a){return not a;};

            constexpr auto plus = [](auto&& a, auto&& b){return a + b;};
            constexpr auto minus = [](auto&& a, auto&& b){return a - b;};
            constexpr auto mul = [](auto&& a, auto&& b){return a*b;};
            constexpr auto div = [](auto&& a, auto&& b){return a/b;};
            constexpr auto mod = [](auto&& a, auto&& b){return a % b;};

            constexpr auto first = []<PairType P>(P&& p){return std::get<0>(PERF_FWD(p));};
            constexpr auto first_value = []<auto head_, auto ...tail_>(const ValueList<head_, tail_...>){return head_;};
            constexpr auto first_type = []<typename head_, typename ...tail_>(const TypeList<head_, tail_...>){return detail::TypeValue<head_>();};
            constexpr auto second = []<PairType P>(P&& p){return std::get<1>(PERF_FWD(p));};

            constexpr auto head = []<auto head_, auto ...tail_>(const ValueList<head_, tail_...>){return head_;};
            constexpr auto tail = []<auto head_, auto ...tail_>(const ValueList<head_, tail_...>){return ValueList<tail_...>{};};

            constexpr auto max = [](auto&& a, auto&& b){return conditional(a > b, PERF_FWD(a), PERF_FWD(b));};
            constexpr auto min = [](const auto& a, const auto& b){return conditional(a < b, PERF_FWD(a), PERF_FWD(b));};

            constexpr auto del_identity = [](auto&& func, auto&& I){return std::forward<decltype(I)>(I);};
            constexpr auto max_del = [](auto&& func, auto&& item_head, auto&& item_neck, auto&& ...items){return max(item_head, func(item_neck, items...));};
            constexpr auto min_del = [](auto&& func, auto&& item_head, auto&& item_neck, auto&& ...items){return min(item_head, func(item_neck, items...));};

            constexpr auto sum_del = [](auto&& func, auto&& item_head, auto&& item_neck, auto&& ...items){return item_head + func(item_neck, items...);};



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
                };*/

            afore_var group_value_list = []<auto ...list>
            (auto&& func, const ValueList<list...>)
            {

            };

            afore_var group_type_list = []<typename ...list>
            (auto&& func, const TypeList<list...>)
            {

            };

            afore_var curry_op = []<typename flA, typename baA, typename flB, typename baB>
            (const function<flA, baA>& func_a, const function<flB, baB>& func_b, auto&& ...args)
            {
                return func_a(func_b(PERF_FWD(args)...));
            };

            afore_var pipe = []<typename flA, typename baA, typename flB, typename baB>
            (const function<flA, baA>& func_a, const function<flB, baB>& func_b, auto&& ...args)
            {
                return func_b(func_a(PERF_FWD(args)...));
            };
        }

        afore_var _ = placeholder{};

        afore_var identity = function(lambdas::identity);
        afore_var always_true = function(lambdas::always_true);
        afore_var always_false = function(lambdas::always_false);

        afore_var equals = function(lambdas::equals);

        afore_var negate = function(lambdas::knot);

        afore_var plus = function(lambdas::plus);
        afore_var minus = function(lambdas::minus);
        afore_var mul = function(lambdas::mul);
        afore_var div = function(lambdas::div);
        afore_var mod = function(lambdas::mod);

        afore_var first = function(lambdas::first, lambdas::first_value, lambdas::first_type);
        afore_var second = function(lambdas::second);

        afore_var max = delegate(lambdas::del_identity, lambdas::max_del);
        afore_var min = delegate(lambdas::del_identity, lambdas::min_del);

        afore_var sum = delegate(lambdas::del_identity, lambdas::sum_del);

        afore_var curry = function(lambdas::curry_op);
        afore_var pipe = function(lambdas::pipe);
    }
}

namespace afore::detail::test
{
    template <typename F, typename ...Args, bool B = std::is_invocable_v<F, Args...>>
    afore_var extract_func(const TypeList<Args...>&&){return true;}
    template <typename F, typename T>
    afore_var extract_func(T){return false;}
    template <typename A, typename B>
    struct Resolve
    {
        using T = std::unwrap_ref_decay_t<decltype(false ? std::declval<A>() : std::declval<B>())>;
    };

    template <auto F, typename ...Args>
    afore_var check_valid = extract_func<decltype(F)>(TypeList<Args...>());

    template <typename F, typename ...Args>
    concept ValidCall = requires(F f, Args ...args)
    {
        f(args...);
    };

    template <typename F, typename ...Args>
    struct ValidCaller;
    template <typename ...Args, ValidCall<Args...> func>
    struct ValidCaller<func, Args...> :
    AFORE_RESULT_TRUE;
    template <typename F, typename ...Args>
    struct ValidCaller :
    AFORE_RESULT_FALSE;

    template <auto F, typename ...Args>
    afore_var val_call = result_v<ValidCaller<decltype(F), Args...>>;


    template <typename func, template <typename ...> typename Types, typename ...Args, typename = decltype(std::declval<func>()(std::declval<Args>()...), int())>
    inline constexpr static bool valid_call(const Types<Args...>)
    {
        return true;
    }

    template <typename func, typename args>
    afore_var valid_call(const args)
    {
        return false;
    }

    template <const auto& func, typename ...Args>
    afore_var is_valid_call = valid_call<decltype(func)>(TypeList<Args...>());
}

#endif //AFORE_FUNCTORS_H
