#ifndef API_WINDOWS_H
#define API_WINDOWS_H

#include <string>

namespace api::windows {

struct version
{
	int major;
	int minor;
	int build;
};

} // namespace api::windows

namespace api::windows {

bool is_user_administrator();
void run_process(const std::string &cmd);
bool restart_as_administrator(bool retain_app);
version get_version();

} // namespace api::windows

#endif
