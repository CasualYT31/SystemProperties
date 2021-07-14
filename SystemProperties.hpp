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

/**\file SystemProperties.hpp
 * \brief This file declares functions allowing the client to query the computer
 *        for hardware and software details.
 */

/**
 * \brief Contains functions allowing the client to query the computer for hardware
 *        and software details.
 */
namespace sys {
	/**
	 * Enumeration representing different units of memory/srorage capacity.
	 */
	enum class unit {
		Bytes,
		KB,
		MB,
		GB
	};

	/**
	 * \brief  Function which converts bytes into a different unit.
	 * \param  bytes The bytes value to convert.
	 * \param  unit  The unit of capacity to convert to.
	 * \return The converted number of bytes.
	 */
	std::uint64_t convert(const std::uint64_t bytes, const sys::unit unit);

	/**
	 * \brief  Finds out the notation of a given unit of storage capacity.
	 * \param  unit The unit of storage capacity.
	 * \return The label/notation given to the unit.
	 */
	std::string notation(const sys::unit unit);

	/**
	 * \brief Contains functions allowing the client to query the computer for CPU
	 *        details.
	 */
	namespace cpu {
		/**
		 * \brief  Retrieves the name of the CPU model.
		 * \return The user-friendly name of the CPU.
		 */
		std::string model();

		/**
		 * \brief  Retrieves the architecture of the CPU.
		 * \return The architecture of the CPU.
		 */
		std::string architecture();
	}

	/**
	 * \brief Contains functions allowing the client to query the computer for
	 *        memory details.
	 */
	namespace mem {
		/**
		 * \brief  Retrieves the total RAM installed.
		 * \param  The unit of memory capacity to convert the total RAM into. By
		 *         default, it is GB.
		 * \return The installed RAM.
		 */
		std::string total(const sys::unit unit = sys::unit::GB);
	}

	/**
	 * \brief Contains functions allowing the client to query the computer for
	 *        Operating System (software) details.
	 */
	namespace os {
		/**
		 * \brief  Retrieves the name of the running Operating System.
		 * \return The user-friendly name of the Operating System of the computer.
		 */
		std::string name();

		/**
		 * \brief  Retrieves the version of the running Operating System.
		 * \return The version of the Operating System currently running.
		 */
		std::string version();
	}

	/**
	 * \brief Contains functions allowing the client to query the computer for GPU
	 *        details.
	 */
	namespace gpu {
		/**
		 * \brief  Retrieves the vendor of the installed GPU.
		 * \return The name of the GPU's vendor.
		 */
		std::string vendor();

		/**
		 * \brief  Retrieves the name of the installed GPU.
		 * \return The user-friendly name of the GPU.
		 */
		std::string name();

		/**
		 * \brief  Retrieves the version of the graphics driver in use.
		 * \return The version of the driver.
		 */
		std::string driver();
	}

	/**
	 * \brief Contains functions allowing the client to query the computer for
	 *        information on the primary drive.
	 */
	namespace storage {
		/**
		 * \brief  Retrieves the capacity of the primary drive.
		 * \param  The unit of storage capacity to convert the bytes into. By
		 *         default, it is GB.
		 * \return Returns the capacity of the primary drive.
		 */
		std::string capacity(const sys::unit unit = sys::unit::GB);

		/**
		 * \brief  Retrieves the free space available in the primary drive.
		 * \param  The unit of memory capacity to convert the total free space
		 *         into. By default, it is GB.
		 * \return Returns the free space available on the primary drive.
		 */
		std::string free(const sys::unit unit = sys::unit::GB);
	}
}