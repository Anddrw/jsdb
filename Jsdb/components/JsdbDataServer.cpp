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

