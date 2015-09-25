#include "logging.h"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include <map>

const std::map<const std::string, const el::Level> loglevel_map {
                                                                  {"debug",   el::Level::Debug},
                                                                  {"info",    el::Level::Info},
                                                                  {"verbose", el::Level::Verbose},
                                                                  {"warning", el::Level::Warning},
                                                                  {"fatal",   el::Level::Fatal},
                                                                  {"error",   el::Level::Error}
                                                                };

void logging_setup(std::vector<std::string> enabled_log_levels)
{
    el::Configurations log_config;
    log_config.setToDefault();
    log_config.setGlobally(el::ConfigurationType::Enabled, "false");

    log_config.set(el::Level::Info,    el::ConfigurationType::Format, "[%level] %msg");
    log_config.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level] %msg");
    log_config.set(el::Level::Debug,   el::ConfigurationType::Format, "[%level] %fbase:%line %msg");
    log_config.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] %fbase:%line %msg");
    log_config.set(el::Level::Fatal,   el::ConfigurationType::Format, "[%level] %fbase:%line %msg");
    log_config.set(el::Level::Error,   el::ConfigurationType::Format, "[%level] %fbase:%line %msg");
    for(auto level : enabled_log_levels) {
        log_config.set(loglevel_map.at(level), el::ConfigurationType::Enabled, "true");
    }
    el::Loggers::reconfigureLogger("default", log_config);
}
