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

bool DataAdapter::SetKey(std::string _key, json _value)
{
	LOG_WARNING("DataAdapter --> Setting item --> " + _key);
	// Fetch and apply max keystore size
	auto keystoreMaxSize	= Config::getInstance().GetInt32("keystore.container.maxsize", 10000);
	auto keystoreDefaultTTL = Config::getInstance().GetUInt32("keystore.ttl", 50);
	auto keystoreStrategy	= Config::getInstance().GetString("keystore.data.strategy", "overwrite");

	if (DataSource::getInstance().GetSize() >= keystoreMaxSize) {
		LOG_WARNING("Keystore reached it's maximum size, future keys will not be added! ---> " + std::to_string(keystoreMaxSize));
		// Throw an exception to prevent adding any other item from the current session
		JSDB_THROW(adapter_error::create("Keystore reached it's maximum capacity!", 1030));
	}
	
	if (IsSharded(_key)) {
		LOG_WARNING("Key " + _key + " is sharded and cannot be changed!");
		return false;
	}
		

	json _j;

	_j["jsdb:keyts"] = CURRENT_TIMESTAMP();
	_j["jsdb:value"] = _value;

	// Check and add TTL if needed
	if (_j.count("jsdb:keyttl") == 0) {
		// We've got no TTL, set it to default CURRENT_TIMESTAMP + keystoreDefaultTTL
		_j["jsdb:keyttl"] = CURRENT_TIMESTAMP() + keystoreDefaultTTL;
	}

	// @TODO Refactor
	if (keystoreStrategy == "overwrite") {
		if (DataSource::getInstance().GetDS().count(_key) > 0) {
			LOG_DEBUG("Overwriting value of key " + _key);
			DataSource::getInstance().GetDS()[_key] = _j;
		}
		else {
			DataSource::getInstance().GetDS().insert({_key, _j});
		}
	}

	if (keystoreStrategy == "readonly") {
		if (DataSource::getInstance().GetDS().count(_key) > 0) {
			LOG_DEBUG("Keystore is currently running in readonly mode! Following key will not be modified: " + _key);
			// Throw an exception to prevent adding any other item from the current session
			JSDB_THROW(adapter_error::create("Keystore is currently running in readonly mode!", 1029));
		}
		else {
			DataSource::getInstance().GetDS().insert({ _key, _j });
		}
		return false;
	}
	
	return true;
}

json DataAdapter::GetKey(std::string _key)
{
	LOG_WARNING("DataAdapter --> Getting key --> " + _key);
	
	json _j = DataSource::getInstance().GetDS().at(_key);

	return _j;
}

bool DataAdapter::DropOne(std::string _key) 
{
	return DataSource::getInstance().GetDS().erase(_key) != 0 ? true : false;
}

bool DataAdapter::DropMany(std::vector<std::string> _keyList) 
{
	for (auto const& key : _keyList)
	{
		DataSource::getInstance().GetDS().erase(key);
	}

	return true;
}

bool DataAdapter::Refresh(std::string _key)
{
	// Is expired?
	// Is malfunctioned?
	// -----> DROP!!
	return true;
}

bool DataAdapter::RefreshAll()
{
	// Alias of Flush()
	return false;
}

bool DataAdapter::Flush()
{
	// Same functionality as refresh, just that it goes trough all keys
	return false;
}

bool DataAdapter::IsExpired(std::string _key) 
{
	auto _j = DataSource::getInstance().GetDS().at(_key);
	
	if (_j["jsdb:keyttl"] == -1) {
		return false;
	}

	if ((CURRENT_TIMESTAMP() - _j["jsdb:keyttl"].get<int32_t>()) < 0) {
		return false;
	}

	return true;
}

bool DataAdapter::ExpireOne(std::string _key) 
{
	// Set the keyttl to current time
	DataSource::getInstance()
		.GetDS()[_key]["jsdb:keyttl"] = CURRENT_TIMESTAMP();
	return true;
}

