namespace afore::detail
{
    /**
         * \section Function Dispatch Utilities
         *
         * Why do we need explicit overload dispatch?
         * Take for example this code:
         * \code
         *      auto fd = [](double){\/*Do Stuff*\/};
         *      auto ff = [](float){\/*Do Stuff*\/};
         *      auto fa = [](auto){\/*Do stuff*\/};
         *
         *      template <typename ...Funcs>
         *      struct Overloads : Funcs...
         *      {
         *          Overloads(const Funcs&...){}
         *          using Funcs::operator()...;
         *      };
         *      int main()
         *      {
         *          auto F = Overloads(ff, fd, fa);
         *          F(1.0);
         *      }
         *
         * \endcode
         * While one might assume that the invocation \code F(1.0) \endcode would
         * call fd, attempting to compile this code would actually result in an
         * ambiguity error. As of C++20, the compiler is only able to resolve a
         * call to a function that has been overloaded by multiple inheritence if
         * there is only one such overload that would make for a valid call - even
         * if normal overload resolution would seemingly work.
         * Given the above example, \code ff(1.0); fd(1.0); fa(1.0); \endcode would
         * all be valid invocations, and we are thus treated with a compilation error.
         * Lovely.
         * The primary two guidelines Afore uses for dispatch are as follows, in order:
         *
         *      -> Function calls that wouldn't require any type conversion/promotion
         *         of their arguments are prefered over those that would.
         *      -> Function calls whose declared argument types are not generic are
         *         prefered over those whose are.
         *
         * Looking back at the above example while following these principles would
         * yield the ordering \code
         *      fd(1.0);
         *      fa(1.0);
         *      ff(1.0);
         * \endcode , which is exactly the ordering the standard C++ overload
         * resolution rules yields.
         *
         */

    /**
     * \struct GenArgType
     * \fn template <typename T> operator T() const noexcept;
     * Generic argument type used for type matching partially evaluated template functions
     * to ensure the bound arguments would be valid. Not automatically compatible with
     * all constrained types, explicit specializations of the generic conversion operator
     * must be declared for a type that satisfies any concept one wishes to match against.
     * The specializations need not be defined, as the conversion operator is only called in
     * unevaluated contexts.
     *
     * Predeclared specializations exist for some of the core types used in Afore, including
     *      ValueList<>         ->          Variadic ValueLists.
     *      ValueList<0>        ->          ValueLists that are required to not be empty.
     *      TypeList<>          ->          Variadic TypeLists.
     *      TypeList<void>      ->          TypeLists that are required to not be empty.
     *      Index               ->          Index types.
     *      Value<0>            ->          Value types.
     *
     * Actual evaluation of a function call with GenArgType arguments is undefined, though should result
     * in a compilation error (given the declared but undefined conversion function).
     */
    struct GenArgType
    {
        template <typename T>
        constexpr operator T() const noexcept;
    };

