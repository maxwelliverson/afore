//
// Created by maxwell on 2020-02-26.
//

#ifndef AFORE_MACROS_H
#define AFORE_MACROS_H

#define conditional_type(cond, type_a, type_b) decltype([]{if constexpr(cond) return type_a{}; else return type_b{};}())
#define breakpoint(cond) ValueList<if_t<!(cond), int>{}>

#define AFORE_NEXT(...) AFORE_NEXT_CLASS(__VA_ARGS__) { using AFORE_NEXT_CLASS(__VA_ARGS__)::result;}
#define AFORE_RESULT_T(...) AFORE_RESULT_BASE {afore_type result = typename AFORE_NEXT_CLASS(__VA_ARGS__)::result;}
#define AFORE_RESULT_V(...) AFORE_RESULT_BASE {afore_var result = __VA_ARGS__;}
#define AFORE_RESULT_FALSE AFORE_RESULT_V(false)
#define AFORE_RESULT_TRUE AFORE_RESULT_V(true)
#define AFORE_NEXT_CLASS(...) afore::detail::ResultType<__VA_ARGS__>
#define AFORE_RESULT_BASE afore::detail::ResultBase
#define AFORE_ENABLE_IF(bool_val) typename enable_fence = afore::result_t<afore::detail::EnablerImpl<bool_val, void>>
#define AFORE_ENABLE_IF_MSG(bool_val, msg) typename enable_fence = result_t<afore::detail::EnablerImpl<\
        []()constexpr{static_assert(bool_val, msg); return bool_val;}(), void>>
#define afore_var inline constexpr static decltype(auto)
#define afore_local_var constexpr decltype(auto)
#define afore_type using
#define afore_mem_func inline constexpr decltype(auto)

#define AFORE_TO_STRING_INDIRECT(...) #__VA_ARGS__
#define AFORE_TO_STRING(...) AFORE_TO_STRING_INDIRECT(__VA_ARGS__)

#define PERF_FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)





#endif //AFORE_MACROS_H
