#include "core/sentinel_scanner.h"
#include <algorithm>

SentinelScanner::SentinelScanner(std::string sentinel)
    : sentinel_(std::move(sentinel)) {}

SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    // 1. Combine pending text with the incoming chunk
    std::string combined = pending_ + std::string(chunk);
    std::size_t pos = combined.find(sentinel_);
    
    if (pos != std::string::npos) {
        // 2. Sentinel found: everything before it is safe. Clear pending.
        std::string safe_text = combined.substr(0, pos);
        pending_.clear();
        return {safe_text, true};
    } else {
        // 3. Sentinel not found: hold back at most (sentinel.size() - 1) characters
        std::size_t keep_len = std::min(combined.size(), sentinel_.size() - 1);
        std::size_t split_pos = combined.size() - keep_len;
        
        std::string safe_text = combined.substr(0, split_pos);
        pending_ = combined.substr(split_pos);
        
        return {safe_text, false};
    }
}

SentinelScanner::Out SentinelScanner::flush() {
    // Emit anything left in the buffer when the stream is completely done
    std::string safe_text = pending_;
    pending_.clear();
    return {safe_text, false};
}