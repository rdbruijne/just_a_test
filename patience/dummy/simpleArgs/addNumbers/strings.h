#pragma once

#include <numeric>
#include <sstream>
#include <string>
#include <vector>

template<typename iter>
std::string join(iter first, iter last, const std::string& separator)
{
	if(first == last)
		return {};

	using T = decltype(*first);

	if constexpr (std::is_convertible_v<T, std::string>)
	{
		std::string result = *first;
		while(++first != last)
		{
			result.append(separator);
			result.append(*first);
		}
		return result;
	}
	else
	{
		std::ostringstream os;
		os << *first;
		while(++first != last)
		{
			os << separator.c_str();
			os << *first;
		}
		return os.str();
	}
}

template<typename T>
std::string join(const std::vector<T>& list, const std::string& separator)
{
	return join(list.begin(), list.end(), separator);
}
