//
// Created by maxwell on 2020-03-03.
//

#include <iostream>
#include "afore/data_structure/tuple.h"
#include "afore/modules.h"
#include "afore/string/nameof.h"
#include "afore/data_structure/tree.h"
#include "afore/algorithm/compare.h"

template <typename ...T>
std::ostream& operator<<(std::ostream& out, const afore::Tuple<T...>& tup)
{
    using namespace afore_module::index;
    using namespace afore_module::utils;

    out << "(" << afore::get<0>(tup);

    [&]<Index ...I>(const IndexList<I...>&&)
    {
        ((out << ", " << afore::get<I>(tup)) , ...);
    }(afore::make_sequence<sizeof...(T) - 1, 1ul>());

    out << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const afore::NilNode&&)
{
    return out;
}

template <typename ...Types>
std::ostream& operator<<(std::ostream& out, const afore::TypeList<Types...>&& tup)
{
    using namespace afore_module::utils;
    using namespace afore_module::index;
    using namespace afore::detail;
    auto print_func = [&]<Index I>(Value<I>&&){out << type_name<typename afore::TypeList<Types...>::template type<I>>() << ", "; return true;};
    auto printer = [&print_func]<Index ...I>(IndexList<I...>&&){(... && print_func(Value<I>{}));};
    constexpr auto size = sizeof...(Types) - 1;
    out << "(";
    printer(afore::make_sequence<size>());
    return out << type_name<typename afore::TypeList<Types...>::template type<size>>() << ")";
}

template <auto ...Types>
std::ostream& operator<<(std::ostream& out, const afore::TypeList<afore::detail::Value<Types>...>&& tup)
{
    using T = afore::TypeList<afore::detail::Value<Types>...>;
    using namespace afore_module::utils;
    using namespace afore_module::index;
    using namespace afore::detail;
    auto print_func = [&]<Index I>(Value<I>&&){out << result_v<decltype(T::template get<I>())> << ", "; return true;};
    auto printer = [&print_func]<Index ...I>(IndexList<I...>&&){(... && print_func(Value<I>{}));};
    constexpr auto size = sizeof...(Types) - 1;
    out << "(";
    printer(afore::make_sequence<size>());
    return out << result_v<decltype(T::template get<size>())> << ")";
}

template <auto ...Args>
std::ostream& operator<<(std::ostream& out, const afore::ValueList<Args...>&& tup)
{
  using T = afore::ValueList<Args...>;
  using namespace afore_module::utils;
  using namespace afore_module::index;
  using namespace afore::detail;
  auto print_func = [&]<Index I>(Value<I>&&){out << T::template get<I>() << ", "; return true;};
  auto printer = [&print_func]<Index ...I>(IndexList<I...>&&){(... && print_func(Value<I>{}));};
  constexpr auto size = sizeof...(Args) - 1;
  out << "(";
  printer(afore::make_sequence<size>());
  return out << T::template get<size>() << ")";
}


template <auto Val, typename Left, typename Right>
std::ostream& operator<<(std::ostream& out, const afore::Tree<afore::detail::Value<Val>, Left, Right>&&)
{
    return out << "(" << Left() << " " << Val << " " << Right() << ")";
}

template <typename Val, typename Left, typename Right>
std::ostream& operator<<(std::ostream& out, const afore::Tree<Val, Left, Right>&&)
{
  return out << "(" << Left() << " " << afore::nameof::type_name<Val>() << " " << Right() << ")";
}

template <typename Tree, typename Policy>
std::ostream& operator<<(std::ostream& out, const afore::TreeBase<Policy, Tree>)
{
    return out << Tree();
}

template <typename ...T>
void print_tupletypes(const afore::Tuple<T...>&)
{
    using namespace afore_module::utils;
    std::cout << afore::Tuple(type_name<T>()...) << std::endl;

}

template <typename A, typename B>
struct greater_than :
    AFORE_RESULT_V(afore::compare::greater_than<A, B>);

template <typename A, typename B>
struct less_than :
    AFORE_RESULT_V(afore::compare::less_than<A, B>);

int main()
{
    using afore::Tuple;
    using afore::tuple_cat;
    using namespace afore_module::string;
    using namespace afore_module::utils;

    constexpr auto tup1 = Tuple(0, "Hello", 23.4, string("Afore string test"));
    constexpr auto tup2 = Tuple('f', -0.0, nullptr);
    constexpr auto tup3 = tuple_cat(tup1, tup2);

    std::cout << tup1 << std::endl;
    std::cout << tup2 << std::endl;
    std::cout << tup3 << std::endl;

    std::cout << ("Hel"_A + "lo World!"_A) << std::endl;

    //print_tupletypes(tup1);
    //print_tupletypes(tup2);
    //print_tupletypes(tup3);

    std::cout << "\n\n";

    afore_type tree_policy = afore::tree_policies::SortedPolicy<less_than>;
    afore_type initial_list = afore::ValueList<3, 7, 10, -3, 5, 0, 1, 8>;

    afore_type tree = afore::make_tree_from_list<initial_list, tree_policy>;
    afore_type flattened_tree = afore::as_values<afore::flatten_tree<tree>>;

    afore_type int_types = afore::TypeList<long, short, int, long long int, char>;
    afore_type int_tree = afore::make_tree_from_list<int_types, tree_policy>;
    afore_type int_list = afore::flatten_tree<int_tree>;

    std::cout << int_types() << std::endl;
    std::cout << int_tree() << std::endl;
    std::cout << int_list() << std::endl;

    std::cout << "\n\n";

    std::cout << initial_list() << std::endl;
    std::cout << tree() << std::endl;
    std::cout << flattened_tree() << std::endl;
}