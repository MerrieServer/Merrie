#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_LOGGING_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_LOGGING_HPP

#include "Commons.hpp"

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/type_index.hpp>

/**
 * Creates a stream for writing a log message to a logger with the given severity.
 */
#define M_LOG(logger, severity) BOOST_LOG_SEV(logger, severity)

/**
 * \def M_LOG_TRACE(logger, exp)
 *
 * Writes to the logger using the given expression and trace severity if M_ENABLE_TRACE is set.
 */
#ifdef M_ENABLE_TRACE
#   define M_LOG_TRACE(logger, exp) M_LOG(logger, Merrie::LoggingSeverity::Trace) << __func__  << ": " << exp
#else
#   define M_LOG_TRACE(logger, exp)
#endif

/**
 * \def M_LOG_DEBUG(logger, exp)
 *
 * Writes to the logger using the given expression and debug severity if M_ENABLE_DEBUG is set.
 */
#ifdef M_ENABLE_DEBUG
#   define M_LOG_DEBUG(logger, exp) M_LOG(logger, Merrie::LoggingSeverity::Debug) << exp
#else
#   define M_LOG_DEBUG(logger, exp)
#endif

/**
 * Creates a stream for writing a log message to a logger with the INFO severity.
 * Equivalent of M_LOG(logger, Merrie::LoggingSeverity::INFO)
 */
#define M_LOG_INFO(logger)             M_LOG(logger, Merrie::LoggingSeverity::Info)

/**
 * Creates a stream for writing a log message to a logger with the SUCCESS severity.
 * Equivalent of M_LOG(logger, Merrie::LoggingSeverity::SUCCESS)
 */
#define M_LOG_SUCCESS(logger)          M_LOG(logger, Merrie::LoggingSeverity::Success)

/**
 * Creates a stream for writing a log message to a logger with the WARNING severity.
 * Equivalent of M_LOG(logger, Merrie::LoggingSeverity::WARNING)
 */
#define M_LOG_WARNING(logger)          M_LOG(logger, Merrie::LoggingSeverity::Warning)

/**
 * Creates a stream for writing a log message to a logger with the ERROR severity.
 * Equivalent of M_LOG(logger, Merrie::LoggingSeverity::ERROR)
 */
#define M_LOG_ERROR(logger)            M_LOG(logger, Merrie::LoggingSeverity::Error)

/**
 * Creates a stream for writing a log message to a logger with the FATAL severity.
 * Equivalent of M_LOG(logger, Merrie::LoggingSeverity::FATAL)
 */
#define M_LOG_FATAL(logger)            M_LOG(logger, Merrie::LoggingSeverity::Fatal)

/**
 * Declares a new logger as a field called m_logger.
 * This can be placed only inside a class declaration.
 * The name of the logger will be deduced from the class name.
 */
#define M_DECLARE_LOGGER               Merrie::Logger m_logger = Merrie::LoggingSystem::InitLoggerFromType(boost::typeindex::type_id_with_cvr<decltype(*this)>().pretty_name())

/**
 * Declares a new logger with the given name as a field called m_logger.
 * This can be placed only inside a class declaration.
 */
#define M_DECLARE_LOGGER_EX(name)      Merrie::Logger m_logger = Merrie::LoggingSystem::InitLogger(name)

/**
 * Creates a stream for writing a log message to a logger with the given severity.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 */
#define M_LOG_THIS(severity)           M_LOG(this->m_logger, severity)

/**
 * Writes to the the logger of this class using the given expression and TRACE severity if M_ENABLE_TRACE is set.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_TRACE(this->m_logger, exp)
 */
#define M_LOG_TRACE_THIS(exp)          M_LOG_TRACE(this->m_logger, exp)

/**
 * Writes to the the logger of this class using the given expression and DEBUG severity if M_ENABLE_DEBUG is set.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_DEBUG_THIS(this->m_logger, exp)
 */
#define M_LOG_DEBUG_THIS(exp)          M_LOG_DEBUG(this->m_logger, exp)

/**
 * Creates a stream for writing a log message to a logger with the INFO severity.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_THIS(Merrie::LoggingSeverity::INFO)
 */
#define M_LOG_INFO_THIS                M_LOG_THIS(Merrie::LoggingSeverity::Info)

/**
 * Creates a stream for writing a log message to a logger with the SUCCESS severity.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_THIS(Merrie::LoggingSeverity::SUCCESS)
 */
#define M_LOG_SUCCESS_THIS             M_LOG_THIS(Merrie::LoggingSeverity::Success)

/**
 * Creates a stream for writing a log message to a logger with the WARNING severity.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_THIS(Merrie::LoggingSeverity::WARNING)
 */
#define M_LOG_WARNING_THIS             M_LOG_THIS(Merrie::LoggingSeverity::Warning)

/**
 * Creates a stream for writing a log message to a logger with the ERROR severity.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_THIS(Merrie::LoggingSeverity::ERROR)
 */
#define M_LOG_ERROR_THIS               M_LOG_THIS(Merrie::LoggingSeverity::Error)

/**
 * Creates a stream for writing a log message to a logger with the FATAL severity.
 * This can be used only inside a class member method that has a logger initialized with M_DECLARE_LOGGER or M_DECLARE_LOGGER_EX.
 * Equivalent of M_LOG_THIS(Merrie::LoggingSeverity::FATAL)
 */
#define M_LOG_FATAL_THIS               M_LOG_THIS(Merrie::LoggingSeverity::Fatal)


namespace Merrie {


    /**
     * Represents a severity of a log message.
     */
    enum class LoggingSeverity : uint8_t {
            Trace,
            Debug,
            Info,
            Success,
            Warning,
            Error,
            Fatal,
    };

    /**
     * Basic Logger type used by Merrie
     */
    using Logger = boost::log::sources::severity_logger<LoggingSeverity>;

    /**
     * A RAII helper class for initializing and destructing the logging system.
     */
    class LoggingSystem {
        public: // Constructors & destructors

            /**
             * Init the logging system
             */
            LoggingSystem();

            /**
             * Shutdown the logging system
             */
            ~LoggingSystem();

            NON_COPYABLE(LoggingSystem);
            NON_MOVEABLE(LoggingSystem);

        public: // Public methods

            /**
              * Initialized a new logger with the given name
              * @param name name to use
              * @return the newly created logger
              */
            static Logger InitLogger(std::string name);

            /**
                * Transforms the given name of a class to human-readable simple class name and initializes a new logger with such name.
                * @param name class name to use
                * @return the newly created logger
                */
            static Logger InitLoggerFromType(std::string name);

            /**
             * Updates the filters used for filtering logging messages.
             * @param filters filters to set.
             */
            static void UpdateFilters(std::vector<std::string> filters);

            /**
             * Gets the list of the filters used for filtering logging messages.
             */
            static const std::vector<std::string>& GetFilters() noexcept;

        private: // Private variables
            struct Data;
            static std::unique_ptr<Data> s_data;
    };

} // namespace Merrie

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_LOGGING_HPP
