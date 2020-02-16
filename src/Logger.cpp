#include <fusion.h>

namespace fusion {

	Logger::Logger() {

		m_CurrentLevel = loglevel::TRACE;
		m_LogOutput = logoutput::CONSOLE;
		m_LogFile = std::string();
		m_LogLevelMap = {
			{loglevel::TRACE,"TRACE"},
			{loglevel::DEBUG,"DEBUG"},
			{loglevel::INFO,"INFO "},
			{loglevel::WARN,"WARN "},
			{loglevel::ERROR,"ERROR"},
			{loglevel::FATAL,"FATAL"},
		};
		m_LogColorMap = {
			{loglevel::TRACE,"\u001b[94m "},
			{loglevel::DEBUG,"\u001b[96m "},
			{loglevel::INFO,"\u001b[92m "},
			{loglevel::WARN,"\u001b[93m "},
			{loglevel::ERROR,"\u001b[91m "},
			{loglevel::FATAL,"\u001b[95m "},
		};
		m_LogStringMap = {
			{std::this_thread::get_id(),""}
		};

	}

	Logger& Logger::get() {

		return s_Instance;
	}

	void Logger::setLogFilter(loglevel level) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		m_LogFilter = level;
	}

	void Logger::setLogOutput(const logoutput output) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		m_LogOutput = output;
	}

	void Logger::setLogFile(const std::string& file) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		m_LogFile = file;
		m_FStream.open(file);
	}

	std::string Logger::timeStamp() {

		auto now = std::chrono::system_clock::now();
		std::time_t rawTime = std::chrono::system_clock::to_time_t(now);
		std::tm calenderTime;
		localtime_s(&calenderTime, &rawTime);
		std::stringstream stream;
		stream << calenderTime.tm_hour << ":" << calenderTime.tm_min << ":" << calenderTime.tm_sec;
		return stream.str();
	}

	std::string Logger::wrapper(std::string str) {

		std::stringstream stream;
		stream << "[" << str << "] ";
		return stream.str();
	}

	std::string Logger::threadId() {
		std::stringstream stream;
		stream << "Thread Id: " << std::this_thread::get_id();
		return stream.str();
	}

	std::string Logger::stringifyLevel(const loglevel level) {
		std::stringstream stream;
		try {
			stream << m_LogColorMap.at(level) << m_LogLevelMap.at(level) << " \u001b[39m";
		}
		catch (const std::out_of_range & e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;
		}
		return stream.str();
	}

	Logger& Logger::operator()(loglevel level) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		m_CurrentLevel = level;
		return *this;
	}

	void Logger::append(const std::string& msg) {
		std::lock_guard<std::mutex> lock(m_LogMutex);
		m_LogStringMap[std::this_thread::get_id()] += msg;
	}

	void Logger::log(const std::string& msg) {
		log(m_CurrentLevel, msg);
	}

	void Logger::log(loglevel level, const std::string& msg) {

		if (level >= m_LogFilter) {

			std::lock_guard<std::mutex> lock(m_LogMutex);
			std::stringstream stream;
			stream << wrapper(stringifyLevel(level)) << wrapper(threadId()) << wrapper(timeStamp()) << msg << std::endl;

			if (m_LogOutput == logoutput::ALL || m_LogOutput == logoutput::CONSOLE) {
				std::cout << stream.str();
			}

			if (m_LogOutput == logoutput::ALL || m_LogOutput == logoutput::FILE) {
				if (m_FStream.is_open())
					m_FStream << stream.str();
			}
		}
	}

	Logger::~Logger() {
		m_FStream.close();
	}

	Logger Logger::s_Instance;

	std::string Logger::endl = "\n";
}