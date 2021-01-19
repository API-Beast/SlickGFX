#pragma once

#include <GLFW/glfw3.h>
#include <sGFX/Core.hpp>
#include "yestest.h"

struct TestGlContext
{
	TestGlContext() = delete;
	TestGlContext(int w, int h)
	{
		if(num_contexts == 0)
			glfwInit();
		num_contexts++;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		window = glfwCreateWindow(w, h, "sGFX Test", NULL, NULL);
		sGFX::SetupOpenGL(window);
	}
	~TestGlContext()
	{
		if(window)
			glfwDestroyWindow(window);

		num_contexts--;
		if(num_contexts == 0)
			glfwTerminate();
	}

	static int num_contexts;
	GLFWwindow* window = nullptr;
};

template<int N, typename T>
void print_value(const sGFX::VectorN<N, T>& f) { printf("{"); f.for_each(&YesTest::print_value); printf("}"); }

template<int N, typename T>
void ulp_difference(const sGFX::VectorN<N, T>& a, const sGFX::VectorN<N, T>& b)
{
	return a.sum(&YesTest::ulp_difference, b);
}