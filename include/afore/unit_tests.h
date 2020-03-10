//
// Created by maxwell on 2020-02-28.
//

#ifndef FUNCT_UNIT_TESTS_H
#define FUNCT_UNIT_TESTS_H

#include "macros.h"
#include <type_traits>

namespace afore::detail
{
    template <typename A, typename B>
    afore_var is_same = std::is_same_v<std::remove_cvref_t<A>, std::remove_cvref_t<B>>;
    template <typename A, typename B>
    afore_var is_exact_same = std::is_same_v<A, B>;

    template <typename T>
    struct InspectType
    {
        static_assert(T::failure);
    };

    namespace assertions
    {
        template <bool B>
        struct Assert
        {
            static_assert(B, "Failed");
        };

        template <typename A, typename B>
        struct AssertSameType : Assert<is_same<A, B>>{};
        template <typename A, typename B>
        struct AssertNotSameType : Assert<!is_same<A, B>>{};

        struct FunctionObj
        {
            template<typename T>
            constexpr decltype(auto) operator()() const;
        };
        constexpr auto lambda = [](const auto&){return 0;};
        using lambda_type = decltype(lambda);
        template <typename A>
        int normal_func();
    }
}

#ifndef NDEBUG
#define assert_same_type(a, b) static_assert(afore::detail::is_same< a , b >, \
            AFORE_TO_STRING(Assertion a and b have same underlying types failed))
#define assert_exact_same_type(a, b) static_assert(afore::detail::is_exact_same< a , b >, \
            AFORE_TO_STRING(Assertion a and b have same types failed))
#define assert_not_same_type(a, b) static_assert(!afore::detail::is_same< a , b >, \
            AFORE_TO_STRING(Assertion a and b have different underlying types failed))
#define assert_not_exact_same_type(a, b) static_assert(!afore::detail::is_exact_same< a , b >, \
            AFORE_TO_STRING(Assertion a and b have different types failed))
#define assert_models_concept(a, con, ...) static_assert(con<a , ##__VA_ARGS__>, \
            AFORE_TO_STRING(Assertion a models con failed))
#define assert_doesnt_model_concept(a, con, ...) static_assert(!con<a, ##__VA_ARGS__>, \
            AFORE_TO_STRING(Assertion a does not model con failed))
#define assert_is_valid_call(f, ...) static_assert(afore::detail::valid_call<decltype(f)>(TypeList<__VA_ARGS__>()),\
            AFORE_TO_STRING(Assertion f (__VA_ARGS__) is a valid call failed))
#define assert_is_not_valid_call(f, ...) static_assert(!afore::detail::valid_call<decltype(f)>(TypeList<__VA_ARGS__>()),\
            AFORE_TO_STRING(Assertion f (__VA_ARGS__) is not a valid call failed))

#else //Assertions
    #define assert_same_type(a, b)
    #define assert_exact_same_type(a, b)
    #define assert_not_same_type(a, b)
    #define assert_not_exact_same_type(a, b)
    #define assert_models_concept(a, con, ...)
    #define assert_doesnt_model_concept(a, con, ...)
    #define assert_is_valid_call(f, ...)
    #define assert_is_not_valid_call(f, ...)
#endif //Assertions

#endif //FUNCT_UNIT_TESTS_H
