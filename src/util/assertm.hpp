#pragma once


#ifdef NDEBUG
#define assertm(a, b)
#else
#include <iostream>
#define assertm(a, b) if (!(a)) { std::cout << std::endl << "\033[1;31massert: \033[0m" << b << std::endl << __FILE__ << ':' << __LINE__ << ' ' << __FUNCTION__ << "   Assertion '" << #a << "' failed." << std::endl; abort(); }
#endif
