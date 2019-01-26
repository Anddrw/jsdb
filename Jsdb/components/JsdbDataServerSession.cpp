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


DataServerSession::DataServerSession(boost::asio::io_service& ios)
	: _socket(ios) { }

tcp::socket& DataServerSession::GetSocket()
{
	return _socket;
}

void DataServerSession::Start()
{
	LOG_DEBUG("Received connection from --> " + _socket.remote_endpoint().address().to_string());
	_socket.async_read_some(
		boost::asio::buffer(_data, max_length),
		boost::bind(&DataServerSession::HandleRead, this,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void DataServerSession::HandleRead(
	std::shared_ptr<DataServerSession>& s,
	const boost::system::error_code& err, 
	size_t bytes_transferred
) {
	if (!err) {
		std::string recv(_data, bytes_transferred);
		auto response = ParseRequest(recv);
		LOG_DEBUG("Received payload: " + recv);
		_socket.async_read_some(
			boost::asio::buffer(_data, max_length),
			boost::bind(&DataServerSession::HandleRead, this,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

		boost::asio::write(GetSocket(), boost::asio::buffer(response, response.length()));
	}
	else {
		//std::cerr << "err (recv): " << err.message() << std::endl;
		LOG_ERROR("DataServerSession --> HandleRead() --> Received error: " + err.message());
	}
}

std::string DataServerSession::ParseRequest(std::string _request)
{
	// @TODO: Refactor this into a request handler
	
	try
	{
		json _jre;
		// Convert request json to json object
		_jre = json::parse(_request);

		// if the request contains no elements
		if (_jre.size() == 0) {
			return RESPONSE(std::map<std::string, std::string>({
				{"status", "error"},
				{"error", "Request payload is empty!"}
			}));
		}

		// if the request contains no action
		if (_jre.find("jsdb:action") == _jre.end()) {
			LOG_WARNING("Malfunctioned request was dropped! Error: Missing jsdb:action directive!");
			return RESPONSE(std::map<std::string, std::string>({
				{"status", "error"},
				{"error", "Missing jsdb:action directive in payload!"}
			}));
		}

		if (_jre["jsdb:action"] == "SetKey") {
			json __tmp = json::parse(_jre["jsdb:value"].dump());
			// run setkey
			auto res = DataAdapter::getInstance().SetKey(
				_jre["jsdb:keyslug"],
				__tmp
			);

			if (res) {
				return RESPONSE(std::map<std::string, std::string>({
					{"status", "success"},
				}));
			}
			else {
				return RESPONSE(std::map<std::string, std::string>({
					{"status", "failed"},
				}));
			}
		}


		if (_jre["jsdb:action"] == "GetKey") {
			auto res = DataAdapter::getInstance().GetKey(_jre["jsdb:keyslug"]);

			return RESPONSE(std::map<std::string, std::string>({
				{"status", "success"},
				{"data", res.dump()}
			}));
		}

		if (_jre["jsdb:action"] == "ShardOne") {
			DataAdapter::getInstance().ShardOne(_jre["jsdb:keyslug"]);

			return RESPONSE(std::map<std::string, std::string>({
				{"status", "success"},
			}));
		}

		if (_jre["jsdb:action"] == "DropShard") {
			DataAdapter::getInstance().DropShard(_jre["jsdb:keyslug"]);

			return RESPONSE(std::map<std::string, std::string>({
				{"status", "success"},
			}));
		}

		if (_jre["jsdb:action"] == "KeyCount") {
			return RESPONSE(std::map<std::string, std::string>({
				{"status", "success"},
				{"keycount", std::to_string(DataSource::getInstance().GetSize())}
			}));
		}

		return "STRRESPONSE";
	}
	catch (jsdb::exception& e)
	{
		LOG_ERROR(e.what());
	}
	catch (json::type_error& e)
	{
		LOG_ERROR("DataServerSession --> ParseRequest() --> Type error thrown during request --> " + std::string(e.what()));
	}
	catch (std::exception& e)
	{
		LOG_ERROR("DataServerSession --> ParseRequest() --> Exception thrown during request --> " + std::string(e.what()));
		
		// Kill existing socket in order to prevent sending any additional data...
		GetSocket().shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
		GetSocket().close();
	}

	return std::string();
}
