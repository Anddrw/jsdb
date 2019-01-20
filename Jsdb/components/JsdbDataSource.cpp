#include "JsdbComponent.hpp"

using namespace jsdb;

bool DataSource::initDS() 
{ 
	LOG_INFO("Initializing data source ....");

	boost::filesystem::path targetDir(
		Config::getInstance().GetString("keystore.shards.path", "shards")
	);

	LOG_INFO("Trying to regenerate shards from: " + targetDir.generic_string());

	boost::filesystem::recursive_directory_iterator iter(targetDir), eod;

	std::vector<std::string> shards;

	BOOST_FOREACH(boost::filesystem::path const& i, std::make_pair(iter, eod)) {
		if (is_regular_file(i)) {
			LOG_INFO("Found shard: " + i.string());
			shards.push_back(i.string());
		}
	}

	for (auto const& shard : shards)
	{
		LOG_INFO("Regenerating shard data from " + shard);
		try
		{
			std::string shardData;
			/// Load shard data
			boost::filesystem::load_string_file(shard, shardData);

			json _j = json::parse(shardData);

			/// Store keyslug separately, keep the rest
			std::string jsdbKeySlug = _j["jsdb:keyslug"];
			_j.erase("jsdb:keyslug");

			_ds.insert({ jsdbKeySlug, _j });
		}
		catch (json::type_error e)
		{
			LOG_ERROR("Could not load shard " + shard + "! Exception: " + std::string(e.what()));
		}
		catch (std::exception e)
		{
			LOG_ERROR("Could not load shard " + shard + "! Exception: " + std::string(e.what()));
		}
	}

	LOG_INFO("--> Regenerated " + std::to_string(shards.size()) + " shards!");

	return true;
}

size_t DataSource::GetSize() 
{ 
	return _ds.size();
}

uint32_t DataSource::GetLastTimestamp() 
{ 
	return 0; 
}

void DataSource::DSRandomCleanup() { }

std::map<std::string, json>& DataSource::GetDS()
{
	return _ds;
}