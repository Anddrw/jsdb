#include "JsdbComponent.hpp"

using namespace jsdb;
using namespace boost::program_options;

void Globals::initArgv(int32_t argc, char* argv[])
{
	try
	{
		/// <summary>
		///		Value storage for --port flag
		/// </summary>
		std::string 
			_port = Config::getInstance()
			.GetString("networking.port", "7355");
		/// <summary>
		///		Value storage for --hostname flag
		/// </summary>
		std::string 
			_hostname = Config::getInstance()
			.GetString("networking.hostname", "127.0.0.1");
		
		options_description desc("Allowed options");

		desc.add_options()
			// First parameter describes option name/short name
			// The second is parameter to option
			// The third is description
			("help,h", "Print usage message")
			("console", "Opens the console")
			("client", "Opens the console")
			("shard-all", "Shards all existing keys")
			("refresh-all", "Performs refresh action on all existing keys")
			("expire-all", "Expires all existing keys")
			("uptime", "Gets the current uptime")
			("enable-networking", "Enables networking mode")
			("skip-networking", "Disables networking mode")
			("local-networking", "Binds the networking to localhost (all external packets will be dropped)")
			("create-savepoint", "Creates a savepoint based on the existing keys")
			("drop-shards", "Drops all existing shards and the associated keys")
			("hostname", value<std::string>(&_hostname), "Connects to a Jsdb server on the provided hostname (default port used if no --port provided)")
			("port", value<std::string>(&_port), "Connects to a Jsdb server on the provided port (localhost used if no --hostname provided)")
			("shutdown", "Shutdown");

		store(parse_command_line(argc, argv, desc), _vm);
		
		if (_vm.count("help")) {
			std::cout << desc << "\n";
			JSDB_EXIT_GRACEFULLY
		}

		// All possible commands are incompatible with shutdown
		conflictingOptions(_vm, "console",     "shutdown");
		conflictingOptions(_vm, "shard-all",   "shutdown");
		conflictingOptions(_vm, "refresh-all", "shutdown");
		conflictingOptions(_vm, "expire-all",  "shutdown");
	}
	catch (boost::program_options::error& e)
	{
		LOG_ERROR(e.what());
		JSDB_EXIT_GRACEFULLY
	}
	catch (jsdb::exception& e)
	{
		LOG_ERROR(e.what());
		JSDB_EXIT_GRACEFULLY
	}
	catch (std::exception e)
	{
		LOG_ERROR(e.what());
		JSDB_EXIT_GRACEFULLY
	}
}

void Globals::conflictingOptions(const variables_map& vm, const char* opt1, const char* opt2)
{
	if (
		vm.count(opt1) &&
		!vm[opt1].defaulted() &&
		vm.count(opt2) &&
		!vm[opt2].defaulted()
	) {
		JSDB_THROW(logic_error::create(std::string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.", 1013));
	}
}

void Globals::optionDependency(const variables_map& vm, const char* for_what, const char* required_option)
{
	if (vm.count(for_what) && !vm[for_what].defaulted())
	{
		if (vm.count(required_option) == 0 || vm[required_option].defaulted())
		{
			JSDB_THROW(logic_error::create(std::string("Option '") + for_what + "' requires option '" + required_option + "'.", 1014));
		}
	}
}

bool Globals::hasFlag(std::string _flag)
{
	return (_vm.count(_flag)) ? true : false;
}

std::string Globals::Get(std::string _flag) 
{
	return _vm[_flag].as<std::string>();
}

std::string Globals::dump()
{
	std::string result;

	for (auto flag : _vm) {
		result += flag.first + "\n";
	}

	return result;
}
