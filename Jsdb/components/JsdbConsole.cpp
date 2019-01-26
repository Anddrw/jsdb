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

#include "JsdbComponent.hpp"

using namespace jsdb;

Console::Console(DataClient& _c)
	: _client(_c)
{}

Console::~Console()
{
	_client.GetSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	_client.GetSocket().close();
}

void Console::initConsole()
{	
	try
	{
		_client.Connect();
		Output("Connected to " + std::string("127.0.0.1:7355"));
	}
	catch (jsdb::exception& e)
	{
		LOG_ERROR(e.what());
		JSDB_EXIT_GRACEFULLY
	}
	catch (std::exception& e)
	{
		LOG_ERROR(e.what());
		JSDB_EXIT_GRACEFULLY;
	}
}

void Console::handleEvents()
{
	A:
	StatusLine();

	std::string action;
	std::getline(std::cin, action);

	Output(action);

	goto A;
}

void Console::StatusLine()
{
	std::cout << "jsdb> ";
}

void Console::Output(std::string _out)
{
	std::cout << _out << std::endl;
}

