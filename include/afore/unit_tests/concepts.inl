#include "../macros.h"
namespace afore::detail::assertions
{
        assert_not_exact_same_type(int, const int&);
        assert_same_type(int, const int&);
        assert_exact_same_type(int, std::remove_cvref_t<volatile int &&>);
        assert_not_same_type(int, unsigned int);

        //Enum
        //assert_models_concept(Index, Enum);
        assert_doesnt_model_concept(int, Enum);

        //Integer
        assert_models_concept(int, Integer);
        assert_models_concept(char, Integer);
        assert_doesnt_model_concept(double, Integer);

        //Char
        assert_models_concept(decltype('a'), Char);
        assert_models_concept(char8_t, Char);
        assert_doesnt_model_concept(decltype("a"), Char);
        assert_doesnt_model_concept(const char8_t*, Char);

        //FloatingPoint
        assert_models_concept(float, FloatingPoint);
        assert_models_concept(double, FloatingPoint);
        assert_doesnt_model_concept(int, FloatingPoint);

        //Function
        assert_models_concept(lambda_type, Function);
        assert_models_concept(FunctionObj, Function);
        assert_models_concept(decltype(normal_func<int>), Function);
        assert_doesnt_model_concept(decltype(normal_func<int>()), Function);
        assert_doesnt_model_concept(int, Function);

        //TupleType
        //assert_models_concept(ValueList<(1,2,3)>, TupleType);
        assert_models_concept(std::tuple<double>, TupleType);
        //assert_doesnt_model_concept(double, TupleType);

        //PairType
        //assert_models_concept(std::tuple<int, int>, PairType);

        //CalledFunctionType
        assert_models_concept(lambda_type, CalledFunctionType, TypeList<const int&>);
        assert_doesnt_model_concept(lambda_type, CalledFunctionType, TypeList<>);
        assert_doesnt_model_concept(lambda_type, CalledFunctionType, void);


        using ValListHelper = ValueList<1, 5, 'f'>;
        //SameTemplateAs
        assert_models_concept(TypeList<int>, SameTemplateAs, TypeList<ValueList<1>>);
        assert_models_concept(ValListHelper, SameTemplateAs, ValueList<>);
        assert_doesnt_model_concept(TypeList<int>, SameTemplateAs, ValListHelper);
        assert_doesnt_model_concept(TypeList<ValueList<1>>, SameTemplateAs, ValueList<1>);


}