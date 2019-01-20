// Jsdb.cpp : Defines the entry point for the application.
//

#include "Jsdb.hpp"
#include "JsdbComponent.hpp"

// Standard Library
using namespace std;
// Jsdb Main NS
using namespace jsdb;

int main()
{
	try
	{
		LOG_INFO("Starting JsDb DataServer! at ~" + std::to_string(CURRENT_TIMESTAMP()));
		/// Initialize configuration
		Config::getInstance().initConfig();
		/// Load all existing shards before anything else
		DataSource::getInstance().initDS();
		/// Load io service
		boost::asio::io_service ios;
		DataServer s(ios, 7355);
		ios.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
