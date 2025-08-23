/****************
 *  tester.cpp  *
 ****************
 *
 * This unit is responsible for testing out the various functions against a fibbonaci look-up table.
 * The minimum C++ version required to compile this is C++23.
 * 
 */

#include <print>	// println()
#include <cstdint>	// uint types

#include "fiblib.h"


int main()
{
	uint8_t n{ 10 };
	std::println("fib::lookup_table[{}]         {}", n, fib::lookup_table[n]);
	std::println("fib::get_single_recursive({}) {}", n, fib::get_single_recursive(n));
}