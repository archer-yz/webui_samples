#pragma once
#include <string>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

// Returns "<exe dir>/<name>" when it exists (relocatable/installed layout),
// otherwise falls back to the compile-time path.
inline std::string resolve_assets_root(const char *name, const char *fallback)
{
#ifdef _WIN32
    wchar_t buf[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (n > 0 && n < MAX_PATH)
    {
        auto p = std::filesystem::path(buf).parent_path() / name;
        if (std::filesystem::is_directory(p))
            return p.string();
    }
#endif
    return fallback;
}
