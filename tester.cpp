/****************
 *  tester.cpp  *
 ****************
 *
 * This unit is responsible for testing out the various functions against a fibbonaci look-up table.
 * The minimum C++ version required to compile this is C++23.
 * 
 */


#include <print>		// println()
#include <cstdint>		// uint types
#include <random>		// rand()
#include <concepts> 	// integral<>
#include <functional>	// function<>
#include <chrono>		// steady_clock, time_point<>, duration<>
#include <array>		// array<>
#include <iostream>		// cout.flush()
#include <format>		// format_string<>
#include <forward_list>	// forward_list<>

#include "fiblib.h"


// templated rng for integers
template<class type> requires std::integral<type>
inline type get_random()
{
	return std::move(static_cast<type>(rand()));
}

// basic, independent function to request exit via console
void request_exit()
{
start_request_exit:
	char response{};
	std::println("Exit program? y/n");
	std::cin >> response;
	switch (response)
	{
		case 'Y':
		case 'y':
			std::println("Exiting program...");
			std::cout.flush();
			exit(0);
		break;
		case 'N':
		case 'n':
			std::println("Continuing with operations.");
			std::cout.flush();
		break;
		default: [[unlikely]]
			std::println("Invalid input.");
			std::cout.flush();
			goto start_request_exit;
	}
}

// std::println() doesn't flush on its own, but this function does
template<class... types>
void println_flush(const std::format_string<types...>& format, types&&... args)
{
	std::println(format, args...);
	std::cout.flush();
}

#ifdef _MSC_VER
# pragma warning( disable : 4172 )
#endif
// this thing concatenates an fwlist of strings in one string
inline std::string& concatenate(const std::forward_list<std::string>& strings)
{
	std::string concatenated_strings;
	for (auto iterator{ strings.begin() }; iterator != strings.end(); iterator++)
	{
		concatenated_strings += *iterator + ", ";
	}

	return concatenated_strings;
}
#ifdef _MSC_VER
# pragma warning( default : 4172 )
#endif

// this thing determines whether or not a fwlist contains an item via value comparison
inline bool list_contains_item(const std::forward_list<std::string>& list, const std::string& item)
{
	for (const std::string& string : list)
	{
		if (string.compare(item) == 0)
		{
			return true;
		}
	}
	return false;
}

/**
 * used to test whether a function works and to benchmark it
 * includes `name` for debugging purposes
 */
class function_tester final
{
	using reference = function_tester&;
	using const_reference = const function_tester&;
	using rvalue_reference = function_tester&&;
	using clock = std::chrono::steady_clock;
	using time_point = std::chrono::time_point<clock>;
	using duration = std::chrono::duration<double, std::milli>;
	
public:
	// rule of 5
	explicit function_tester() = delete;
	function_tester(const std::string& name, const std::function<uint64_t(const uint8_t)>& callback)
		: m_name{ name }
		, m_callback{ callback }
	{
	}
	function_tester(const_reference other) = delete;
	function_tester(rvalue_reference other) = delete;
	reference operator=(const_reference other) = delete;
	reference operator=(rvalue_reference other) = delete;
	~function_tester() = default;

	// assesses whether the function acquires the correct fibonacci number
	inline bool test(const uint8_t n) const
	{
		return fib::lookup_table[n] != m_callback(n);
	}
	
	// return how long the function took to run
	inline double benchmark(const uint8_t n) const
	{		
		const time_point start{ clock::now() };
		m_callback(n);
		const time_point finish{ clock::now() };
	
		return duration(finish - start).count();
	}
	
	const inline std::string& get_name() const
	{
		return m_name;
	}
	
private:
	const std::string m_name;
	const std::function<uint64_t(uint8_t)> m_callback;
};


template<class container_t>
const std::forward_list<std::string> perform_tests(const container_t& tests_container, const uint8_t n);
template<class container_t, class list_t>
void perform_benchmarks(const container_t& tests_container, const list_t& failed_funcs_names, const uint8_t n);

int main()
{
	// seed the randomizer and get the random value of the day
	srand(static_cast<unsigned>(time(nullptr)));
	const uint8_t n{ get_random<uint8_t>() % fib::lookup_table.size() };
	println_flush("The random number 'n' today is {}.\n", n);
	
	// all the functions to go here
	std::array<function_tester, 2> testies
	{
		function_tester{ "get_single_recursive", fib::get_single_recursive },
		function_tester{ "get_single_iterative", fib::get_single_iterative }
		/* add more later! =3 */
	};
	
	// first thing, test if all functions operate properly, and collect failures
	const std::forward_list<std::string> failed_funcs_names{ perform_tests(testies, n) };
	
	// now we do benchmarks on every function
	perform_benchmarks(testies, failed_funcs_names, n);
}

#ifdef _MSC_VER
# pragma warning( disable : 4172 )
#endif
template<class container_t>
const std::forward_list<std::string> perform_tests(const container_t& tests_container, const uint8_t n)
{
	std::forward_list<std::string> failed_funcs_names;
	
	// tests happen here
	for (const function_tester& tester : tests_container)
	{
		if (tester.test(n)) [[unlikely]] // heh...
		{
			println_flush("{}({}) failed.", tester.get_name(), n);
			request_exit();
			failed_funcs_names.push_front(tester.get_name());
		}
		println_flush("{}({}) succeeded.", tester.get_name(), n);
	}
	
	// general wrap-up
	println_flush("All functions have been tested.");
	if (!failed_funcs_names.empty()) [[unlikely]] // heh2...
	{
		std::println("The following functions failed:");
		println_flush("{}", concatenate(failed_funcs_names));
	}
	std::cout << std::endl;
	
	return failed_funcs_names;
}

template<class container_t, class list_t>
void perform_benchmarks(const container_t& tests_container, const list_t& failed_func_names, const uint8_t n)
{
	// benchmarks begin here
	for (const function_tester& tester : tests_container)
	{
		if (list_contains_item(failed_func_names, tester.get_name())) [[unlikely]] // heh3...
		{
			println_flush("{}({}) was not benchmarked, because it failed during testing.", tester.get_name(), n);
			continue;
		}
		
		const double ms_elapsed{ tester.benchmark(n) };
		
		println_flush("{}({}) executed at {:.5f}ms.", tester.get_name(), n, ms_elapsed);
		
		constexpr double ms_threshhold{ 30000.0 };
		static bool show_threshhold_notice{ true };
		if (show_threshhold_notice && ms_elapsed >= ms_threshhold) [[unlikely]]
		{
			std::println("{}({}) is taking longer than the threshhold ({}ms).", tester.get_name(), n, ms_threshhold);
			request_exit();
			show_threshhold_notice = false;
		}
	}
	
	// general wrap-up
	println_flush("All functions have been benchmarked.");
	/* todo: order functions by performance */
}
#ifdef _MSC_VER
# pragma warning( default : 4172 )
#endif