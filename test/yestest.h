#pragma once
#include <cstddef>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <string>
#include <algorithm>

#define YES_TEST(CATEGORY,NAME)\
static void yes_test_run_##CATEGORY##_##NAME(int* yes_test_errors);\
static bool yes_test_##CATEGORY##_##NAME##_registered = YesTest::register_test({#CATEGORY, #CATEGORY "::" #NAME, &yes_test_run_##CATEGORY##_##NAME});\
static void yes_test_run_##CATEGORY##_##NAME(int* yes_test_errors)

#define CLR_RESET "\033[0m"
#define CLR_RED "\033[31m"
#define CLR_GREEN "\033[32m"
#define CLR_CYAN "\033[96m"
#define CLR_YELLOW "\033[93m"

namespace YesTest
{
	using TestFunctionPointer = void (*)(int*);
	struct Test
	{
		const char* category;
		const char* name;
		TestFunctionPointer func;
		bool run();
	};

	int run(int argc, char** argv);
	int run_tests(const std::vector<Test>& tests);
	std::vector<Test>& get_test_list();
	bool register_test(Test t);

	inline void print_value(bool b)         { if(b) printf(" true"); else printf("false");};
	inline void print_value(int v)          { printf("%5d", v); };
	inline void print_value(unsigned int v) { printf("%5u", v); };
	inline void print_value(float v)        { printf("%5.3f", double(v)); };
	inline void print_value(double v)       { printf("%5.3f", v); };
	inline void print_value(long double v)  { printf("%5.3Lf", v); };
	inline void print_value(long int ptr)            { printf("%5ld", ptr); };
	inline void print_value(long long int v)         { printf("%5lld", v); };
	inline void print_value(long unsigned int v)     { printf("%5lu", v);  };
	inline void print_value(long long unsigned int v){ printf("%5llu", v); };
	template<typename T>
	inline void print_value(T* ptr){ printf("%5p", ptr); };
	inline void print_value(std::nullptr_t){ printf("nullptr"); };
	inline void print_value(std::string str){ printf("\"%s\"", str.c_str()); };
	inline void print_value(const char* str){ printf("\"%s\"", str); };
	template<typename T>
	auto print_value(const T& container) -> std::void_t<decltype(container.begin(), container.end())>
	{
		printf("{");
		int i = 0;
		for(auto v : container)
		{
			if(i != 0)
				printf(", ");
			print_value(v);
			i++;
			if(i > 6){ printf(" ..."); break;}
		}
		printf("}");
	}

	//
	[[gnu::noinline]] void failure(int* errors);

	template<typename A, typename B>
	inline void print_value(A arg1, B arg2)
	{
		print_value(static_cast<typename std::decay<A>::type>(arg1));
		printf(", ");
		print_value(static_cast<typename std::decay<B>::type>(arg2));
	}

	template<typename A, typename B, typename... Args>
	inline void print_value(A arg1, B arg2, Args... args)
	{
		print_value(static_cast<typename std::decay<A>::type>(arg1));
		printf(", ");
		print_value(static_cast<typename std::decay<B>::type>(arg2));
		printf(", ");
		print_value(args...);
	}

	template<typename T>
	inline void print_container_range(T it, int range_start, int range_end, int size, const std::vector<bool>& is_success)
	{
		using YesTest::print_value;
		printf("%5d elements [", size);
		if(range_start > 0) printf("... ");
		it += range_start;
		for(int i = range_start; i < range_end; i++)
		{
			if(i < size)
			{
				if(is_success[i])
					print_value(*it);
				else
				{
					printf(CLR_YELLOW);
					print_value(*it);
					printf(CLR_RESET);
				}
				it++;
			}
			else
				printf("%s-----%s", CLR_YELLOW, CLR_RESET);
			if(i != range_end-1)
				printf(" ");
		}
		if(range_end < size) printf(" ...");
		printf("]");
	}

	// ULP = unit of least precision, how many discrete steps floats differ
	inline int ulp_difference(float a, float b)
	{
		int ai = *(int*)&a;
		if(ai < 0) ai = 0x80000000 - ai;
		int bi = *(int*)&b;
		if(bi < 0) bi = 0x80000000 - bi;
		float diff = std::abs(a - b);
		// We calculate both bitwise ULP difference as well as EPSILON difference.
		// This is because the bitwise difference can be enormous for numbers very close to 0.
		return std::min(std::abs(ai - bi), int(diff / FLT_EPSILON) / 5);
	}
};

#define YES_PRINT_FAILURE(MSG) printf("%s%s:%u%s: %serror:%s " MSG "\n", CLR_CYAN, __FILE__, __LINE__, CLR_RESET, CLR_RED, CLR_RESET)

