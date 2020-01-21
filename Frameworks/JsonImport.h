#pragma once
#include "picojson.h"

#include <string>
#include <vector>
#include <list>

namespace JsonImport
{
	using object_type = ::picojson::object;
	using value_type = ::picojson::value;

	void split(std::vector<std::string>& path, const std::string& dot_sep_path)
	{
		const char* dp = dot_sep_path.c_str();
		char buf[100] = {};
		for (int i = 0; i < 100; i++) { buf[i] = '\0'; }

		int i = 0;
		while (1)
		{
			// <path‚Ì‹æØ‚è‚ðŒ©‚Â‚¯‚½‚ç”²‚¯‚é>
			if ((dp[i] == '.') || (dp[i] == '\0'))
			{
				break;
			}
			buf[i] = dp[i];
			i++;
		}
		std::string push(buf);
		path.push_back(push);
		if (dp[i] == '\0')	// <I’[‚Ü‚Å—ˆ‚½‚ç•ªŠ„I—¹>
		{
			return;
		}
		split(path, &dot_sep_path[i + 1]);
	}

	static inline auto get_value(const value_type& value, const std::string& dot_separated_path)
		-> value_type&
	{
		std::vector<std::string> path;
		split(path, dot_separated_path);

		auto out = const_cast<const value_type*>(&value);
		for (const auto& path_part : path)
			out = &out->get< object_type >().at(path_part);
		
		return const_cast<value_type&>(*out);
	}

	template <typename Type>
	Type get_as(const value_type& value);

	/**/
	template <>
	inline double get_as(const value_type& value)
	{
		return{ value.get<double>() };
	}

	template <>
	inline float get_as(const value_type& value)
	{
		return float(get_as<double>(value));
	}

	template <>
	inline int get_as(const value_type& value)
	{
		return int(get_as<double>(value));
	}

	template <>
	inline std::string get_as(const value_type& value)
	{
		return value.get<std::string>();
	}

	template <>
	inline picojson::array get_as(const value_type& value)
	{
		return value.get<picojson::array>();
	}

	template <>
	inline picojson::object get_as(const value_type& value)
	{
		return value.get<picojson::object>();
	}
	/**/

	template <typename T>
	inline std::vector<T> get_vector(const picojson::array data_list)
	{
		std::vector<T> dataList;
		for (int i = 0; i < data_list.size(); i++)
		{
			T data = get_as<T>(data_list[i]);
			dataList.push_back(data);
		}
		return dataList;
	}

	template <typename T>
	inline std::list<T> get_list(const picojson::array data_list)
	{
		std::list<T> dataList;
		for (int i = 0; i < data_list.size(); i++)
		{
			T data = get_as<T>(data_list[i]);
			dataList.push_back(data);
		}
		return dataList;
	}

}