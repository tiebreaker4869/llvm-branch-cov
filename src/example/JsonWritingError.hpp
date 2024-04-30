#pragma once
#include <stdexcept>
#include <string>

class JsonWritingError : public std::runtime_error {
	public:
		/**
		 * Constructs the exception with what as an explanatory string that can
		 * be accessed through what().
		 * @param what String describing the error.
		 */
		JsonWritingError(const std::string &what);
		
		/**
		 * Constructs the exception with what as an explanatory string that can
		 * be accessed through what().
		 * @param what String describing the error.
		 */
		JsonWritingError(const char *what);
	};