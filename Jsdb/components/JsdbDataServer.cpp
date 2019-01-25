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
// Boost
using boost::asio::ip::tcp;

DataServer::DataServer(boost::asio::io_service& ios, uint32_t _port)
	: ios(ios), acceptor(ios, tcp::endpoint(tcp::v4(), _port))
{
	std::shared_ptr<DataServerSession> session = std::make_shared<DataServerSession>(ios);
	acceptor.async_accept(session->GetSocket(),
		boost::bind(&DataServer::HandleAccept, this,
			session,
			boost::asio::placeholders::error
		)
	);
}

void DataServer::HandleAccept(
	std::shared_ptr<DataServerSession> session,
	const boost::system::error_code& err
) {
	if (!err) {
		session->Start();
		session = std::make_shared<DataServerSession>(ios);
		acceptor.async_accept(session->GetSocket(),
			boost::bind(&DataServer::HandleAccept, this, session,
				boost::asio::placeholders::error));
	}
	else {
		std::cerr << "err: " + err.message() << std::endl;
		session.reset();
	}
}

