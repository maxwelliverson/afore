//
// Created by maxwell on 2020-02-23.
//

#ifndef AFORE_SCATTER_GATHER_H
#define AFORE_SCATTER_GATHER_H

#include "afore/primitives.h"
#include "sort.h"
#include "afore/algorithm/foreach.h"
#include "tie.h"

namespace afore
{
    namespace detail
    {
        struct placeholder{};

        template <typename List, typename Indices>
        struct GatherImpl;
        template <typename List, typename Indices>
        struct ScatterImpl;

        template <auto ...elements, auto ...indices>
        struct GatherImpl<ValueList<elements...>, ValueList<indices...>> :
            AFORE_RESULT_T(ValueList<ValueList<elements...>::template get<indices>()...>);
        template <typename ...elements, auto ...indices>
        struct GatherImpl<TypeList<elements...>, ValueList<indices...>> :
            AFORE_RESULT_T(TypeList<typename TypeList<elements...>::template type<indices>...>);


        template <auto ...elements, auto ...indices>
        struct ScatterImpl<ValueList<elements...>, ValueList<indices...>>
        {
            using result = for_each_in_list<
                               [](auto&& a){return a.template get<1>();},
                               sort_list<
                               [](auto&& a, auto&& b){return a.template get<0>() < b.template get<0>();},
                               tie<ValueList<elements...>, ValueList<indices...>>>
                            >;
        };

    }

    template <typename List, typename Indices>
    using gather =  result_t<detail::GatherImpl<List, Indices>>;
    template <typename List, typename Indices>
    using scatter = result_t<detail::ScatterImpl<List, Indices>>;

    template <typename List>
    using scatter_indices = result_t<detail::ScatterImpl<List, make_sequence<List::size>>>;

    template <IndexListType Indices, typename T, template <typename...> typename Tp, typename ...Args>
    afore_var scatter_tuple(Tp<Args...>&& tup)
    {
        //detail::InspectType<Indices>();
        //detail::InspectType<scatter_indices<Indices>>();
        return [&tup]<Index ...I>(IndexList<I...>&&)
        {
            //detail::InspectType<decltype(std::get<0>(tup))>();
            return T{std::get<static_cast<std::size_t>(I)>(tup)...};
        }(scatter_indices<Indices>{});
    }

    template <IndexListType Indices, typename T, template <typename...> typename Tp, typename ...Args>
    afore_var gather_tuple(Tp<Args...>& tup)
    {
        return [&tup]<Index ...I>(IndexList<I...>&&){return T{std::get<static_cast<std::size_t>(I)>(tup)...};}(Indices{});
    }
}

#endif //AFORE_SCATTER_GATHER_H
