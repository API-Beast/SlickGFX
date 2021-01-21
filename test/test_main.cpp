#include "yestest.h"
#include "test_utils.h"

int main(int argc, char** argv)
{
	setbuf(stdout, NULL);
	return YesTest::run(argc, argv);
}