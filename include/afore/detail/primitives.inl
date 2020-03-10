namespace afore::detail{

    template<typename T>
    struct TypeValue {
        using result = T;
        constexpr TypeValue() = default;
        constexpr TypeValue(T &&) {};
        constexpr bool operator==(const TypeValue &) const noexcept {
            return true;
        }
        template<typename Other>
        constexpr bool operator==(const TypeValue<Other> &) const noexcept {
            return std::is_same_v<std::remove_cvref_t<std::decay_t<T>>, std::remove_cvref_t<std::decay_t<Other>>>;
        }
        template<typename Other>
        constexpr bool operator!=(const TypeValue<Other> &other) const noexcept {
            return !(*this == other);
        }
        template<typename Other>
        constexpr bool operator==(const Other &) const noexcept {
            return std::is_same_v<std::remove_cvref_t<std::decay_t<T>>, std::remove_cvref_t<std::decay_t<Other>>>;
        }
        template<typename Other>
        constexpr bool operator!=(const Other &other) const noexcept {
            return !(*this == other);
        }
    };

    //A simple implementation enable_if
    template<bool, typename>
    struct EnablerImpl;
    template<typename T>
    struct EnablerImpl<true, T>
        : AFORE_RESULT_T(T);

    ///////////////////////////////////////

    template<typename Type>
    struct IsValueListImpl :
        AFORE_RESULT_FALSE;
    template<auto ...Args>
    struct IsValueListImpl<ValueList < Args...>> :
        AFORE_RESULT_TRUE;


    template<typename Type>
    struct IsTypeListImpl :
        AFORE_RESULT_FALSE;
    template<typename ...Types>
    struct IsTypeListImpl<TypeList < Types...>> :
        AFORE_RESULT_TRUE;


    template<typename Type>
    struct IsCtList :
        AFORE_RESULT_FALSE;
    template<auto ...Values>
    struct IsCtList<ValueList < Values...>> :
        AFORE_RESULT_TRUE;


    template<bool condition, typename if_true>
    struct IfImpl;
    template<typename A>
    struct IfImpl<true, A> {
        template<typename B>
        struct else_t {
            using result = A;
        };
        using result = A;
    };
    template<typename A>
    struct IfImpl<false, A> {
        template<typename B>
        struct else_t {
            using result = B;
        };
    };


    template<Index N, Index Pos, Index Initial, auto Step, typename Holder>
    struct GenerateIndexListImpl;
    template<Index N, Index Initial, auto Step, Index ...args>
    struct GenerateIndexListImpl<N, N, Initial, Step, IndexList<args...>> :
        AFORE_RESULT_T(IndexList<args...>);
    template<Index N, Index Pos, Index Initial, auto Step, Index ...list>
    struct GenerateIndexListImpl<N, Pos, Initial, Step, IndexList<list...>> :
        AFORE_NEXT(GenerateIndexListImpl<N, Pos + 1, Initial, Step, IndexList<list..., (Pos * Step) + Initial>>);


    template<typename ...Seq>
    struct ConcatenateListsImpl;

    template<auto ...head, auto ...tail, typename ...rest>
    struct ConcatenateListsImpl<ValueList < head...>, ValueList<tail...>, rest...> :
        AFORE_NEXT(ConcatenateListsImpl<ValueList < head..., tail...>, rest...>);
    template<auto ...list>
    struct ConcatenateListsImpl<ValueList < list...>> :
        AFORE_RESULT_T(ValueList < list...>);

    template<typename ...head, typename ...tail, typename ...rest>
    struct ConcatenateListsImpl<TypeList < head...>, TypeList<tail...>, rest...> :
        AFORE_NEXT(ConcatenateListsImpl<TypeList < head..., tail...>, rest...>);
    template<typename ...list>
    struct ConcatenateListsImpl<TypeList < list...>> :
        AFORE_RESULT_T(TypeList < list...>);


    template<typename List>
    struct IsEmptyImpl;
    template<>
    struct IsEmptyImpl<empty_list> : AFORE_RESULT_TRUE;
    template<auto head, auto ...tail>
    struct IsEmptyImpl<ValueList < head, tail...>> : AFORE_RESULT_FALSE;
    template<>
    struct IsEmptyImpl<empty_type_list> : AFORE_RESULT_TRUE;
    template<typename head, typename ...tail>
    struct IsEmptyImpl<TypeList < head, tail...>> : AFORE_RESULT_FALSE;



    template<auto A>
    struct TypeOfImpl;
    template<typename T, TypeValue<T> arg>
    struct TypeOfImpl<arg> :
        AFORE_RESULT_T(T);
    template<auto Arg>
    struct TypeOfImpl :
        AFORE_RESULT_T(std::remove_cvref_t<std::decay_t<decltype(Arg)>>);

    template<typename T>
    struct TypesOfListImpl;
    template<auto ...list>
    struct TypesOfListImpl<ValueList < list...>> :
        AFORE_RESULT_T(ValueList < TypeValue<TypeOfImpl<list>>{}...>);
    template<typename ...list, TypeValue<list> ...args>
    struct TypesOfListImpl<ValueList <args...>> :
        AFORE_RESULT_T(TypeList<list...>);


    template <typename T>
    struct HeadImpl;
    template <typename head, typename ...tail>
    struct HeadImpl<TypeList<head, tail...>> :
        AFORE_RESULT_T(TypeList<head>);
    template <auto head, auto ...tail>
    struct HeadImpl<ValueList<head, tail...>> :
        AFORE_RESULT_T(ValueList<head>);

    template <typename T>
    struct TailImpl;
    template <typename head>
    struct TailImpl<TypeList<head>> :
        AFORE_RESULT_T(TypeList<>);
    template <auto head>
    struct TailImpl<ValueList<head>> :
        AFORE_RESULT_T(ValueList<>);
    template <typename head, typename ...tail>
    struct TailImpl<TypeList<head, tail...>> :
        AFORE_RESULT_T(TypeList<tail...>);
    template <auto head, auto ...tail>
    struct TailImpl<ValueList<head, tail...>> :
        AFORE_RESULT_T(ValueList<tail...>);


    template <typename F, typename T>
    struct ApplyImpl;
    template <auto F, auto ...Args>
    struct ApplyImpl<Value<F>, ValueList<Args...>> :
        AFORE_RESULT_V(F(Args...));
    template <auto F, typename ...Args>
    struct ApplyImpl<Value<F>, TypeList<Args...>> :
        AFORE_RESULT_T(decltype(F(std::declval<Args>()...)));
    template <typename F, typename ...Args>
    struct ApplyImpl<F, TypeList<Args...>> :
        AFORE_RESULT_T(decltype(std::declval<F>()(std::declval<Args>()...)));
    template <typename F, auto ...Args>
    struct ApplyImpl<F, ValueList<Args...>> :
        AFORE_RESULT_T(decltype(std::declval<F>()(Args...)));



}