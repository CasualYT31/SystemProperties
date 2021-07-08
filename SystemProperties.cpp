/*MIT License

Copyright (c) 2021 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "SystemProperties.hpp"
#include <fstream>
#include <iostream>

int main() {
	std::cout << sys::cpu::model() << std::endl;
	std::cout << sys::cpu::architecture() << std::endl;
	std::cout << sys::cpu::model() << std::endl;
	return 0;
}

#ifdef _WIN32

/**
 * \brief   Helper function used throughout the Windows implementation.
 * \details This function executes a command on the shell and retrieves the output
 *          from it. A temporary file is created in the process, meaning that this
 *          function is comparatively slow...
 * \param   className  The name of the class of device information to retrieve.
 * \param   objectName The name of the piece of device information to retrieve.
 * \return  Returns the output gleaned from executing the command.
 */
std::string get(const std::string& className, const std::string& objectName) {
	using namespace std::string_literals;
	std::string tempfile = className + "." + objectName + ".temp";
	std::string inp = "start /b powershell.exe \"Get-CimInstance -ClassName " +
		className + " | Select-Object " + objectName + " > \"" + tempfile + "\"\"";
	system(inp.c_str());
	std::ifstream ifs(tempfile);
	std::string ret;
	// get to line 4, extract line 4
	for (int x = 0; x < 4; x++) std::getline(ifs, ret);
	ifs.close();
	std::remove(tempfile.c_str());
	// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
	// we also have to account for NULL characters that Powershell likes to insert
	// into right-aligned fields...
	ret.erase(0, ret.find_first_not_of(" \0\n\r\t"s));
	ret.erase(ret.find_last_not_of(" \0\n\r\t"s) + 1);
	return ret;
}

/**
 * \brief   Macro used to ensure functions comply with a standard structure.
 * \details I've used static variables throughout the function code to reduce the
 *          impact of the slow \c get() function. Once a piece of information has
 *          been retrieved, it will likely not change again during execution, so
 *          cache it and return it when required.
 * \param   c The class name of the device information to retrieve.
 * \param   o The name of the specific piece of device information to retrieve.
 */
#define FUNCTION(c, o) \
	static std::string cache = ""; \
	if (cache == "") cache = get(c, o); \
	return cache;

/////////
// CPU //
/////////
// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/cim-processor

std::string sys::cpu::model() { FUNCTION("CIM_Processor", "name"); }

std::string sys::cpu::architecture() { FUNCTION("CIM_Processor", "addresswidth"); }

#elif __linux__



#elif _OSX



#endif