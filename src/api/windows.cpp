#include "windows.h"

#include "utils.h"

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

bool api::windows::restart_as_administrator(bool retain_app)
{
	CHAR exe_name[MAX_PATH];
	::GetModuleFileNameA(nullptr, exe_name, MAX_PATH);

	SHELLEXECUTEINFOA info{};
	info.cbSize = sizeof(info);
	info.lpVerb = "runas";
	info.fMask = SEE_MASK_FLAG_NO_UI;
	info.nShow = SW_SHOW;
	info.lpFile = exe_name;
	info.lpParameters = retain_app ? "--toggle-psr-restart" : "--toggle-psr";

	if (::ShellExecuteExA(&info))
	{
		// 기존 실행 프로세스를 종료
		auto func = utils::dll::get<VOID(WINAPI *)(ULONG)>("ntdll.dll", "RtlExitUserProcess");
		func(0);

		return true;
	}

	return false;
}

} // namespace api
