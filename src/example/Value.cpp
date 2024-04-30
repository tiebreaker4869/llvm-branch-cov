#include "Value.hpp"
#include "Grammer.hpp"
#include <sstream>
#include <iomanip>

std::string escapeMinimumCharacters(const std::string &str) {
		std::stringstream result;

		// For each character in the string.
		for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
			if (*i == Strings::Std::QUOTATION_MARK) {
				result << Strings::Json::QUOTATION_MARK;

			} else if (*i == Strings::Std::REVERSE_SOLIDUS) {
				result << Strings::Json::REVERSE_SOLIDUS;

			} else if (*i == Strings::Std::BACKSPACE) {
				result << Strings::Json::BACKSPACE;

			} else if (*i == Strings::Std::FORM_FEED) {
				result << Strings::Json::FORM_FEED;

			} else if (*i == Strings::Std::LINE_FEED) {
				result << Strings::Json::LINE_FEED;

			} else if (*i == Strings::Std::CARRIAGE_RETURN) {
				result << Strings::Json::CARRIAGE_RETURN;

			} else if (*i == Strings::Std::TAB) {
				result << Strings::Json::TAB;

			} else if (*i >= '\0' && *i <= '\x1f') {
				result << escapeToUnicode(*i);

			} else {
				result << *i;
			}
		}

		return result.str();
}

std::string escapeAllCharacters(const std::string &str) {
		std::stringstream result;

		// For each character in the string.
		for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
			if (*i == Strings::Std::QUOTATION_MARK) {
				result << Strings::Json::QUOTATION_MARK;

			} else if (*i == Strings::Std::REVERSE_SOLIDUS) {
				result << Strings::Json::REVERSE_SOLIDUS;

			} else if (*i == Strings::Std::SOLIDUS) {
				result << Strings::Json::SOLIDUS;

			} else if (*i == Strings::Std::BACKSPACE) {
				result << Strings::Json::BACKSPACE;

			} else if (*i == Strings::Std::FORM_FEED) {
				result << Strings::Json::FORM_FEED;

			} else if (*i == Strings::Std::LINE_FEED) {
				result << Strings::Json::LINE_FEED;

			} else if (*i == Strings::Std::CARRIAGE_RETURN) {
				result << Strings::Json::CARRIAGE_RETURN;

			} else if (*i == Strings::Std::TAB) {
				result << Strings::Json::TAB;

			} else if (*i >= '\0' && *i <= '\x1f') {
				result << escapeToUnicode(*i);

			} else {
				result << *i;
			}

		}

		return result.str();
}


std::string escapeToUnicode(char charToEscape) {
		std::stringstream result;

		if (charToEscape >= '\0' && charToEscape <= '\x1f') {
			result << "\\u00";
			result << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(charToEscape);
		}

		return result.str();
}

int main() {

	return 0;
}