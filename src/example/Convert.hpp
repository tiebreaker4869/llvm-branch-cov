#pragma once
#include <string>
#include <vector>

class String32 {
    public:
    String32();
    int size() const;
    int at(int index) const;
    void push_back(int value);

    private:
        std::vector<int> m_data;
};

class Convert {
	public:
        Convert();
		/**
		 * Encode the given UTF32 string to a 8bit UTF8 one.
		 * @param utf32String UTF32 string to convert to UTF8.
		 * @return UTF8 string resulting from the conversion.
		 */
		std::string encodeToUTF8(const String32& utf32String);

		/**
		 * Decode the given 8bit UTF8 string to an UTF32 string.
		 * @param utf8String UTF8 string to convert to UTF32.
		 * @return UTF32 string resulting from the conversion.
		 */
		String32 decodeUTF8(const std::string& utf8String);
};