/*
 * Copyright (C) 2019 Jsdb
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 */

#pragma once
#include <exception>

namespace jsdb
{
	class exception : public std::exception
	{
	public:
		/// returns the explanatory string
		const char* what() const noexcept override
		{
			return m.what();
		}

		/// the id of the exception
		const int id;

	protected:
		exception(int id_, const char* what_arg) : id(id_), m(what_arg) {}

		static std::string name(const std::string& ename, int id_)
		{
			return "[jsdb.exception." + ename + "." + std::to_string(id_) + "] ";
		}

	private:
		/// an exception object as storage for error messages
		std::runtime_error m;
	};

	class adapter_error : public exception
	{
	public:
		static adapter_error create(const std::string& what_arg, int id_)
		{
			std::string w = exception::name("adapter_error", id_) + what_arg;
			return adapter_error(id_, w.c_str());
		}

	private:
		adapter_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
	};

	class ds_error : public exception
	{
	public:
		static ds_error create(const std::string& what_arg, int id_)
		{
			std::string w = exception::name("ds_error", id_) + what_arg;
			return ds_error(id_, w.c_str());
		}

	private:
		ds_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
	};

	class server_error : public exception
	{
	public:
		static server_error create(const std::string& what_arg, int id_)
		{
			std::string w = exception::name("server_error", id_) + what_arg;
			return server_error(id_, w.c_str());
		}

	private:
		server_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
	};
}
