/****************************
 *  tester.cpp              *
 ****************************
 *
 * This unit is responsible for testing out the various functions
 * against a fibbonaci look-up table.
 * 
 * 
 */

#include <print>

#define FIBLIB_USE_LOOKUP
#include "fiblib.h"


int main()
{
    std::println("{}", fib::fib(2));
}