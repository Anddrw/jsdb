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
#include "Jsdb.hpp"
#include "JsdbException.hpp"

namespace jsdb
{
	/** Data handling */
	class DataAdapter
	{
		public:
			static DataAdapter& getInstance() {
				static DataAdapter instance;
				return instance;
			}
		private:
			DataAdapter() {}
			DataAdapter(DataAdapter const&);
			virtual ~DataAdapter() {}
			void operator=(DataAdapter const&);
		public:
			bool SetKey(std::string _key, json _value);
			json GetKey(std::string _key);
			bool DropOne(std::string _key);
			bool DropMany(std::vector<std::string> _keyList);
			bool Refresh(std::string _key);
			bool RefreshAll();
			bool Flush();
			bool IsExpired(std::string _key);
			bool ExpireOne(std::string _key);
			bool ExpireMany(std::vector<std::string> _keyList);
			bool ShardOne(std::string _key);
			bool ShardMany(std::vector<std::string> _keyList);
			bool IsSharded(std::string _key);
			bool DropShard(std::string _key);
			bool DropManyShards(std::vector<std::string> _keyList);
			uint32_t SeeTTL(std::string _key);
			bool SetTTL(std::string _key, uint32_t _ttl);
		private:
			std::string __lastKey;
	};

	class DataSource
	{
		public:
			static DataSource& getInstance() {
				static DataSource instance;
				return instance;
			}
		private:
			DataSource() {}
			DataSource(DataSource const&);
			virtual ~DataSource() {}
			void operator=(DataSource const&);
		public:
			bool initDS();
			size_t GetSize();
			uint32_t GetLastTimestamp();
			void DSRandomCleanup();
			std::map<std::string, json>& GetDS();
		public:
			std::map<std::string, json> _ds;
	};
	/** Schema handling */
	class SchemaBuilder
	{
		public:
			static SchemaBuilder& getInstance() {
				static SchemaBuilder instance;
				return instance;
			}
		private:
			SchemaBuilder() {}
			SchemaBuilder(SchemaBuilder const&);
			virtual ~SchemaBuilder() {}
			void operator=(SchemaBuilder const&);
	};
	/** Networking */
	/** Session handler for DataServer */
	class DataServerSession : public std::enable_shared_from_this<DataServerSession>
	{
	public:
		DataServerSession(boost::asio::io_service& ios);
		DataServerSession(DataServerSession const&);
		virtual ~DataServerSession() {}
		void operator=(DataServerSession const&);
	public:
		void Start();
		void HandleRead(
			std::shared_ptr<DataServerSession>& s,
			const boost::system::error_code& err,
			size_t bytes_transferred
		);
		boost::asio::ip::tcp::socket& GetSocket();
	public:
		std::string ParseRequest(std::string _request);
	private:
		boost::asio::ip::tcp::socket _socket;
		enum { max_length = 4096 };
		char _data[max_length];
	};
	/** Data server */
	class DataServer
	{
		public:
			DataServer(boost::asio::io_service& ios, uint32_t _port);
			DataServer(DataServer const&);
			virtual ~DataServer() {}
			void operator=(DataServer const&);
		private:
			void HandleAccept(
				std::shared_ptr<DataServerSession> session,
				const boost::system::error_code& err
			);
		private:
			boost::asio::io_service& ios;
			boost::asio::ip::tcp::acceptor acceptor;
	};
	/** FileSystem */
	class FileSystem
	{
		public:
			static FileSystem& getInstance() {
				static FileSystem instance;
				return instance;
			}
		private:
			FileSystem() {}
			FileSystem(FileSystem const&);
			virtual ~FileSystem() {}
			void operator=(FileSystem const&);
	};
	/** Configuration */
	class Config
	{
		public:
			static Config& getInstance() {
				static Config instance;
				return instance;
			}
		private:
			Config();
			Config(Config const&);
			virtual ~Config() {}
			void operator=(Config const&);
		public:
			void initConfig();
		public:
			std::string GetString(std::string _key, std::string _defValue = std::string());
			int32_t GetInt32(std::string _key, int32_t _defValue = int32_t());
			uint32_t GetUInt32(std::string _key, uint32_t _defValue = uint32_t());
		private:
			json _jsonConfig;
	};
	/** JSDB Logging component*/
	class Logger
	{
		public:
			static Logger& getInstance() {
				static Logger instance;
				return instance;
			}
		private:
			Logger();
			Logger(Logger const&);
			virtual ~Logger() {}
			void operator=(Logger const&);
		private:
			enum ELogLevel
			{
				L_INFO,
				L_WARNING,
				L_ERROR,
				L_DEBUG,
				L_NOTICE,

				E_LEVEL_COUNT
			};
		public:
			void initFile();
			void initFormat();
		public:
			void setErrorFormat(std::string fmt);
			void setNoticeFormat(std::string fmt);
			void setInfoFormat(std::string fmt);
			void setDebugFormat(std::string fmt);
			void setWarningFormat(std::string fmt);
			void LogError(std::string message);
			void LogInfo(std::string message);
			void LogWarning(std::string message);
			void LogDebug(std::string message);
			void LogNotice(std::string message);
		private:
			std::string formatOutputMessage(ELogLevel level, std::string outputMessage);
			void output(ELogLevel level, std::string message);
		private:
			std::ofstream _logFile;
			std::string _mode;
		private:
			struct SLogManagerOutputFormat
			{
				std::string s_errorOutputFormat;
				std::string s_noticeOutputFormat;
				std::string s_infoOutputFormat;
				std::string s_debugOutputFormat;
				std::string s_warningOutputFormat;
				// Date format
				std::string s_dateOutputFormat;
			};
		private:
			SLogManagerOutputFormat _format;
	};
}

#include "JsdbComponentUtils.hpp"