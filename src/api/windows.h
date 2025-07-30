#ifndef API_WINDOWS_H
#define API_WINDOWS_H

#include <string>

namespace api::windows {

bool is_user_administrator();
void run_process(const std::string &cmd);
bool restart_as_administrator();

} // namespace api::windows

#endif
