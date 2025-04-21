#include "LogEntry.h"


LogEntry::LogEntry(const std::string& remote_addr, long local_time, const std::string& request, int status, int bytes_send)
    : remote_addr_(remote_addr), local_time_(local_time), request_(request), status_(status), bytes_send_(bytes_send) {}

int LogEntry::GetStatus() const { return status_; }
long LogEntry::GetLocalTime() const { return local_time_; }
const std::string& LogEntry::GetRemoteAddr() const { return remote_addr_; }
const std::string& LogEntry::GetRequest() const { return request_; }
int LogEntry::GetBytesSend() const { return bytes_send_; }

std::ostream& LogEntry::OutputLogs(std::ostream& out_stream) const {
    out_stream << GetRemoteAddr() << " - - ["
       << GetLocalTime() << "] \""
       << GetRequest() << "\" "
       << GetStatus() << " "
       << GetBytesSend() << "\n";
    return out_stream;
}
