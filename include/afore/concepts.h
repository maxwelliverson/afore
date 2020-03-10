//
// Created by maxwell on 2020-02-27.
//

#ifndef AFORE_CONCEPTS_H
#define AFORE_CONCEPTS_H
#include <tuple>
#include <type_traits>
//#include <afore.h>
//#include <concepts>
#include "core.h"
#include "unit_tests.h"

namespace afore
{
    inline namespace concepts
    {
        template <typename T>
        concept HasResultValue = requires
        {
            typename detail::Value<T::result>;
        };
        template <typename T>
        concept HasResultType = !HasResultValue<T> && requires
        {
            typename T::result;
        };

        template <typename T>
        concept ClassType = std::is_class_v<T>;
        template <typename T>
        concept Scalar = std::is_scalar_v<T>;
    }
    namespace detail
    {
        template <HasResultType T>
        struct ResultType<T> :
            AFORE_NEXT(result_t<T>);
        template <HasResultValue T>
        struct ResultType<T> :
            AFORE_NEXT(Value<result_v<T>>);

        namespace impl
        {
            template <typename T, template <auto...> typename TT>
            afore_var is_auto_template = false;
            template <template <auto...> typename TT, auto ...V>
            afore_var is_auto_template<TT<V...>, TT> = true;

            template <typename T, template <typename...> typename TT>
            afore_var is_type_template = false;
            template <template <typename...> typename TT, typename ...T>
            afore_var is_type_template<TT<T...>, TT> = true;

            template <template <typename...> typename>
            afore_var is_template()
            {
                return true;
            }
            template <template <auto...> typename>
            afore_var is_template()
            {
                return true;
            }
            template <typename>
            afore_var is_template()
            {
                return false;
            }
            template <auto>
            afore_var is_template()
            {
                return false;
            }

            template <typename T>
            afore_var is_template_instance = false;
            template <template <typename...> typename TT, typename ...Args>
            afore_var is_template_instance<TT<Args...>> = true;
            template <template <auto...> typename TT, auto ...Args>
            afore_var is_template_instance<TT<Args...>> = true;


            template <typename T, typename U>
            afore_var same_template = false;
            template <template <typename...> typename TT, typename ...ArgsA, typename ...ArgsB>
            afore_var same_template<TT<ArgsA...>, TT<ArgsB...>> = true;
            template <template <auto...> typename TT, auto ...ArgsA, auto ...ArgsB>
            afore_var same_template<TT<ArgsA...>, TT<ArgsB...>> = true;




            template <typename Lm>
            struct IsFunctionObj;
            template <Scalar Lm>
            struct IsFunctionObj<Lm>{};
            template <ClassType Lm>
            struct IsFunctionObj<Lm> : Lm{};
        }
    }
    inline namespace concepts
    {

        template <typename T>
        concept Enum = std::is_enum_v<T>;
        template <typename T>
        concept Char = std::is_same_v<T, char> ||
            std::is_same_v<T, char8_t> ||
            std::is_same_v<T, char16_t> ||
            std::is_same_v<T, char32_t>;

        template <typename T>
        concept Integer = std::is_integral_v<T>;
        template <typename T>
        concept FloatingPoint = std::is_floating_point_v<T>;
        template <typename T>
        concept Function = std::is_function_v<T> || (std::is_base_of_v<T, detail::impl::IsFunctionObj<T>>);
        template <typename T>
        concept TupleType =
                requires(T t)
        {
            std::get<0>(t);
            std::tuple_size_v<T> > 0;
        }
             || requires
        {
            std::tuple_size<T>::value == 0ul;
        };
        template <typename T>
        concept PairType = TupleType<T> && (std::tuple_size_v<T> == 2);

        template <typename T, typename U>
        concept SameAs = std::is_same_v<T, U>;

        template <typename T, typename U>
        concept SameTemplateAs = detail::impl::is_template_instance<T> && detail::impl::is_template_instance<U>
                && detail::impl::same_template<T, U>;

        template <typename T>
        concept ValueType = detail::impl::is_auto_template<T, detail::Value>;

        template <typename T>
        concept IndexListType = detail::impl::is_auto_template<T, IndexList>;

        template <typename T>
        concept ValueListType = IndexListType<T> || detail::impl::is_auto_template<T, ValueList>;
        template <typename T>
        concept TypeListType = detail::impl::is_type_template<T, TypeList>;

        template <typename T>
        concept AforeListType = ValueListType<T> || TypeListType<T>;

        template <typename T>
        concept EmptyList = AforeListType<T> && (T::size == Index{0});
        template <typename T>
        concept NonEmptyList = !EmptyList<T>;

        template <typename T, auto N>
        concept SizedList = AforeListType<T> && (T::size == Index{N});

        template <typename T>
        concept NonListType = !AforeListType<T>;

    }
    namespace detail
    {
        template <Function func, template <typename ...> typename Types, typename ...Args>
        inline constexpr static bool valid_call(const Types<Args...>)
        {
            return std::is_invocable_v<func, Args...>;
        }
        /*template <typename F, typename T>
        inline constexpr static bool valid_call(const T){return false;}*/

        template <Function func, TypeListType args>
        afore_var is_valid_call = valid_call<func>(args{});
    }
    inline namespace concepts
    {
        template <typename F, typename type_list>
        concept CalledFunctionType = detail::is_valid_call<F, type_list>;
    }
}

#include "unit_tests/concepts.inl"

#endif //AFORE_CONCEPTS_H