#define EXPECT_SIMILAR(x, y, ulps)\
{\
	using YesTest::print_value;\
	using YesTest::ulp_difference;\
	auto val_a = (x); auto val_b = (y);\
	int difference = ulp_difference(val_a, val_b);\
	if(difference > ulps)\
	{\
		YesTest::failure(yes_test_errors);\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: " #x " ~= " #y "\n");\
		printf(" -> |     Actual: %s", CLR_YELLOW);\
		print_value(val_a);\
		printf(" !~= ");\
		print_value(val_b);\
		printf("%s\n", CLR_RESET);\
		printf(" -> |  Precision: %d ULPs (Threshold: " #ulps " ULPs)\n", difference);\
	}\
}


// TODO: Move most into extra function so I can properly debug this.
#define EXPECT_CONTAINER_OP(x, y, OPER, FAILURE)\
{\
	using YesTest::print_value;\
	auto& val_a = (x); auto& val_b = (y);\
	auto size_a = std::distance(std::begin(val_a), std::end(val_a)); auto size_b = std::distance(std::begin(val_b), std::end(val_b));\
	size_t smaller_size = std::min(size_a, size_b);\
	size_t larger_size = std::max(size_a, size_b);\
	std::vector<bool> is_success(larger_size, false);\
	auto it_a = std::begin(val_a);\
	auto it_b = std::begin(val_b);\
	int unequal_elements = larger_size - smaller_size;\
	for(int i = 0; i<smaller_size; i++)\
	{\
		is_success[i] = (*it_a OPER *it_b);\
		if(!is_success[i]) unequal_elements++;\
		it_a++;\
		it_b++;\
	}\
	if(unequal_elements > 0)\
	{\
		int range_start = 0;\
		int range_end = larger_size;\
		auto it = std::find(is_success.begin(), is_success.end(), false);\
		if(it != is_success.end())\
		{\
			range_start = std::distance(it, is_success.begin()) - 2;\
			range_end = range_start + 6;\
		}\
		if(range_start < 0)\
		{\
			range_end += -range_start;\
			range_start = 0;\
		}\
		if(range_end > larger_size)\
		{\
			int diff = range_end - larger_size;\
			range_start -= diff;\
			range_end -= diff;\
		}\
		if(range_start < 0) range_start = 0;\
		if(range_end > larger_size) range_end = larger_size;\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: " #x" "#OPER" "#y"\n");\
		printf(" -> |     Actual: " #x" "#FAILURE" "#y"\n");\
		printf(" -> | " #x " is:\n -> | ");\
		YesTest::print_container_range(std::begin(val_a), range_start, range_end, size_a, is_success);\
		printf("\n");\
		printf(" -> | " #y " is:\n -> | ");\
		YesTest::print_container_range(std::begin(val_b), range_start, range_end, size_b, is_success);\
		int additional_failures = unequal_elements - std::count(is_success.begin()+range_start, is_success.begin()+range_end, false);\
		if(additional_failures > 0)\
			printf("\n -> | ... and %d more mismatches.", additional_failures);\
		printf("\n");\
		YesTest::failure(yes_test_errors);\
	}\
}


#define EXPECT_UNSIMILAR(x, y, ulps)\
{\
	using YesTest::print_value;\
	using YesTest::ulp_difference;\
	auto val_a = (x); auto val_b = (y);\
	int difference = ulp_difference(val_a, val_b);\
	if(difference < ulps)\
	{\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: "#x" !~= " #y"\n");\
		printf(" -> |     Actual: %s", CLR_YELLOW);\
		print_value(val_a);\
		printf(" ~= ");\
		print_value(val_b);\
		printf("%s\n", CLR_RESET);\
		printf(" -> |  Precision: %d ULPs (Threshold: " #ulps " ULPs)\n", difference);\
		YesTest::failure(yes_test_errors);\
	}\
}

#define EXPECT_OP(x, y, OPER, FAILURE)\
{\
	using YesTest::print_value;\
	auto val_a = (x); auto val_b = (y);\
	if(!(val_a OPER val_b))\
	{\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: "#x" "#OPER" "#y"\n");\
		if(val_a FAILURE val_b)\
		{\
			printf(" -> |     Actual: %s", CLR_YELLOW);\
			print_value(val_a);\
			printf(" " #FAILURE " ");\
			print_value(val_b);\
			printf("%s\n", CLR_RESET);\
		}\
		else\
		{\
			printf(" -> |     Actual: %s", CLR_YELLOW);\
			print_value(val_a);\
			printf(" ??? ");\
			print_value(val_b);\
			printf("%s\n", CLR_RESET);\
			printf(" -> | Comparing the values failed for both "#OPER" and "#FAILURE" (broken operator overloads?)\n");\
		}\
		YesTest::failure(yes_test_errors);\
	}\
}

#define EXPECT_TRUE(x)\
{\
	using YesTest::print_value;\
	auto val_a = (x);\
	if(!(val_a))\
	{\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: "#x"\n");\
		printf(" -> |     Actual: %s", CLR_YELLOW);\
		print_value(val_a);\
		printf("%s\n", CLR_RESET);\
		YesTest::failure(yes_test_errors);\
	}\
}

#define EXPECT_FALSE(x)\
{\
	using YesTest::print_value;\
	auto val_a = (x);\
	if((val_a))\
	{\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: "#x" is false\n");\
		printf(" -> |     Actual: %s", CLR_YELLOW);\
		print_value(val_a);\
		printf("%s is true\n", CLR_RESET);\
		YesTest::failure(yes_test_errors);\
	}\
}

#define PRINT_INFO(x)\
{\
	using YesTest::print_value;\
	if(*yes_test_errors)\
	{\
		auto val = (x);\
		printf(" -> |       Info: %8s = %s", #x, CLR_CYAN);\
		print_value(val);\
		printf("%s\n", CLR_RESET);\
	}\
}

#define EXPECT_EQ(x, y) EXPECT_OP(x, y, ==, !=)
#define EXPECT_NEQ(x, y) EXPECT_OP(x, y, !=, ==)
#define EXPECT_GREATER(x, y) EXPECT_OP(x, y, >, <=)
#define EXPECT_LESS(x, y) EXPECT_OP(x, y, <, >=)
#define EXPECT_GREATER_OR_EQ(x, y) EXPECT_OP(x, y, >=, <)
#define EXPECT_LESS_OR_EQ(x, y) EXPECT_OP(x, y, <=, >)

#define EXPECT_CONTAINER_EQ(x, y)            EXPECT_CONTAINER_OP(x, y, ==, !=)
#define EXPECT_CONTAINER_NEQ(x, y)           EXPECT_CONTAINER_OP(x, y, !=, ==)
#define EXPECT_CONTAINER_GREATER(x, y)       EXPECT_CONTAINER_OP(x, y, >, <=)
#define EXPECT_CONTAINER_LESS(x, y)          EXPECT_CONTAINER_OP(x, y, <, >=)
#define EXPECT_CONTAINER_GREATER_OR_EQ(x, y) EXPECT_CONTAINER_OP(x, y, >=, <)
#define EXPECT_CONTAINER_LESS_OR_EQ(x, y)    EXPECT_CONTAINER_OP(x, y, <=, >)

#define ASSERT_OP(x, y, OPER, FAILURE)\
{\
	using YesTest::print_value;\
	auto val_a = (x); auto val_b = (y);\
	if(!(val_a OPER val_b))\
	{\
		YES_PRINT_FAILURE("assertion not fulfilled");\
		printf(" -> |   Expected: "#x" "#OPER" "#y"\n");\
		printf(" -> |     Actual: %s", CLR_YELLOW);\
		print_value(val_a);\
		printf(" " #FAILURE " ");\
		print_value(val_b);\
		printf("%s\n", CLR_RESET);\
		YesTest::failure(yes_test_errors);\
		return;\
	}\
}

#define ASSERT_EQ(x, y) ASSERT_OP(x, y, ==, !=)
#define ASSERT_NEQ(x, y) ASSERT_OP(x, y, !=, ==)
#define ASSERT_GREATER(x, y) ASSERT_OP(x, y, >, <=)
#define ASSERT_LESS(x, y) ASSERT_OP(x, y, <, >=)
#define ASSERT_GREATER_OR_EQ(x, y) ASSERT_OP(x, y, >=, <)
#define ASSERT_LESS_OR_EQ(x, y) ASSERT_OP(x, y, <=, >)

#define EXPECT_BETWEEN(x, low, high) {\
	using YesTest::print_value;\
	auto val_a = (x); auto val_low = (low); auto val_high = (high);\
\
	if(val_a < val_low || val_a > val_high)\
	{\
		YES_PRINT_FAILURE("expectation not fulfilled");\
		printf(" -> |   Expected: "#low" <= "#x" <= " #high"\n");\
		if(val_a < val_low)\
		{\
			printf(" -> |     Actual: %s", CLR_YELLOW);\
			print_value(val_a);\
			printf(" < ");\
			print_value(val_low);\
			printf("%s\n", CLR_RESET);\
		}\
		if(val_a > val_high)\
		{\
			printf(" -> |     Actual: %s", CLR_YELLOW);\
			print_value(val_a);\
			printf(" > ");\
			print_value(val_high);\
			printf("%s\n", CLR_RESET);\
		}\
		YesTest::failure(yes_test_errors);\
	}\
}


// Each floating point operation has a error of up to 0.5 NUPs, so we use a default tolerance of 10 NUPs which should be enough for reasonable complex operations
#define EXPECT_EQf(x, y) EXPECT_SIMILAR(x, y, 10)
#define EXPECT_NEQf(x, y) EXPECT_UNSIMILAR(x, y, 10)