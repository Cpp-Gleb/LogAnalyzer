#include "LogAnalyzer.h"

LogAnalyzer::LogAnalyzer(const std::string& log_filename, const std::string& output_file, bool is_print,
    int stats_count, int window_time, long from_time, long to_time): log_filename_(log_filename),
    output_file_(output_file), is_print_(is_print), stats_count_(stats_count), window_time_(window_time),
    from_time_(from_time), to_time_(to_time) {}

long LogAnalyzer::ConvertTimeToSeconds_(const std::string& local_time) {
    std::tm tm = {};
    std::istringstream ss(local_time);
    std::string time_without_tz = local_time.substr(0, local_time.find_last_of(' '));

    ss >> std::get_time(&tm, "%d/%b/%Y:%H:%M:%S");
    if (ss.fail()) {
        std::cerr << "Time parsing error: " << local_time << "\n";
        return kSucces;
    }

    return std::mktime(&tm);
}

void LogAnalyzer::ParseLogFile() {
    std::ifstream file(log_filename_);
    std::string line;

    std::regex log_pattern("(\\S+) - - \\[(.+?)\\] \"(.+?)\" (\\d{3}) (\\d+|-)");
    std::smatch match;

    while (std::getline(file, line)) {
        if (std::regex_match(line, match, log_pattern)) {
            std::string remote_addr = match[kRemoteAddress_];
            std::string local_time = match[kLocalTime_];
            std::string request = match[kRequest_];
            int status = std::stoi(match[kStatus_]);
            int bytes_send = match[kBytesSend_] == "-" ? 0 : std::stoi(match[kBytesSend_]);
            long entry_time = ConvertTimeToSeconds_(local_time);

            entries_.emplace_back(remote_addr, entry_time, request, status, bytes_send);
        }
    }
}

void LogAnalyzer::OutputErrors() const {
    std::ofstream out_file_stream;
    if (!output_file_.empty()) {
        out_file_stream.open(output_file_);
        if (!out_file_stream) {
            std::cerr << "Error opening file for writing: " << output_file_ << "\n";
            return;
        }
    }

    bool has_errors = false;
    for (const auto& entry : entries_) {
        long entry_time = entry.GetLocalTime();
        if (entry.GetStatus() >= kLowerStatusError_ && entry.GetStatus() < kUpperStatusError_ && entry_time >= from_time_ && entry_time <= to_time_) {
            has_errors = true;
            entry.OutputLogs(out_file_stream);
            if (is_print_) {
                entry.OutputLogs(std::cout);
            }
        }
    }

    if (!has_errors) {
        out_file_stream << "No 5XX errors found.\n";
        if(is_print_) {
            std::cout<< "No 5XX errors found.\n";
        }
    }
}

void LogAnalyzer::AnalyzeRequests() const {

    std::vector<long> timestamps;
    std::vector<const LogEntry*> entries_in_window;

    for (const auto& entry : entries_) {
        long entry_time = entry.GetLocalTime();
        // Проверяем, что временная метка попадает в диапазон
        if (entry_time >= from_time_ && entry_time <= to_time_) {
            timestamps.push_back(entry.GetLocalTime());
        }
    }

    std::sort(timestamps.begin(), timestamps.end());

    int max_requests = 0;
    long start_time = 0, end_time = 0;

    for (size_t i = 0; i < timestamps.size(); ++i) {
        int count = 0;
        long window_start = timestamps[i];
        long window_end = window_start + window_time_;
        std::vector<const LogEntry*> current_window_entries;

        for (size_t j = i; j < timestamps.size(); ++j) {
            long current_time = timestamps[j];

            if (current_time >= window_end) {
                break;
            }
            ++count;
            current_window_entries.push_back(&entries_[j]);
        }

        if (count > max_requests) {
            max_requests = count;
            start_time = window_start;
            end_time = window_end;
            entries_in_window = current_window_entries;
        }
    }

    std::cout << std::endl << "Maximum number of requests: " << max_requests
              << "in the time window from " << start_time << " to " << end_time << "\n";

    std::cout << "Logs in this time window:\n";
    for (const auto* entry : entries_in_window) {
        entry->OutputLogs(std::cout);
    }
}

void LogAnalyzer::OutputTopRequests() const {
    std::unordered_map<std::string, int> request_count;

    for (const auto& entry : entries_) {
        long entry_time = entry.GetLocalTime();

        if (entry.GetStatus() >= kLowerStatusError_ && entry.GetStatus() < kUpperStatusError_ && entry_time >= from_time_ && entry_time <= to_time_) {
            request_count[entry.GetRequest()]++;
        }
    }

    std::vector<std::pair<std::string, int>> sorted_requests(request_count.begin(), request_count.end());

    std::sort(sorted_requests.begin(), sorted_requests.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << std::endl << "Top " << stats_count_ << " requests:\n";
    for (int i = 0; i < std::min(stats_count_, static_cast<int>(sorted_requests.size())); ++i) {
        std::cout << sorted_requests[i].first << " - " << sorted_requests[i].second << " times\n";
    }
}
