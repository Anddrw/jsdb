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

#include "Jsdb.hpp"
#include "JsdbComponent.hpp"

/*< Standard library namespace >*/
using namespace std;
/*< Jsdb main namespace >*/
using namespace jsdb;

/*< Main application entry point >*/
int32_t main(int32_t argc, char* argv[])
{
	try
	{
		/// Register argv input
		Globals::getInstance().initArgv(argc, argv);
		/// Initialize configuration
		Config::getInstance().initConfig();
		/// Initialize io service
		boost::asio::io_service ios;
		boost::shared_ptr< boost::asio::io_service::work > work(
			new boost::asio::io_service::work(ios)
		);
		/// Check if jsdb should be started in console mode
		if (Globals::getInstance().hasFlag("console"))
		{
			// @TODO: Fix Globals::getInstance().Get() --> Throws "bad cast"
			DataClient c(
				ios,
				"127.0.0.1",
				"7355"
			);

			Console console(c);
			console.initConsole();
			console.handleEvents();

			work.reset();
			ios.run();
		}
		else
		{
			/// Load all existing shards before anything else
			DataSource::getInstance().initDS();
			LOG_INFO("Jsdb server initialized");

			/// Initialize networking
			uint32_t _port = Config::getInstance()
				.GetUInt32("networking.port", 7355);
			std::string _hostname = Config::getInstance()
				.GetString("networking.hostname", std::string());

			DataServer s(ios, _port);
			
			work.reset();
			ios.run();
		}
	}
	catch (jsdb::exception& e) {
		LOG_ERROR(e.what());
	}
	catch (std::exception& e) {
		LOG_ERROR(e.what());
	}
	return 0;
}