bool DataAdapter::ExpireMany(std::vector<std::string> _keyList)
{
	auto timestamp = CURRENT_TIMESTAMP();

	for (auto const& key : _keyList)
	{
		DataSource::getInstance().GetDS()[key]["jsdb:keyttl"] = timestamp;
	}

	return true;
}

///* Sharded keys are no longer touchable(cannot be modified, only re-created) and will be respawned every time jsdb starts
///* Sharded keys cannot have ttl and cannot expire
///* Sharded keys can only be removed on-demand
bool DataAdapter::ShardOne(std::string _key)
{
	// If it's already sharded then there's no point in re-creating it
	if (IsSharded(_key))
		return false;

	const std::string shardPath = Config::getInstance().GetString("keystore.shards.path", "shards");
	// Full path
	std::string shardDataPath = shardPath + "/" + std::to_string(boost::hash_value(_key)) + ".shard";
	LOG_INFO("Creating shard " + shardDataPath + " for key: " + _key);
	// Mark the key as sharded, so it can no longer be touched
	DataSource::getInstance().GetDS()[_key]["jsdb:shard"] = true;
	// Set TTL to -1
	DataSource::getInstance().GetDS()[_key]["jsdb:keyttl"] = -1;

	// Fetch key info as-is
	auto shard = DataSource::getInstance().GetDS().at(_key);

	// Add keyslug to shard
	shard["jsdb:keyslug"] = _key;

	// Write everything to the shard
	boost::filesystem::save_string_file(shardDataPath, shard.dump());

	return true;
}

bool DataAdapter::ShardMany(std::vector<std::string> _keyList)
{
	const std::string shardPath = Config::getInstance().GetString("keystore.shards.path", "shards");

	for (auto const& key : _keyList)
	{
		std::string shardDataPath = shardPath + "/" + std::to_string(boost::hash_value(key)) + ".shard";

		// Mark the key as sharded, so it can no longer be touched
		DataSource::getInstance().GetDS()[key]["jsdb:shard"] = true;
		// Set TTL to -1
		DataSource::getInstance().GetDS()[key]["jsdb:keyttl"] = -1;

		// Fetch key info as-is
		auto shard = DataSource::getInstance().GetDS().at(key);
		
		// Add keyslug to shard
		shard["jsdb:keyslug"] = key;

		// Write everything to the shard
		boost::filesystem::save_string_file(shardDataPath, shard.dump());
	}

	return true;
}

bool DataAdapter::IsSharded(std::string _key)
{
	const std::string shardPath = Config::getInstance().GetString("keystore.shards.path", "shards");
	return boost::filesystem::exists(shardPath + "/" + std::to_string(boost::hash_value(_key)) + ".shard");
}

bool DataAdapter::DropShard(std::string _key)
{
	const std::string shardPath = Config::getInstance().GetString("keystore.shards.path", "shards");
	std::string keyShardPath = shardPath + "/" + std::to_string(boost::hash_value(_key)) + ".shard";

	LOG_INFO("Dropping shard data " + keyShardPath + " for key " + _key + "!");
	boost::filesystem::remove(keyShardPath);

	// Also clear from container
	DataSource::getInstance().GetDS().erase(_key);

	return true;
}

bool DataAdapter::DropManyShards(std::vector<std::string> _keyList) 
{ 
	const std::string shardPath = Config::getInstance().GetString("keystore.shards.path", "shards");
	for (auto const& key : _keyList)
	{
		std::string keyShardPath = shardPath + "/" + std::to_string(boost::hash_value(key)) + ".shard";
		boost::filesystem::remove(keyShardPath);

		// Also clear from container
		DataSource::getInstance().GetDS().erase(key);
	}

	return true;
}

uint32_t DataAdapter::SeeTTL(std::string _key)
{
	return DataSource::getInstance().GetDS()[_key]["jsdb:keyttl"];
}

bool DataAdapter::SetTTL(std::string _key, uint32_t _ttl)
{
	DataSource::getInstance().GetDS()[_key]["jsdb:keyttl"] = _ttl;
	return true;
}
