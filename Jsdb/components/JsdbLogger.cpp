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

Logger::Logger()
{
	initFormat();
	initFile();
}

void Logger::initFile()
{
	_mode = "file";

	std::string logFileNameFormat =
		boost::str(
			boost::format(
				"jsdb-log-%s.txt"
			)
			% date::format(
				"%Y%m%d",
				std::chrono::system_clock::now()
			)
		);

	if (_mode == "file") {
		_logFile.open(
			logFileNameFormat,
			std::ofstream::app
		);
	}
}

void Logger::initFormat()
{
	// Initialize all formats with default from configuration
	_format.s_dateOutputFormat = Config::getInstance().GetString("log.output.date.format", "%Y%m%d");
}

void Logger::setErrorFormat(std::string fmt)
{
	_format.s_errorOutputFormat = fmt;
}

void Logger::setNoticeFormat(std::string fmt)
{
	_format.s_noticeOutputFormat = fmt;
}

void Logger::setInfoFormat(std::string fmt)
{
	_format.s_infoOutputFormat = fmt;
}

void Logger::setDebugFormat(std::string fmt)
{
	_format.s_debugOutputFormat = fmt;
}

void Logger::setWarningFormat(std::string fmt)
{
	_format.s_warningOutputFormat = fmt;
}

void Logger::LogError(std::string message) {
	output(ELogLevel::L_ERROR, message);
}

void Logger::LogInfo(std::string message) {
	output(ELogLevel::L_INFO, message);
}

void Logger::LogWarning(std::string message) {
	output(ELogLevel::L_WARNING, message);
}

void Logger::LogDebug(std::string message) {
	output(ELogLevel::L_DEBUG, message);
}

void Logger::LogNotice(std::string message) {
	output(ELogLevel::L_NOTICE, message);
}

std::string Logger::formatOutputMessage(ELogLevel level, std::string outputMessage)
{
	std::string messageFormat;
	std::string literalLogLevel;

	switch (level)
	{
		case ELogLevel::L_INFO:
			literalLogLevel = "INFO";
			break;
		case ELogLevel::L_DEBUG:
			literalLogLevel = "DEBUG";
			break;
		case ELogLevel::L_ERROR:
			literalLogLevel = "ERROR";
			break;
		case ELogLevel::L_NOTICE:
			literalLogLevel = "NOTICE";
			break;
		case ELogLevel::L_WARNING:
			literalLogLevel = "WARNING";
			break;
		default:
			literalLogLevel = "UNKNOWN";
			break;
	}


	auto filters =
	{
		"{log.date}",
		"{log.message}",
		"{log.level}",
		"{log.file}"
	};

	if (messageFormat == "") {
		messageFormat = Config::getInstance().GetString("log.output.format", "[{log.date}] {log.level}: {log.message}");
	}

	std::string timeFormat =
		date::format(
			_format.s_dateOutputFormat,
			std::chrono::system_clock::now()
		);

	boost::replace_all(messageFormat, "{log.date}", timeFormat);
	boost::replace_all(messageFormat, "{log.message}", outputMessage);
	boost::replace_all(messageFormat, "{log.level}", literalLogLevel);

	return messageFormat;
}

void Logger::output(ELogLevel level, std::string message)
{
	// Format
	std::string outputFmt = formatOutputMessage(level, message);
	// Syserr
	std::cout << outputFmt << std::endl;
	// File
	_logFile << outputFmt << std::endl;
}
