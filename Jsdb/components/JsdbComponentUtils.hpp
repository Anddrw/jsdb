#pragma once

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
