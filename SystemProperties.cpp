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

std::uint64_t System::convert(const std::uint64_t bytes, const System::Unit unit)
	noexcept {
	switch (unit) {
	case System::Unit::Bytes:
		return bytes;
	case System::Unit::KB:
		return bytes / 1024;
	case System::Unit::MB:
		return bytes / 1024 / 1024;
	case System::Unit::GB:
		return bytes / 1024 / 1024 / 1024;
	default:
		return bytes;
	}
}

std::string System::notation(const System::Unit unit) noexcept {
	switch (unit) {
	case System::Unit::Bytes:
		return " bytes";
	case System::Unit::KB:
		return "KB";
	case System::Unit::MB:
		return "MB";
	case System::Unit::GB:
		return "GB";
	default:
		return "";
	}
}

////////////////////////////
// WINDOWS IMPLEMENTATION //
////////////////////////////
#ifdef _WIN32
// https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer
// this web page was invaluable

System::Properties::Properties() {
	HRESULT res;
	
	// step 1: initialise COM
	res = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(res)) {
		throw std::system_error(std::error_code(res, std::system_category()),
			"Failed to initialise COM");
	}

	// step 2: set general COM security levels
	res = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);
	if (FAILED(res)) {
		CoUninitialize();
		throw std::system_error(std::error_code(res, std::system_category()),
			"Failed to initialise COM security");
	}

	// step 3: obtain the initial locator to WMI
	res = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&_pLoc);
	if (FAILED(res)) {
		CoUninitialize();
		throw std::system_error(std::error_code(res, std::system_category()),
			"Failed to create IWbemLocator object");
	}

	// step 4: connect to WMI
	res = _pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object
		&_pSvc                   // pointer to IWbemServices proxy
	);
	if (FAILED(res)) {
		_pLoc->Release();
		CoUninitialize();
		throw std::system_error(std::error_code(res, std::system_category()),
			"Failed to create IWbemServices proxy");
	}

	// step 5: set security levels on the proxy
	res = CoSetProxyBlanket(
		_pSvc,                       // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);
	if (FAILED(res)) {
		_pSvc->Release();
		_pLoc->Release();
		CoUninitialize();
		throw std::system_error(std::error_code(res, std::system_category()),
			"Failed to initialise IWbemServices proxy security");
	}
}

System::Properties::~Properties() noexcept {
	if (_pSvc) _pSvc->Release();
	if (_pLoc) _pLoc->Release();
	CoUninitialize();
}

