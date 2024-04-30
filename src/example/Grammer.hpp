#pragma once
#include <string>

namespace Strings {
		// C++ string characters.
		namespace Std {
			const char QUOTATION_MARK = '"';
			const char REVERSE_SOLIDUS = '\\';
			const char SOLIDUS = '/';
			const char BACKSPACE = '\b';
			const char FORM_FEED = '\f';
			const char LINE_FEED = '\n';
			const char CARRIAGE_RETURN = '\r';
			const char TAB = '\t';
		}
		// Json escape strings.
		namespace Json {
			const std::string QUOTATION_MARK = "\\\"";
			const std::string REVERSE_SOLIDUS = "\\\\";
			const std::string SOLIDUS = "\\/";
			const std::string BACKSPACE = "\\b";
			const std::string FORM_FEED = "\\f";
			const std::string LINE_FEED = "\\n";
			const std::string CARRIAGE_RETURN = "\\r";
			const std::string TAB = "\\t";
			const std::string BEGIN_UNICODE = "\\u";
			namespace Escape {
				const char BEGIN_ESCAPE = '\\';
				const char QUOTATION_MARK = '"';
				const char REVERSE_SOLIDUS = '\\';
				const char SOLIDUS = '/';
				const char BACKSPACE = 'b';
				const char FORM_FEED = 'f';
				const char LINE_FEED = 'n';
				const char CARRIAGE_RETURN = 'r';
				const char TAB = 't';
				const char BEGIN_UNICODE = 'u';
			}
		}
}