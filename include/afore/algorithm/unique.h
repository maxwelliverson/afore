//
// Created by maxwell on 2020-03-06.
//

#ifndef AFORE_UNIQUE_H
#define AFORE_UNIQUE_H

#include "../primitives.h"

namespace afore
{
    namespace detail::impl
    {
        template <AforeListType list, AforeListType result>
        struct GetUnique;
    }
}

#endif //AFORE_UNIQUE_H
