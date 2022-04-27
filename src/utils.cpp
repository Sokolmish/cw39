#include "utils.hpp"

cw39_exception::cw39_exception(std::string excClass, std::string loc, std::string msg)
        : std::exception(), excClass(std::move(excClass)),
          loc(std::move(loc)), msg(std::move(msg)) {}

const char *cw39_exception::what() const noexcept {
    return msg.c_str();
}

std::string cw39_exception::prettyWhat() const {
    if (loc.empty())
        return fmt::format("{}:\n\t{}\n", excClass, msg);
    return fmt::format("{} ({}):\n\t{}\n", excClass, loc, msg);
}


cw39_internal_error::cw39_internal_error(std::string msg, const source_location &l)
        : cw39_exception("Internal error", formLoc(l), std::move(msg)) {}

std::string cw39_internal_error::formLoc(const source_location &loc) const {
    return fmt::format("{}:{}", basename(loc.file_name()), loc.line());
}


cw39_not_implemented::cw39_not_implemented(const std::string &msg, const std::source_location &l)
        : cw39_internal_error("Not implemented: " + msg, l) {}


cw39_error::cw39_error(std::string msg) : cw39_exception("General error", "", std::move(msg)) {}