    inline namespace GenArgOperators
    {
        /**
         * \section ADL GenArg operators
         *
         *
         */
        template <typename T, typename T2>
        inline constexpr static GenArgType operator+(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static GenArgType operator-(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static GenArgType operator*(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static GenArgType operator/(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static GenArgType operator%(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static bool operator>(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static bool operator<(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static bool operator==(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static bool operator!=(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static bool operator<=(T&&, T2&&);
        template <typename T, typename T2>
        inline constexpr static bool operator>=(T&&, T2&&);
    }


    /**
     * Null Argument Type, used to detect templated/auto arguments of a potential function call.
     * The idea here is that, given a function F that can be invoked with a given set of
     * arguments types A, a boolean mask can be generated with 1s indicating the positions
     * of generic argument types in F's type signature. This is done by replacing each element
     * of A with NullArgType one at a time, each time determining whether or not F could be
     * called with the new set of argument types. Given that NullArgType cannot be implicitly
     * converted to any other type and that no functions are defined specifically on NullArgType,
     * the only circumstance in which F should be invocable with the Nth type in A having been
     * replaced by NullArgType is when the Nth argument type in the signature of F is generic.
     *
     * @warning Do not define any functions on NullArgType. Doing so shouldn't break anything for
     * any other function, but it will lead to unpredictable behaviour. There also shouldn't be
     * any need to do so in the first place.
     *
     */
    struct NullArgType
    {
        constexpr NullArgType() = default;
    };
    /**
     * Just Argument Type. Similar to NullArgType in that it is used to generate
     * a mask on an argument set A with which a given function F can be invoked,
     * though the mask generated indicates the positions of A where implicit
     * conversions/promotion would occur during invocation.
     * This is accomplished by a clever bit of machinery whereby JustArgType is
     * templated on type T, and has a conversion operator declared that is
     * templated on type U, but this operator will always be explicit (and thus
     * unusable for purposes of implicit conversion), except when T and U are the
     * same type. Lets take the Nth type of A and call it Tp. Were you to replace
     * the Nth type in A with JustArgType<Tp>, the resulting set would only be
     * a valid invocation set for F if Tp is the exact same type as the Nth arg
     * type in F's type signature, as Tp is the only type to which JustArgType<Tp>
     * is implicitly convertable. Of course, it would also be valid if the Nth arg
     * type is generic, however generic arg types can be detected with NullArgType
     * as outlined above.
     * At first it seems as though it would be simpler to have JustArgType<Tp>'s
     * conversion operator not be templated, but rather be declared only for Tp.
     * \code
     *      template <typename Tp>
     *      struct JustArgType
     *      {
     *          constexpr operator Tp() const noexcept;
     *      };
     * \endcode
     * At first glance, this seems as though it would work, but it actually doesn't,
     * as it is incapable of detecting promotion or narrowing of builtin types. Say
     * you wish to invoke a function with a double, and that function has two
     * overloads, one that takes a float, and one that takes a double. If JustArgType
     * were defined as it is in that example,\code std::is_invocable \endcode would
     * return true for both overloads. This is because conversion of builtins happens
     * independantly from user-defined conversion. It would detect that JustArgType
     * can be converted to a double, which can then be passed to the function as a
     * float. By making the conversion operator templated, it will instantiate the
     * conversion operator directly to float, but because the operator is only
     * allowed to be implicitly used to convert directly to a double (the type upon
     * which JustArgType is templated), the implicit conversion will fail and
     * \code std::is_invocable \endcode will return false.
     */
    template <typename T>
    struct JustArgType
    {
        template <typename U>
        constexpr explicit(!std::is_same_v<T, U>) operator U() const noexcept;
    };

    /*template <>
    constexpr GenArgType::operator ValueList<>() const noexcept;
    template <>
    constexpr GenArgType::operator ValueList<0>() const noexcept;
    template <>
    constexpr GenArgType::operator TypeList<>() const noexcept;
    template <>
    constexpr GenArgType::operator TypeList<void>() const noexcept;
    template <>
    constexpr GenArgType::operator Index() const noexcept;
    template <>
    constexpr GenArgType::operator Value<0>() const noexcept;*/



    template <typename ...Args>
    afore_type ArgsList = TypeList<Args...>;
    template <Function ...F>
    afore_type FunctionList = TypeList<F...>;

    namespace impl
    {
        template <typename funcs, typename args, typename result>
        struct ArgViabilityFilter
        {
            static_assert(std::is_same_v<typename funcs::Failure, void>, "ArgViabilityFilter was called with incompatible types.");
        };
        template <typename Args, typename Result>
        struct ArgViabilityFilter<empty_type_list, Args, Result> :
            AFORE_RESULT_T(Result);

        template <typename ...funcs, typename ...args, CalledFunctionType<ArgsList<args...>> head, typename ...results>
        struct ArgViabilityFilter<FunctionList<head, funcs...>, ArgsList<args...>, FunctionList<results...>> :
            AFORE_NEXT(ArgViabilityFilter<FunctionList<funcs...>, ArgsList<args...>, FunctionList<results..., head>>);

        template <typename head, typename ...funcs, typename Args, typename Results>
        struct ArgViabilityFilter<FunctionList<head, funcs...>, Args, Results> :
            AFORE_NEXT(ArgViabilityFilter<FunctionList<funcs...>, Args, Results>);


        template <typename func, typename head_args, typename tail_args, typename results>
        struct ArgConversionMask;
        template <typename func, typename args, typename results>
        struct ArgConversionMask<func, args, empty_type_list, results> :
            AFORE_RESULT_T(results);
        template <typename func, typename ...front_args, typename head, typename ...tail, bool ...results>
        struct ArgConversionMask<func, ArgsList<front_args...>, ArgsList<head, tail...>, ValueList<results...>> :
            AFORE_NEXT(ArgConversionMask<func, ArgsList<front_args..., head>, ArgsList<tail...>, ValueList<results..., valid_call<func>(ArgsList<front_args..., JustArgType<head>, tail...>{})>>);



        template <typename func, typename head_args, typename tail_args, typename results>
        struct ArgAutoMask;
        template <typename func, typename args, typename results>
        struct ArgAutoMask<func, args, empty_type_list, results> :
            AFORE_RESULT_T(results);
        template <typename func, typename ...front_args, typename head, typename ...tail, bool ...results>
        struct ArgAutoMask<func, ArgsList<front_args...>, ArgsList<head, tail...>, ValueList<results...>> :
            AFORE_NEXT(ArgAutoMask<func, ArgsList<front_args..., head>, ArgsList<tail...>, ValueList<results..., valid_call<func>(ArgsList<front_args..., NullArgType, tail...>{})>>);



        template <typename Args, typename Replacement, typename Result>
        struct ReplaceArgs;
        template <typename Arg, TypeListType Result>
        struct ReplaceArgs<empty_type_list, Arg, Result> :
            AFORE_RESULT_T(Result);
        template <typename ...tail, NonListType Arg, typename ...results>
        struct ReplaceArgs<ArgsList<placeholder_t, tail...>, Arg, ArgsList<results...>> :
            AFORE_NEXT(ReplaceArgs<ArgsList<tail...>, Arg, ArgsList<results..., Arg>>);
        template <typename head, typename ...tail, NonListType Arg, typename ...results>
        struct ReplaceArgs<ArgsList<head, tail...>, Arg, ArgsList<results...>> :
            AFORE_NEXT(ReplaceArgs<ArgsList<tail...>, Arg, ArgsList<results..., head>>);



        template <typename BoundArgs, typename UnboundArgs, typename Output>
        struct SpliceArgs;
        template <TypeListType Result>
        struct SpliceArgs<empty_type_list, empty_type_list, Result> :
            AFORE_RESULT_T(Result);
        template <typename ...tail, typename ...results>
        struct SpliceArgs<ArgsList<placeholder_t, tail...>, empty_type_list, ArgsList<results...>> :
            AFORE_NEXT(SpliceArgs<ArgsList<tail...>, empty_type_list, ArgsList<results...>>);
        template <typename head_t, typename ...tail_t, typename ...results>
        struct SpliceArgs<empty_type_list, ArgsList<head_t, tail_t...>, ArgsList<results...>> :
            AFORE_NEXT(SpliceArgs<empty_type_list, ArgsList<tail_t...>, ArgsList<results..., head_t>>);
        template <typename ...tail, typename headarg, typename ...args, typename ...results>
        struct SpliceArgs<ArgsList<placeholder_t, tail...>, ArgsList<headarg, args...>, ArgsList<results...>> :
            AFORE_NEXT(SpliceArgs<ArgsList<tail...>, ArgsList<args...>, ArgsList<results..., headarg>>);
        template <typename head, typename ...tail, typename ...args, typename ...results>
        struct SpliceArgs<ArgsList<head, tail...>, ArgsList<args...>, ArgsList<results...>> :
            AFORE_NEXT(SpliceArgs<ArgsList<tail...>, ArgsList<args...>, ArgsList<results..., head>>);


        constexpr auto add_func = [](auto a, auto b){return a + b;};
        template <ValueListType bools>
        afore_var count_true = Index{unary_fold_left<add_func, 0, bools>};

        template <typename List, Index Max>
        struct FindMax;
        template <Index Max>
        struct FindMax<empty_list, Max> :
            AFORE_RESULT_V(Max);
        template <auto head, Index ...Objs, Index Max>
        struct FindMax<ValueList<head, Objs...>, Max> :
            AFORE_NEXT(FindMax<ValueList<Objs...>, (head > Max) ? head : Max>);


        template <typename List, Index Max>
        struct FindMin;
        template <Index Min>
        struct FindMin<empty_list, Min> :
            AFORE_RESULT_V(Min);
        template <auto head, Index ...Objs, Index Min>
        struct FindMin<ValueList<head, Objs...>, Min> :
            AFORE_NEXT(FindMax<ValueList<Objs...>, (head < Min) ? head : Min>);

        template <typename List, typename Vals, typename Output, Index Max>
        struct CullNonMax;
        template <typename Output, Index Max>
        struct CullNonMax<empty_type_list, empty_list, Output, Max> :
            AFORE_RESULT_T(Output);
        template <typename funchead, typename ...functail, Index ...count_tail, typename ...func_results, Index Max>
        struct CullNonMax<FunctionList<funchead, functail...>, ValueList<Max, count_tail...>, FunctionList<func_results...>, Max> :
            AFORE_NEXT(CullNonMax<FunctionList<functail...>, ValueList<count_tail...>, FunctionList<func_results..., funchead>, Max>);
        template <typename funchead, typename ...functail, Index count_head, Index ...count_tail, typename ...func_results, Index Max>
        struct CullNonMax<FunctionList<funchead, functail...>, ValueList<count_head, count_tail...>, FunctionList<func_results...>, Max> :
            AFORE_NEXT(CullNonMax<FunctionList<functail...>, ValueList<count_tail...>, FunctionList<func_results...>, Max>);


        template <template <typename> typename Filter, TypeListType funcs, ValueListType List>
        struct ForEachFunction;
        template <template <typename> typename Filter, ValueListType List>
        struct ForEachFunction<Filter, empty_type_list, List> :
            AFORE_RESULT_T(List);
        template <template <typename> typename Filter, typename F, typename ...Tail, Index ...I>
        struct ForEachFunction<Filter, FunctionList<F, Tail...>, ValueList<I...>> :
            AFORE_NEXT(ForEachFunction<Filter, FunctionList<Tail...>, ValueList<I..., count_true<Filter<F>>>>);

        template <template <typename> typename Filter, TypeListType funcs>
        afore_type for_each_function = result_t<ForEachFunction<Filter, funcs, empty_list>>;
    }

    template <typename functions, typename args>
    afore_type valid_functions = result_t<impl::ArgViabilityFilter<functions, args, empty_type_list>>;

    template <typename args>
    afore_type make_generic_args = result_t<impl::ReplaceArgs<args, GenArgType, empty_type_list>>;

    template <TypeListType bound_args, TypeListType new_args>
    afore_type splice_args = result_t<impl::SpliceArgs<bound_args, new_args, empty_type_list>>;

    namespace impl
    {
        template <typename func, TypeListType args>
        afore_type find_converted_args = result_t<impl::ArgConversionMask<func, empty_type_list, args, empty_list>>;

        template <typename func, TypeListType args>
        afore_type find_generic_args = result_t<impl::ArgAutoMask<func, empty_type_list, args, empty_list>>;

        template <typename functions, typename args>
        struct FilterFuncsByConverted
        {
            template <typename F>
            afore_type filter = find_converted_args<F, args>;
            afore_type ranked_funcs = for_each_function<filter, functions>;
            afore_var max = result_v<FindMax<ranked_funcs, 0ul>>;
            afore_type result = result_t<CullNonMax<functions, ranked_funcs, empty_type_list, max>>;
        };

        template <typename functions, typename args>
        struct FilterFuncsByAuto
        {
            template <typename F>
            afore_type filter = find_generic_args<F, args>;
            afore_type ranked_funcs = for_each_function<filter, functions>;
            afore_var min = result_v<FindMin<ranked_funcs, 0ul>>;
            afore_type result = result_t<CullNonMax<functions, ranked_funcs, empty_type_list, min>>;
        };

        template <typename placeholders, auto size>
        afore_var equals_placeholder(const Index i)
        {
            constexpr auto bool_arrays = to_array_with_default<placeholders, size, true>;
            return bool_arrays[static_cast<std::size_t>(i)];
        };
        template <typename placeholders, auto size>
        afore_var not_equals_placeholder(const Index i)
        {
            constexpr auto bool_arrays = to_array_with_default<placeholders, size, true>;
            return !bool_arrays[static_cast<std::size_t>(i)];
        };
    }

    template <typename functions, typename args>
    afore_type filter_funcs_by_converted = result_t<impl::FilterFuncsByConverted<functions, args>>;
    template <typename functions, typename args>
    afore_type filter_funcs_by_auto = result_t<impl::FilterFuncsByAuto<functions, args>>;

    template <typename _FuncList>
    struct OverloadedFunction
    {
        template <typename ...F>
        constexpr OverloadedFunction(F&& ...){}

        template <typename Args>
        afore_type valid_overloads = valid_functions<_FuncList, make_generic_args<Args>>;

        afore_var size = _FuncList::size;
        afore_var stored_functions = to_tuple_t<_FuncList>{};
    };
    template <Function ...F>
    OverloadedFunction(F&&...) -> OverloadedFunction<FunctionList<F...>>;

    template <typename ...BoundArgs>
    afore_var combine_args(std::tuple<BoundArgs...> a, std::tuple<>&& b)
    {
        constexpr auto size = sizeof...(BoundArgs);
        afore_type bound_args = ArgsList<BoundArgs...>;
        afore_type spliced_args = splice_args<ArgsList<BoundArgs...>, ArgsList<>>;
        afore_type placeholders = find_type<bound_args, placeholder_t>;
        afore_type bound_indices = reduce<&impl::equals_placeholder<placeholders, size>, make_sequence<size>>;

        return gather_tuple<bound_indices, to_tuple_t<spliced_args>>(a);
    }

    template <typename ...BoundArgs, typename HeadArg, typename ...Args>
    afore_var combine_args(std::tuple<BoundArgs...> a, std::tuple<HeadArg, Args...>&& b)
    {
        using bound_args = ArgsList<BoundArgs...>;
        using spliced_args = splice_args<ArgsList<BoundArgs...>, ArgsList<HeadArg, Args...>>;
        using next_tup = to_tuple_t<spliced_args>;

        static_assert(!is_same<next_tup, std::tuple<BoundArgs...>>);

        constexpr auto bound_size = sizeof...(BoundArgs);
        constexpr auto unbound_size = sizeof...(Args) + 1;

        afore_type placeholders = find_type<bound_args, placeholder_t>;
        constexpr auto size = std::max(bound_size, std::max(unbound_size, spliced_args::size));
        afore_type bound_indices = reduce<&impl::equals_placeholder<placeholders, size>, make_sequence<size>>;
        afore_type unbound_indices = reduce<&impl::not_equals_placeholder<placeholders, size>, make_sequence<size>>;
        afore_type bound_types = gather<TypeList<BoundArgs...>, bound_indices>;
        afore_type bound_tt = typename bound_types::tuple_type;


        afore_type indices = concatenate_lists<bound_indices, unbound_indices>;
        auto result = scatter_tuple<indices, next_tup>(std::tuple_cat(gather_tuple<bound_indices, bound_tt>(a), b));
        return result;
    }
}