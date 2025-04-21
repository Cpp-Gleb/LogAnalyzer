#include <iostream>
#include <climits>
#include "LogAnalyzer.h"

const int kMinimumNumberArguments = 2;

bool IsError(const std::string& key, const std::string& value) {
    if (value.empty() || value.find_first_not_of("0123456789-") != std::string::npos) {
        std::cerr << "Error: Invalid value for " << key << ". Expected a valid number.\n";
        return kError;
    }
    return kSucces;
}

bool IsHelp(int argc, char* argv[]) {
    if (argc < kMinimumNumberArguments) {
        std::cerr << "Usage: AnalyzeLog [OPTIONS] logs_filename\n"
                  << "Options:\n"
                  << "  -p, --print           Print errors (5XX) to console.\n"
                  << "  -o, --output=FILE     Output errors (5XX) to the specified file.\n"
                  << "  -s, --stats=N         Display top N most frequent requests (default: 10).\n"
                  << "  -w, --window=N        Analyze requests within a time window of N seconds.\n"
                  << "  -f, --from=TIME       Set the start time (in seconds) for log analysis.\n"
                  << "  -e, --to=TIME         Set the end time (in seconds) for log analysis.\n";
        return kError;
    }
    return kSucces;
}

struct ParsedArguments {
    std::string output_file;
    bool is_print = false;
    int stats_count = 10;
    int window_time = 0;
    long from_time = 0;
    long to_time = LONG_MAX;
    std::string log_filename;
};

ParsedArguments ParseArguments(int argc, char* argv[]) {
    ParsedArguments args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-p" || arg == "--print") {
            args.is_print = true;
        } else if (arg == "-o" || arg.find("--output=") == 0) {
            if (arg == "-o") {
                if (++i >= argc) {
                    std::cerr << "Error: Expected output file after -o.\n";
                    throw;
                }
                args.output_file = argv[i];
            } else {
                args.output_file = arg.substr(9);
            }
        } else if (arg == "-s" || arg.find("--stats=") == 0) {
            std::string value = (arg == "-s") ? argv[++i] : arg.substr(8);
            if (IsError("--stats", value)) throw;
            args.stats_count = std::stol(value);
        } else if (arg == "-w" || arg.find("--window=") == 0) {
            std::string value = (arg == "-w") ? argv[++i] : arg.substr(9);
            if (IsError("--window", value)) throw;
            args.window_time = std::stoi(value);
        } else if (arg == "-f" || arg.find("--from=") == 0) {
            std::string value = (arg == "-f") ? argv[++i] : arg.substr(7);
            if (IsError("--from", value)) throw;
            args.from_time = std::stol(value);
        } else if (arg == "-e" || arg.find("--to=") == 0) {
            std::string value = (arg == "-e") ? argv[++i] : arg.substr(5);
            if (IsError("--to", value)) throw;
            args.to_time = std::stol(value);
        } else if (args.log_filename.empty()) {
            args.log_filename = arg;
        } else {
            std::cerr << "Invalid option: " << arg << "\n";
            throw;
        }
    }
    return args;
}

int main(int argc, char* argv[]) {

    if (IsHelp(argc, argv)) { return kError; }

    ParsedArguments args = ParseArguments(argc, argv);

    LogAnalyzer analyzer(args.log_filename, args.output_file, args.is_print, args.stats_count,
                         args.window_time, args.from_time, args.to_time);
    analyzer.ParseLogFile();

    if (args.is_print || !args.output_file.empty()) {
        analyzer.OutputErrors();
    }

    if (args.window_time > 0) {
        analyzer.AnalyzeRequests();
    }

    if (args.stats_count > 0) {
        analyzer.OutputTopRequests();
    }

    return kSucces;
}
