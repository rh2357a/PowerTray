#include "windows.h"

#include <windows.h>
#include <shlobj.h>

namespace api {

bool windows::is_user_administrator()
{
	return ::IsUserAnAdmin();
}

void windows::run_process(const std::string &cmd)
{
	PROCESS_INFORMATION pi{};
	STARTUPINFOA si{};
	si.cb = sizeof(STARTUPINFOA);
	if (::CreateProcessA(nullptr, (LPSTR)cmd.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
	{
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}
}

} // namespace api
