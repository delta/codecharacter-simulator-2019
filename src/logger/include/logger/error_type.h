/**
 * file error_type.h
 * Defines a type to represent a particular category of errors
 */

#pragma once

#include <string>
#include <vector>

namespace logger {

/**
 * Enum representing different categories of errors. Errors from the same
 * category can potentially be grouped
 *
 * ALERT! - When adding a value here, add it in the string mapping below as well
 */
enum class ErrorType {
	// TODO: Add error types
};

/**
 * Provides an easy way to get the string representation of the above enum
 */
const std::vector<std::string> ErrorTypeName = {
    //
};

} // namespace logger
