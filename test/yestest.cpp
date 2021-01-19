
#include "yestest.h"
#include <cstring>

using namespace YesTest;

bool YesTest::register_test(Test t)
{
	std::vector<Test>& v = get_test_list();
	v.push_back(t);
	return true;
};

std::vector<Test>& YesTest::get_test_list()
{
	static std::vector<Test> vector;
	return vector;
}

bool YesTest::Test::run()
{
	int errors = 0;
	try
	{
		if(func)
			func(&errors);
	}
	catch(std::exception& e)
	{
		errors += 1;
		std::printf("  Exception: %s", e.what());
	}
	catch(...)
	{
		errors += 1;
		std::printf("  Exception: Unknown exception.");
	}
	return errors == 0;
}

int YesTest::run(int argc, char** argv)
{
	std::vector<Test> tests = get_test_list();
	if(argc > 1)
	{
		if(std::strcmp(argv[1], "--list") == 0 || std::strcmp(argv[1], "-l") == 0 )
		{
			for(Test current_test : tests)
				std::puts(current_test.name);
			return 0;
		}
		else if(argc == 2)
		{
			std::vector<Test> test_list;
			for(Test test : tests)
				if((std::strcmp(argv[1], test.name) == 0) || (std::strcmp(argv[1], test.category) == 0))
					test_list.push_back(test);
			if(test_list.size() == 0)
			{
				std::printf("%s[  FAILED  ]%s No test named %s was found.", CLR_RED, CLR_RESET, argv[1]);
				return 1;
			}
			return YesTest::run_tests(test_list);
		}
		else if(argc == 3)
		{
			std::vector<Test> test_list;
			for(Test test : tests)
				if((std::strcmp(argv[1], test.category) == 0) && (std::strcmp(argv[2], test.name) == 0))
					test_list.push_back(test);
			if(test_list.size() == 0)
			{
				std::printf("%s[  FAILED  ]%s No test named %s::%s was found.", CLR_RED, CLR_RESET, argv[1], argv[2]);
				return 1;
			}
			return YesTest::run_tests(test_list);
		}
		std::printf("Invalid arguments: %d arguments given", argc);
		return 1;
	}
	return YesTest::run_tests(tests);
}

int YesTest::run_tests(const std::vector<Test>& tests)
{
	std::vector<Test> failed_tests;

	if(tests.size() == 0)
	{
		std::printf("%s[  FAILED  ]%s No tests were executed.", CLR_RED, CLR_RESET);
		return 1;
	}

	std::printf("%s[==========]%s Running %lu test cases.\n", CLR_GREEN, CLR_RESET, tests.size());
	for(Test current_test : tests)
	{
		std::printf("%s[ RUN      ]%s %s\n", CLR_GREEN, CLR_RESET, current_test.name);
		bool result = current_test.run();
		if(result == true)
			std::printf("%s[       OK ]%s %s\n", CLR_GREEN, CLR_RESET, current_test.name);
		else
		{
			std::printf("%s[  FAILED  ]%s %s\n", CLR_RED, CLR_RESET, current_test.name);
			failed_tests.push_back(current_test);
		}
	}
	std::printf("%s[==========]%s %lu test cases ran.\n", CLR_GREEN, CLR_RESET, tests.size());
	if(failed_tests.size() > 0)
	{
		std::printf("%s[  FAILED  ]%s %lu tests, listed below:\n", CLR_RED, CLR_RESET, failed_tests.size());
		for(Test failed_test : failed_tests)
			std::printf("%s[  FAILED  ]%s %s\n", CLR_RED, CLR_RESET, failed_test.name);
	}
	return failed_tests.size();
}

void YesTest::failure(int* errors)
{
	(*errors) += 1;
	// Set a breakpoint to this function to inspect
}
