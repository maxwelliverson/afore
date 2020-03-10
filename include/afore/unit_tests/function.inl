namespace afore::detail::assertions
{
    //Declarations
    afore_var test_func_1 = [](const auto&, int, float, auto){};
    afore_var test_func_2 = [](){};
    afore_var test_func_3 = [](auto...){};
    afore_var test_func_4 = [](const std::string&, auto, std::array<int, 3>&&){};
    afore_type funclist_1234 = FunctionList<decltype(test_func_1), decltype(test_func_2), decltype(test_func_3), decltype(test_func_4)>;
    afore_type funclist_13 = FunctionList<decltype(test_func_1), decltype(test_func_3)>;
    afore_type funclist_23 = FunctionList<decltype(test_func_2), decltype(test_func_3)>;

    afore_type overload_1234 = OverloadedFunction<funclist_1234>;
    template <typename ...Args>
    afore_type valid_funcs_1234 = typename overload_1234::template valid_overloads<ArgsList<Args...>>;


    //Test 1
    assert_is_valid_call(test_func_1, const int&, long, double, const char*);
    assert_is_valid_call(test_func_1, int, int, int, int);

    assert_is_not_valid_call(test_func_1, int, int, int);
    assert_is_not_valid_call(test_func_1, int, int, int, int, int);
    assert_is_not_valid_call(test_func_1, char, float, int);
    assert_is_not_valid_call(test_func_1, int);
    assert_is_not_valid_call(test_func_1, );

    //Test 2
    assert_is_valid_call(test_func_2, );

    assert_is_not_valid_call(test_func_2, int);
    assert_is_not_valid_call(test_func_2, float);
    assert_is_not_valid_call(test_func_2, int, int, int);

    //Test 3
    assert_is_valid_call(test_func_3, );
    assert_is_valid_call(test_func_3, int);
    assert_is_valid_call(test_func_3, double, char&&, __cfloat128);

    //Test 4
    assert_is_valid_call(test_func_4, const char*, int, decltype(std::array{1, 3, 4}));
    assert_is_valid_call(test_func_4, std::string, float, std::array<int, 3>);

    assert_is_not_valid_call(test_func_4, std::string&&, float, std::array<int, 4>);


    //Function Lists

    AssertSameType<valid_funcs_1234<>, funclist_23> assert1;
    AssertSameType<valid_funcs_1234<int, int, int, int>, funclist_13> assert2;

    AssertNotSameType<valid_funcs_1234<>, funclist_13> assert3;
    AssertNotSameType<valid_funcs_1234<int, int, int, int>, funclist_23> assert4;


    //function


    //delegate



}