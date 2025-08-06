#include "../include/environment.hpp"
#include "../include/logger.hpp"
using namespace cobble;

environment::configuration environment::load(const std::filesystem::path &where) {
    configuration config;
    logger::log(logger::severity::informational, "Loading configuration '", where, "'");
    toml::table table;

    table = toml::parse_file(where.string());

    return config;
}