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

bool DataSource::initDS() 
{ 
	LOG_INFO("Initializing data source ....");

	boost::filesystem::path targetDir(
		Config::getInstance().GetString("keystore.shards.path", "shards")
	);

	LOG_INFO("Trying to regenerate shards from: " + targetDir.generic_string());

	// Check if the shards storage path exists, if not create it
	if (!boost::filesystem::is_directory(targetDir.relative_path())) {
		boost::filesystem::create_directory(targetDir.relative_path());
	}

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