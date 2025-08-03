#include "windows.h"

#include "utils.h"

#include <windows.h>
#include <shlobj.h>

#include <format>

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

bool windows::restart_as_administrator(const std::string &parameters)
{
	CHAR exe_name[MAX_PATH];
	::GetModuleFileNameA(nullptr, exe_name, MAX_PATH);

	SHELLEXECUTEINFOA info{};
	info.cbSize = sizeof(info);
	info.lpVerb = "runas";
	info.fMask = SEE_MASK_FLAG_NO_UI;
	info.nShow = SW_SHOW;
	info.lpFile = exe_name;

	auto params = std::format("{} {}", "--from-restart", parameters);
	info.lpParameters = params.c_str();

	if (::ShellExecuteExA(&info))
	{
		// 기존 실행 프로세스를 종료
		auto func = utils::dll::get<VOID(WINAPI *)(ULONG)>("ntdll.dll", "RtlExitUserProcess");
		func(0);

		return true;
	}

	return false;
}

windows::version windows::get_version()
{
	auto func = utils::dll::get<LONG(WINAPI *)(PRTL_OSVERSIONINFOW)>("ntdll.dll", "RtlGetVersion");

	RTL_OSVERSIONINFOW info{};
	info.dwOSVersionInfoSize = sizeof(info);

	func(&info);

	return {
		static_cast<int>(info.dwMajorVersion),
		static_cast<int>(info.dwMinorVersion),
		static_cast<int>(info.dwBuildNumber),
	};
}

} // namespace api
