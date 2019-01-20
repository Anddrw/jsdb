#include "JsdbComponent.hpp"

using namespace jsdb;

Config::Config() {}

void Config::initConfig()
{
	
	try
	{
		// if the configuration file does not exist
		// let's go ahead and create one
		if (!boost::filesystem::exists("jsdb.json")) {
			LOG_NOTICE("Configuration file jsdb.json seems to be missing... creating an empty one...");
			boost::filesystem::save_string_file("jsdb.json", "{}");
		}

		std::string jsdbConfig;
		// Load the contents of the config file
		boost::filesystem::load_string_file("jsdb.json", jsdbConfig);
		// Parse the config json
		_jsonConfig = json::parse(jsdbConfig);
	}
	catch (json::parse_error e)
	{
		LOG_ERROR("InitConfig() --> Failed to initialize config --> Exception: " + std::string(e.what()));
		LOG_INFO("InitConfig() --> Please check the syntax of the configuration file (jsdb.json)!");
	}
	catch (json::type_error e)
	{
		LOG_ERROR("InitConfig() --> Failed to initialize config --> Exception: " + std::string(e.what()));
		LOG_INFO("InitConfig() --> Please check the syntax of the configuration file (jsdb.json)!");
	}
	catch (std::exception e)
	{
		LOG_ERROR("InitConfig() --> Failed to initialize config --> Exception: " + std::string(e.what()));
	}

	if (_jsonConfig.size() == 0) {
		LOG_WARNING("InitConfig() --> Loaded 0 configuration items! --> Defaults will be used at runtime!");
	}
}

std::string Config::GetString(std::string _key, std::string _defValue) 
{
	// Check if we have this key
	if (_jsonConfig.count(_key) != 0) {
		return _jsonConfig[_key].get<std::string>();
	}
	// we did not find anything, fallback to _defValue
	return _defValue;
}

int32_t Config::GetInt32(std::string _key, int32_t _defValue)
{
	// Check if we have this key
	if (_jsonConfig.count(_key) != 0) {
		return _jsonConfig[_key].get<int32_t>();
	}
	// we did not find anything, fallback to _defValue
	return _defValue;
}

uint32_t Config::GetUInt32(std::string _key, uint32_t _defValue)
{
	// Check if we have this key
	if (_jsonConfig.count(_key) != 0) {
		return _jsonConfig[_key].get<uint32_t>();
	}
	// we did not find anything, fallback to _defValue
	return _defValue;
}
