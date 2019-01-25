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
		LOG_ERROR(e.what());
	}
	return 0;
}
