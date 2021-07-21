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

#pragma once

#include <string>
#include <system_error>

#ifdef _WIN32
	#include <variant>
	#include <vector>
	#define _WIN32_DCOM
	#include <comdef.h>
	#include <WbemIdl.h>
	#pragma comment(lib, "wbemuuid.lib")
#endif

/**\file  SystemProperties.hpp
 * \brief This file declares the class which allows the client to query the
 *        computer for hardware and software information.
 */

/**
 * \brief The \c System namespace contains the \c Properties class.
 */
namespace System {
	/**
	 * \brief   The units of memory available for the client to use.
	 * \remarks If more units are added, do not forget to update
	 *          \c System::convert() and \c System::notation() accordingly.
	 */
	enum class Unit {
		Bytes,
		KB,
		MB,
		GB
	};

	/**
	 * \brief  This function converts a given number of bytes into a given unit of
	 *         memory.
	 * \param  bytes The bytes to convert, in \c System::Unit::Bytes.
	 * \param  unit  The unit of memory to convert the given number of bytes to.
	 * \return The number of bytes, converted.
	 */
	std::uint64_t convert(const std::uint64_t bytes, const System::Unit unit)
		noexcept;

	/**
	 * \brief  This function retrieves the notation for the given unit of memory.
	 * \param  unit The unit of memory to retrieve the notation of.
	 * \return The notation of the given memory unit.
	 */
	std::string notation(const System::Unit unit) noexcept;

	/**
	 * \brief   This class lets the client query the computer for hardware and
	 *          software information.
	 * \warning Due to limitations within the Windows implementation, this class
	 *          should only be instantiated \b once, and kept alive for as long as
	 *          necessary, before it is disposed of. It should \b not be
	 *          instantiated more than once in a program, regardless of platform!
	 * \warning In addition, within the Windows implementation, this library makes
	 *          use of the COM library. Within the constructor, the following
	 *          functions are called: \c CoInitializeEx(),
	 *          \c CoInitializeSecurity(), \c CoCreateInstance(),
	 *          \c IWbemLocator::ConnectServer(), and \c CoSetProxyBlanket().
	 *          Microsoft in their Windows API documentation instructs the
	 *          programmer to avoid calling most of these functions more than once
	 *          in a program, so if you call these functions at all within your own
	 *          code, you will likely not be able to use this library verbatim. You
	 *          can extract the relevant code for yourself, or you may amend the
	 *          constructor to fit around your application, if this is the case.
	 */
	class Properties {
	public:
		/**
		 * \brief   Initialises the connection between the program and the
		 *          computer.
		 * \warning If this constructor throws, \b do \b not use the resulting
		 *          object, as it will be unsafe to do so (accessing NULL pointers
		 *          internally, etc.).
		 * \throws  std::system_error if initialisation failed. An OS-specific
		 *          code and error string will be generated.
		 */
		Properties();

		/**
		 * \brief Safely destroys the connection between the program and the
		 *        computer.
		 */
		~Properties() noexcept;

		/**
		 * \brief  Retrieves the CPU model name.
		 * \return User-friendly name of the CPU.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string CPUModel();

		/**
		 * \brief  Retrieves the CPU architecture.
		 * \return Architecture of the CPU.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string CPUArchitecture();

		/**
		 * \brief  Retrieves the total installed RAM available.
		 * \param  unit The unit of memory to return the total RAM in. By default,
		 *              it is \c System::Unit::GB.
		 * \return The total RAM installed.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string RAMTotal(const System::Unit unit = System::Unit::GB);

		/**
		 * \brief  Retrieves the name of the OS the machine is running.
		 * \return User-friendly OS name.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string OSName();

		/**
		 * \brief  Retrieves the version of the OS the machine is running.
		 * \return Version string.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string OSVersion();

		/**
		 * \brief  Retrieves the vendor of the currently installed GPU.
		 * \return The name of the vendor of the installed GPU.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string GPUVendor();

		/**
		 * \brief  Retrieves the name of the currently installed GPU.
		 * \return User-friendly name of the installed GPU.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string GPUName();

		/**
		 * \brief  Retrieves the version of the driver the installed GPU is using.
		 * \return Version string.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string GPUDriver();

		/**
		 * \brief  Retrieves the capacity of the primary drive.
		 * \param  unit The unit of memory to return the capacity in. By default,
		 *              it is \c System::Unit::GB.
		 * \return The capacity of the primary drive.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string StorageTotal(const System::Unit unit = System::Unit::GB);

		/**
		 * \brief  Retrieves the amount of free space on the primary drive.
		 * \param  unit The unit of memory to return the free space in. By default,
		 *              it is \c System::Unit::GB.
		 * \return The free space of the primary drive.
		 * \throws std::system_error if the the request failed. An OS-specific
		 *         code and error string will be generated.
		 */
		std::string StorageFree(const System::Unit unit = System::Unit::GB);
	private:
#ifdef _WIN32
		/**
		 * \brief  Make a request to the WMI and retrieve the output.
		 * \param  className  Name of the WMI class containing the information to
		 *                    retrieve.
		 * \param  objectName Name of the WMI object within the WMI class
		 *                    containing the information to retrieve.
		 * \param  datatype   The CIM data type of the information to retrieve.
		 * \return The output received from the WMI.
		 * \throws std::system_error if the the request failed. A
		 *         Windows-specific code and error string will be generated.
		 */
		std::variant<std::vector<std::int64_t>, std::vector<std::uint64_t>,
			std::vector<std::string>>
			_wmiRequest(const char* className, const char* objectName,
				const CIMTYPE datatype);

		/**
		 * \brief   Pointer to the WMI locator.
		 * \details This will be manually released in the destructor.
		 */
		IWbemLocator* _pLoc = 0;

		/**
		 * \brief   Pointer to the WMI services.
		 * \details This will be manually released in the destructor.
		 */
		IWbemServices* _pSvc = 0;
#endif
	};
}

#ifdef _WIN32
	// just in case leaving this in could cause problems in another's Windows code
	// idk if this is even a useful thing to do, but it can't hurt... hopefully...
	#undef _WIN32_DCOM
#endif