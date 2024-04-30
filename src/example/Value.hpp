#pragma once
#include <string>
		/**
		 * Replaces characters with their JSON equivalent. The only difference
		 * from escapeAllCharacters is that the solidi won't be escaped in this
		 * method.
		 * @param str String to have its characters escaped.
		 * @return Copy of the recieved string, but with the concerned
		 * characters escaped.
		 * @see JsonBox::Value::escapeAllCharacters(const std::string& str)
		 */
std::string escapeMinimumCharacters(const std::string &str);


		/**
		 * Replaces characters with its JSON equivalent for escape characters.
		 * So for example, if in the string there is the newline character '\n',
		 * it will be replaced by the two characters '\' and 'n'.
		 * @param str String make a copy and have its characters escaped.
		 * @return Copy of the recieved string, but with the concerned
		 * characters escaped.
		 * @see JsonBox::Value::escapeMinimumCharacters(const std::string& str)
		 */
std::string escapeAllCharacters(const std::string &str);


		/**
		 * Escapes a character to its unicode equivalent. This function only
		 * takes characters from '/0' to '/x1f'.
		 * @param charToEscape Character to escape, must be between '\0' and
		 * '\\x1f'.
		 * @return String with the character escaped in the format "\u00xx".
		 * "xx" being the hexadecimal ASCII code of the character escaped.
		 */
std::string escapeToUnicode(char charToEscape);