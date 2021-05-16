#include "Resha.h"

std::string ExtractFileName(const char* path)
{
    std::string result = path;
    const size_t last_slash_idx = result.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        result.erase(0, last_slash_idx + 1);
    }
    // Remove extension if present.
    const size_t period_idx = result.rfind('.');
    if (std::string::npos != period_idx)
    {
        result.erase(period_idx);
    }
    return result;
}

bool EndsWith(const std::string_view str, const std::string_view suffix)
{
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}
