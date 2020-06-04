//
// Created by maxwell on 2020-02-17.
//

#ifndef AFORE_NAMEOF_H
#define AFORE_NAMEOF_H

#include <array>
#include "afore/string/string.h"

#ifdef __GLIBCXX__
    #define AFORE_NAME_EXCESS 20
    #define AFORE_ENUM_NAME_EXCESS 107
#elif defined(__clang__)
    #define AFORE_NAME_EXCESS 28
    #define AFORE_ENUM_NAME_EXCESS 55
#elif
    #define AFORE_NAME_EXCESS 40
    #define AFORE_ENUM_NAME_EXCESS 80
#endif

namespace afore{
    namespace nameof {
        template<typename T>
        constexpr auto type_name() {
            constexpr int len = sizeof(__PRETTY_FUNCTION__) - AFORE_NAME_EXCESS - 1;
            return string<char, len>{__PRETTY_FUNCTION__ + AFORE_NAME_EXCESS};
        }

        template<Enum auto E>
        constexpr auto enum_name() {
            constexpr int excess = 107;
            constexpr int len = sizeof(__PRETTY_FUNCTION__) - excess - 1;
            return string<char, len>{__PRETTY_FUNCTION__ + excess};
        }

        template<typename T>
        constexpr auto nameof(T &&val) {
            return type_name<T>();
        }
        template <typename T>
        constexpr auto nameof(detail::TypeValue<T> val)
        {
            return type_name<T>();
        }
    }
}

#endif //AFORE_NAMEOF_H