std::variant<std::vector<std::int64_t>, std::vector<std::uint64_t>,
	std::vector<std::string>> System::Properties::_wmiRequest(
	const char* className, const char* objectName, const CIMTYPE datatype) {
	HRESULT res;

	// step 1: construct WMI query string
	// format: SELECT x FROM y\0
	std::size_t qryBuffSize = 14 + strlen(className) + strlen(objectName);
	char* qry = (char*)calloc(qryBuffSize, sizeof(char));
	strncpy_s(qry, qryBuffSize, "SELECT ", strlen("SELECT "));
	strncat_s(qry, qryBuffSize, objectName, strlen(objectName));
	strncat_s(qry, qryBuffSize, " FROM ", strlen(" FROM "));
	strncat_s(qry, qryBuffSize, className, strlen(className));

	// step 2: use the IWbemServices proxy to make the request to the WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	res = _pSvc->ExecQuery(bstr_t("WQL"), bstr_t(qry),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	free(qry);
	if (FAILED(res)) {
		std::string e = "Failed to perform query for WMI object ";
		e.append(className);
		e.append(".");
		e.append(objectName);
		throw std::system_error(std::error_code(res, std::system_category()), e);
	}

	// step 3: get the data from the query's result
	std::variant<std::vector<std::int64_t>, std::vector<std::uint64_t>,
		std::vector<std::string>> ret;
	std::size_t numberOfValues = 0;
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	while (pEnumerator) {
		res = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (!uReturn) break;
		VARIANT vtProp = VARIANT();
		wchar_t wtext[500]; // no object name is going to be this long... right?
		std::size_t unused = 0;
		mbstowcs_s(&unused, wtext, 500, objectName, strlen(objectName)+1);
		// unfortunately, retrieving the CIM data type here is not reliable
		// for example, CIM_Processor.AddressWidth is stored as a number, as this
		// function says it should be,
		// but CIM_PhysicalMemory.Capacity is stored as a series of strings, even
		// though this function says it should be numeric??? this happens with
		// Win32_LogicalDisk, too. idk man...
		res = pclsObj->Get(wtext, 0, &vtProp, NULL, 0);

		switch (datatype) {
		case CIM_STRING:
			// setup vector
			if (numberOfValues == 0) ret = std::vector<std::string>();

			{ std::wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
			int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &ws[0],
				(int)ws.size(), NULL, 0, NULL, NULL);
			std::get<std::vector<std::string>>(ret).push_back(
				std::string(sizeNeeded, 0));
			WideCharToMultiByte(CP_UTF8, 0, &ws[0], (int)ws.size(),
				&(std::get<std::vector<std::string>>(ret)[numberOfValues])[0],
				sizeNeeded, NULL, NULL); }
			break;
		case CIM_SINT8:
		case CIM_SINT16:
		case CIM_SINT32:
		case CIM_SINT64:
			// setup vector
			if (numberOfValues == 0) ret = std::vector<std::int64_t>();
			std::get<std::vector<std::int64_t>>(ret).push_back(
				(std::int64_t)vtProp.llVal);
			break;
		case CIM_UINT8:
		case CIM_UINT16:
		case CIM_UINT32:
		case CIM_UINT64:
			// setup vector
			if (numberOfValues == 0) ret = std::vector<std::uint64_t>();
			std::get<std::vector<std::uint64_t>>(ret).push_back(
				(std::uint64_t)vtProp.ullVal);
			break;
		}

		VariantClear(&vtProp);
		pclsObj->Release();
		numberOfValues++;
	}

	pEnumerator->Release();
	return ret;
}

std::string System::Properties::CPUModel() {
	return std::get<std::vector<std::string>>(
		_wmiRequest("CIM_Processor", "name", CIM_STRING))[0];
}

std::string System::Properties::CPUArchitecture() {
	return std::to_string(std::get<std::vector<std::uint64_t>>(
		_wmiRequest("CIM_Processor", "addresswidth", CIM_UINT64))[0]);
}

std::string System::Properties::RAMTotal(const System::Unit unit) {
	std::uint64_t total = 0;
	std::vector<std::string> bars = std::get<std::vector<std::string>>(
		_wmiRequest("CIM_PhysicalMemory", "capacity", CIM_STRING));
	for (auto& i : bars) total += std::stoull(i);
	return std::to_string(System::convert(total, unit)) + System::notation(unit);
}

std::string System::Properties::OSName() {
	return std::get<std::vector<std::string>>(
		_wmiRequest("Win32_OperatingSystem", "caption", CIM_STRING))[0];
}

std::string System::Properties::OSVersion() {
	return std::get<std::vector<std::string>>(
		_wmiRequest("Win32_OperatingSystem", "version", CIM_STRING))[0];
}

std::string System::Properties::GPUVendor() {
	return std::get<std::vector<std::string>>(
		_wmiRequest("Win32_VideoController", "adaptercompatibility",
			CIM_STRING))[0];
}

std::string System::Properties::GPUName() {
	return std::get<std::vector<std::string>>(
		_wmiRequest("Win32_VideoController", "name", CIM_STRING))[0];
}

std::string System::Properties::GPUDriver() {
	return std::get<std::vector<std::string>>(
		_wmiRequest("Win32_VideoController", "driverversion", CIM_STRING))[0];
}

std::string System::Properties::StorageTotal(const System::Unit unit) {
	std::string total = std::get<std::vector<std::string>>(
		_wmiRequest("Win32_LogicalDisk", "size", CIM_STRING))[0];
	return std::to_string(System::convert(std::stoull(total), unit)) +
		System::notation(unit);
}

std::string System::Properties::StorageFree(const System::Unit unit) {
	std::string total = std::get<std::vector<std::string>>(
		_wmiRequest("Win32_LogicalDisk", "freespace", CIM_STRING))[0];
	return std::to_string(System::convert(std::stoull(total), unit)) +
		System::notation(unit);
}

#endif