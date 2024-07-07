#include "common.hpp"
#include "logger.hpp"

#ifdef _DEBUG
namespace CentroMidi
{
namespace Logger
{

// public
std::list<Log> logs;
const size_t MAX_DISPLAY_LOGS = 100;

// private
int Log::_next_log_id = 0;

} // Logger
} // CentroMidi
#endif