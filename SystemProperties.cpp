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

std::uint64_t sys::convert(const std::uint64_t bytes, const sys::unit unit) {
	switch (unit) {
	case sys::unit::Bytes:
		return bytes;
	case sys::unit::KB:
		return bytes / 1024;
	case sys::unit::MB:
		return bytes / 1024 / 1024;
	case sys::unit::GB:
		return bytes / 1024 / 1024 / 1024;
	default:
		return bytes;
	}
}

std::string sys::notation(const sys::unit unit) {
	switch (unit) {
	case sys::unit::Bytes:
		return " bytes";
	case sys::unit::KB:
		return "KB";
	case sys::unit::MB:
		return "MB";
	case sys::unit::GB:
		return "GB";
	default:
		return "";
	}
}

#ifdef _WIN32

/**
 * \brief  Perform the PowerShell command.
 * \param  className  The name of the class of device information to retrieve.
 * \param  objectName The name of the piece of device information to retrieve.
 * \return The name of the temporary file created to store the output of the
 *         PowerShell command.
 * \sa     get()
 */
std::string powershell(const std::string& className,
	const std::string& objectName) {
	std::string tempfile = className + objectName + ".temp";
	std::string inp = "start /wait /b powershell.exe \"Get-CimInstance -ClassName "
		+ className + " | Select-Object " + objectName + " > \"" + tempfile +
		"\"\"";
	system(inp.c_str());
	return tempfile;
}

/**
 * \brief   Reads a line from a temporary file and cleans it up.
 * \details The string is trimmed on both ends, and all extra bytes are removed.
 * \param   ifs The \c ifstream to read from.
 */
std::string getcleanline(std::ifstream& ifs) {
	std::string ret;
	std::getline(ifs, ret);
	// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
	// we also have to account for NULL characters:
	// I tried getting wstrings to work but it just wasn't happening...
	// I opted for simply removing the extra bytes (i.e. the NULLs)
	// https://stackoverflow.com/questions/20326356/how-to-remove-all-the-occurrences-of-a-char-in-c-string
	ret.erase(std::remove(ret.begin(), ret.end(), '\0'), ret.end());
	ret.erase(0, ret.find_first_not_of(" \n\r\t"));
	ret.erase(ret.find_last_not_of(" \n\r\t") + 1);
	return ret;
}

/**
 * \brief   Helper function used throughout the Windows implementation.
 * \details This function executes a command on the shell and retrieves the output
 *          from it. A temporary file is created in the process, meaning that this
 *          function is comparatively slow...
 * \param   className  The name of the class of device information to retrieve.
 * \param   objectName The name of the piece of device information to retrieve.
 * \return  Returns the output gleaned from executing the command.
 * \sa      powershell()
 */
std::string get(const std::string& className, const std::string& objectName) {
	std::string tempfile = powershell(className, objectName);
	std::ifstream ifs(tempfile);
	std::string ret;
	// get to line 4, extract line 4
	for (int x = 0; x < 4; x++) ret = getcleanline(ifs);
	ifs.close();
	remove(tempfile.c_str());
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
	if (cache == "") { cache = get(c, o); } \
	return cache;

/////////
// CPU //
/////////
// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/cim-processor

std::string sys::cpu::model() { FUNCTION("CIM_Processor", "name"); }

std::string sys::cpu::architecture() { FUNCTION("CIM_Processor", "addresswidth"); }

////////////
// MEMORY //
////////////
// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/cim-physicalmemory

std::string sys::mem::total(const sys::unit unit) {
	static std::string cache = "";
	if (cache == "") {
		std::string tempfile = powershell("CIM_PhysicalMemory", "capacity");
		std::ifstream ifs(tempfile);
		std::string ret;
		// skip first three lines
		for (int x = 0; x < 3; x++) std::getline(ifs, ret);
		// continue on and count up each memory stick's capacity to calculate the
		// total
		std::uint64_t count = 0;
		for (;;) {
			ret = getcleanline(ifs);
			if (ret == "") break;
			count += std::stoll(ret);
		}
		ifs.close();
		remove(tempfile.c_str());
		cache = std::to_string(sys::convert(count, unit)) + sys::notation(unit);
	}
	return cache;
}

////////
// OS //
////////
// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-operatingsystem

std::string sys::os::name() { FUNCTION("Win32_OperatingSystem", "caption"); }

std::string sys::os::version() { FUNCTION("Win32_OperatingSystem", "version"); }

/////////
// GPU //
/////////
// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-videocontroller

std::string sys::gpu::vendor() {
	FUNCTION("Win32_VideoController", "adaptercompatibility");
}

std::string sys::gpu::name() { FUNCTION("Win32_VideoController", "name"); }

std::string sys::gpu::driver() {
	FUNCTION("Win32_VideoController", "driverversion");
}

/////////////
// STORAGE //
/////////////
// https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-logicaldisk

std::string sys::storage::capacity(const sys::unit unit) {
	static std::string cache = "";
	if (cache == "") {
		std::string tempfile = powershell("Win32_LogicalDisk", "size");
		std::ifstream ifs(tempfile);
		std::string ret;
		// skip first three lines, read fourth line
		for (int x = 0; x < 4; x++) std::getline(ifs, ret);
		std::cout << ret << std::endl;
		std::uint64_t count = std::stoll(ret);
		ifs.close();
		remove(tempfile.c_str());
		cache = std::to_string(sys::convert(count, unit)) + sys::notation(unit);
	}
	return cache;
}

std::string sys::storage::free(const sys::unit unit) {
	static std::string cache = "";
	if (cache == "") {
		std::string tempfile = powershell("Win32_LogicalDisk", "freespace");
		std::ifstream ifs(tempfile);
		std::string ret;
		// skip first three lines, read fourth line
		for (int x = 0; x < 4; x++) std::getline(ifs, ret);
		std::uint64_t count = std::stoll(ret);
		ifs.close();
		remove(tempfile.c_str());
		cache = std::to_string(sys::convert(count, unit)) + sys::notation(unit);
	}
	return cache;
}

#elif __linux__



#elif _OSX



#endif