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

using boost::asio::ip::tcp;

DataClient::DataClient(boost::asio::io_service& ios, std::string _hostname, std::string _port)
	: _serverHostname(_hostname), _serverPort(_port), _socket(ios), _resolver(ios)
{
}

void DataClient::Connect()
{
	boost::asio::connect(GetSocket(), _resolver.resolve(tcp::resolver::query(_serverHostname, _serverPort)));
}

void DataClient::Send(json _jsdbPayload)
{
	std::string request = _jsdbPayload.dump();
	boost::asio::write(GetSocket(), boost::asio::buffer(request, request.length()));
}

json DataClient::GetResponse()
{
	std::string response(_data, max_length);
	size_t reply_length = boost::asio::read(
		GetSocket(),
		boost::asio::buffer(response, max_length)
	);

	json _j = json::parse(response);

	return _j;
}

tcp::socket& DataClient::GetSocket()
{
	return _socket;
}
