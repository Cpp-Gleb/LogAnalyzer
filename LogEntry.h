#ifndef LODENTRY_H
#define LODENTRY_H
#include <iostream>

enum FinishCode {
    kSucces,
    kError
};

class LogEntry {
public:
    LogEntry(const std::string& remote_addr, long local_time, const std::string& request, int status, int bytes_send);
    int GetStatus() const;
    long GetLocalTime() const;
    const std::string& GetRemoteAddr() const;
    const std::string& GetRequest() const;
    int GetBytesSend() const;
    std::ostream& LogEntry::OutputLogs(std::ostream& os) const;

private:
    std::string remote_addr_;
    long local_time_;
    std::string request_;
    int status_;
    int bytes_send_;
};



#endif //LODENTRY_H
