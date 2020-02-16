#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <map>
#include <mutex>

namespace fusion {

	enum class loglevel {
		OFF,
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	enum class logoutput {
		CONSOLE,
		FILE,
		ALL
	};


	class Logger {

	private:
		loglevel m_CurrentLevel;
		loglevel m_LogFilter;
		logoutput m_LogOutput;
		std::string m_LogFile;
		std::ofstream m_FStream;
		std::map<loglevel, std::string> m_LogColorMap;
		std::map<loglevel, std::string> m_LogLevelMap;
		std::map<std::thread::id, std::string> m_LogStringMap;
		std::mutex m_LogMutex;

	public:
		static Logger s_Instance;
		static std::string endl;

	private:
		Logger();
		~Logger();
		std::string timeStamp();
		std::string threadId();
		std::string stringifyLevel(const loglevel level);
		std::string wrapper(std::string str);

	public:
		static Logger& get();
		void log(const std::string& msg);
		void log(loglevel level, const std::string& msg);
		void setLogFilter(loglevel level);
		void setLogFile(const std::string& file);
		void setLogOutput(const logoutput output);
		Logger& operator()(loglevel level);
		void append(const std::string& msg);

		template<class T>
		Logger& operator<<(const T& value)
		{
			std::stringstream stream;
			stream << value;
			if (stream.str() == Logger::endl) {
				this->log(m_LogStringMap[std::this_thread::get_id()]);
			}
			else {
				this->append(stream.str());
			}
			return *this;
		}

	public:
		Logger(const Logger&) = delete;
		Logger operator = (const Logger&) = delete;

	};

}