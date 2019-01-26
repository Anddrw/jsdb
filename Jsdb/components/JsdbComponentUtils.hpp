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

#pragma once

using namespace jsdb;

template<typename T>
constexpr auto LOG_DEBUG(T msg) { return Logger::getInstance().LogDebug(msg); }

template<typename T>
constexpr auto LOG_ERROR(T msg) { return Logger::getInstance().LogError(msg); }

template<typename T>
constexpr auto LOG_INFO(T msg) { return Logger::getInstance().LogInfo(msg); }

template<typename T>
constexpr auto LOG_WARNING(T msg) { return Logger::getInstance().LogWarning(msg); }

template<typename T>
constexpr auto LOG_NOTICE(T msg) { return Logger::getInstance().LogNotice(msg); }

#define JSDB_THROW(exception) throw exception
#define JSDB_EXIT_GRACEFULLY exit(0);

/// 1030 - Keystore reached it's maximum capacity
/// 1029 - Keystore is currently running in readonly mode!
template<typename T>
constexpr auto RESPONSE(T data) {
	json j;
	j = data;

	return j.dump();
}

inline uint64_t CURRENT_TIMESTAMP() {
	return 
	std::chrono::duration_cast
	<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

inline std::vector<std::string> explode(std::string const & s, char delim)
{
	std::vector<std::string> result;
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); )
	{
		result.push_back(std::move(token));
	}

	return result;
}
