#include <iostream>
#include <tuple>
#include <range/v3/view.hpp>
//
#include "afore.h"

namespace range = ranges::cpp20;
namespace view = ranges::cpp20::views;
using view::transform, view::filter, view::iota, ranges::views::repeat, view::take;


int main()
{
    namespace af = afore;
    using namespace af::nameof;
    using namespace afore_module::functional;
    using ranges::views::generate_n;
    using ranges::views::generate;






    int a = 0;
    int b = 1;
    const auto fibb = sum(a, b, _);

    int N;
    std::cin >> N;


    for(auto n : generate(fibb) | take(N))
    {
        std::cout << a << std::endl;
        a = b;
        b = n;
    }

    return 0;
}

