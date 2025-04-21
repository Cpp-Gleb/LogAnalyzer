#ifndef LOGANALYZER_H
#define LOGANALYZER_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "LogEntry.h"


class LogAnalyzer {
public:
    LogAnalyzer(const std::string& log_filename, const std::string& output_file,
        bool is_print, int stats_count, int window_time, long from_time, long to_time);
    void ParseLogFile();
    void OutputErrors() const;
    void AnalyzeRequests() const;
    void OutputTopRequests() const;

private:
    std::vector<LogEntry> entries_;
    std::string log_filename_;
    std::string output_file_;
    bool is_print_;
    int stats_count_;
    int window_time_;
    long from_time_;
    long to_time_;
    int kUpperStatusError_ = 600;
    int kLowerStatusError_ = 500;
    long ConvertTimeToSeconds_(const std::string& local_time);
};


#endif //LOGANALYZER_H
