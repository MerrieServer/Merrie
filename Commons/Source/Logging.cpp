#include <Commons/Logging.hpp>

#include <iomanip>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/phoenix.hpp>
#include <boost/utility/string_view.hpp>

#ifndef M_SUPPORTS_ANSI
#   if defined(M_PLATFORM_UNIX) || defined(M_PLATFORM_MACOS)
#       define M_SUPPORTS_ANSI
#   endif
#endif

namespace Merrie {
    BOOST_LOG_ATTRIBUTE_KEYWORD(g_nameAttr, "Name", std::string);
    BOOST_LOG_ATTRIBUTE_KEYWORD(g_severityAttr, "Severity", LoggingSeverity);

    namespace logging = boost::log;
    namespace sinks = logging::sinks;
    namespace kw = logging::keywords;

    namespace {
        /**
         * String representations of severity values for sinks supproting ANSII escape codes.
         */
        const boost::string_view g_severityValues[7] = {
                "  \033[36mTRACE\033[0m",
                "  \033[35mDEBUG\033[0m",
                "   \033[37mINFO\033[0m",
                "\033[32mSUCCESS\033[0m",
                "\033[33mWARNING\033[0m",
                "  \033[31mERROR\033[0m",
                "  \033[1;31mFATAL\033[0m",
        };

        /**
        * String representations of severity values for sinks NOT supproting ANSII escape codes.
        */
        const boost::string_view g_rawSeverityValues[7] = {
                "  TRACE",
                "  DEBUG",
                "   INFO",
                "SUCCESS",
                "WARNING",
                "  ERROR",
                "  FATAL",
        };

        /**
         * A formatting function with deciding whether or not to use colored or non-colored severity names.
         */
        const boost::string_view __Formatter(bool colored, const logging::value_ref<LoggingSeverity, tag::g_severityAttr>& level) {
            #ifdef M_SUPPORTS_ANSI
            if (colored)
                return g_severityValues[static_cast<size_t>(level.get())];
            else
                return g_rawSeverityValues[static_cast<size_t>(level.get())];
            #endif

            return g_rawSeverityValues[static_cast<size_t>(level.get())];
        }

        /**
         * Creates a new logging format.
         * 
         * @param colored whether or not this format should be colored.
         * @return the newly created format.
         */
        inline auto __CreateFormat(const bool colored) {
            namespace expr = boost::log::expressions;

            const auto format = expr::stream
                    << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "]"
                    << "[" << std::setw(24) << g_nameAttr << "]"
                    << " " << boost::phoenix::bind(&__Formatter, colored, g_severityAttr.or_none()) << " "
                    << expr::smessage;

            return format;
        }

        /**
         * Mode of a filter.
         * Stdout - only log messge levels with severity lower than Warning.
         * Stderr - only log messge levels with severity greater or equal to Warning.
         * Stdout - log all messages.
         */
        enum class Mode {
                Stdout,
                Stderr,
                All
        };

        /**
         * Message filter, logs messages based on the mode and the given filters.
         */
        bool __NameFilter(
                Mode mode,
                const std::vector<std::string>& filters,
                const logging::value_ref<LoggingSeverity, tag::g_severityAttr>& level,
                const logging::value_ref<std::string, tag::g_nameAttr>& name) {

            if (mode == Mode::Stdout && level >= LoggingSeverity::Warning) return false;
            if (mode == Mode::Stderr && level < LoggingSeverity::Warning) return false;

            return std::find(begin(filters), end(filters), name) == end(filters);
        }

    }

    struct LoggingSystem::Data {
        std::vector<std::string> m_filters{};
        boost::shared_ptr<sinks::synchronous_sink<sinks::text_file_backend>> m_fileSink{};
        boost::shared_ptr<sinks::synchronous_sink<sinks::text_ostream_backend>> m_stdout{};
        boost::shared_ptr<sinks::synchronous_sink<sinks::text_ostream_backend>> m_stderr{};
    };
    std::unique_ptr<LoggingSystem::Data> LoggingSystem::s_data = std::make_unique<LoggingSystem::Data>();


    LoggingSystem::LoggingSystem() {
        boost::shared_ptr<boost::log::core> core = boost::log::core::get();

        // Init format
        core->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());

        const auto coloredFormat = __CreateFormat(true);
        const auto notColoredFormat = __CreateFormat(false);

        // Sinks
        s_data->m_fileSink = boost::make_shared<sinks::synchronous_sink<sinks::text_file_backend>>(
                kw::file_name = "logs/%Y-%m-%d_%N.log",
                kw::rotation_size = 10 * 1024 * 1024, // 10 mb
                kw::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                kw::open_mode = std::ios_base::app
        );
        s_data->m_fileSink->set_formatter(notColoredFormat);
        s_data->m_fileSink->locked_backend()->auto_flush(true);
        core->add_sink(s_data->m_fileSink);

        s_data->m_stdout = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();
        s_data->m_stdout->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::cout, boost::null_deleter()));
        s_data->m_stdout->locked_backend()->auto_flush(true);

        s_data->m_stdout->set_formatter(coloredFormat);
        core->add_sink(s_data->m_stdout);

        s_data->m_stderr = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();
        s_data->m_stderr->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::cerr, boost::null_deleter()));
        s_data->m_stderr->locked_backend()->auto_flush(true);
        s_data->m_stderr->set_formatter(coloredFormat);
        core->add_sink(s_data->m_stderr);

        // Update filters
        UpdateFilters(s_data->m_filters);
    }

    LoggingSystem::~LoggingSystem() = default;


    Logger LoggingSystem::InitLogger(std::string name) {
        Logger logger;
        logger.add_attribute(g_nameAttr.get_name(), boost::log::attributes::constant<std::string>(std::move(name)));
        return logger;
    }

    Logger LoggingSystem::InitLoggerFromType(std::string name) {
        // Find template parameters if the exist in the name and remove them
        auto templateParameterStart = boost::find_first(name, "<");
        if (!templateParameterStart.empty())
            name.erase(begin(templateParameterStart), end(name));

        // Some compilers add "class " before the class name
        if (boost::starts_with(name, "class "))
            name.erase(begin(name), begin(name) + 6);

        // Remove Merrie:: namespace, it is redundant, we assume everything is in this namespace (or a sub-namespace).
        if (boost::starts_with(name, "Merrie::"))
            name.erase(begin(name), end(boost::find_last(name, "::")));

        // Remove everything after the reference symbol.
        auto refStart = boost::find_first(name, "&");

        if (!refStart.empty())
            name.erase(begin(refStart), end(name));

        // Trim the name from any useless whitespaces
        boost::trim(name);

        return InitLogger(std::move(name));
    }


    void LoggingSystem::UpdateFilters(std::vector<std::string> filters) {
        s_data->m_filters = std::move(filters);

        if (s_data->m_fileSink)
            s_data->m_fileSink->set_filter(boost::phoenix::bind(&__NameFilter, Mode::All, s_data->m_filters, g_severityAttr.or_none(), g_nameAttr.or_none()));
        if (s_data->m_stdout)
            s_data->m_stdout->set_filter(boost::phoenix::bind(&__NameFilter, Mode::Stdout, s_data->m_filters, g_severityAttr.or_none(), g_nameAttr.or_none()));
        if (s_data->m_stderr)
            s_data->m_stderr->set_filter(boost::phoenix::bind(&__NameFilter, Mode::Stderr, s_data->m_filters, g_severityAttr.or_none(), g_nameAttr.or_none()));
    }

    const std::vector<std::string>& LoggingSystem::GetFilters() noexcept {
        return s_data->m_filters;
    }

} // namespace Merrie
