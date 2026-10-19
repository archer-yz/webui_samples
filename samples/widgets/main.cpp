#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "webui.hpp"
#include "../common/assets_root.hpp"

// ASSETS_ROOT is set by CMake to the folder holding index.html.
#ifndef ASSETS_ROOT
#define ASSETS_ROOT "assets"
#endif

namespace fs = std::filesystem;

// C++20 u8string is std::u8string; convert to UTF-8 std::string / from UTF-8 std::string
static std::string to_str(const std::u8string &s) { return std::string(s.begin(), s.end()); }
static fs::path to_path(const std::string &s) { return fs::path(std::u8string(s.begin(), s.end())); }

static std::string json_escape(const std::string &s)
{
    std::string o;
    for (unsigned char c : s)
    {
        switch (c)
        {
        case '"': o += "\\\""; break;
        case '\\': o += "\\\\"; break;
        case '\n': o += "\\n"; break;
        case '\r': o += "\\r"; break;
        case '\t': o += "\\t"; break;
        default:
            if (c < 0x20) { char b[8]; snprintf(b, sizeof b, "\\u%04x", c); o += b; }
            else o += (char)c;
        }
    }
    return o;
}

static std::string error_json(const std::string &m) { return "{\"error\":\"" + json_escape(m) + "\"}"; }

// Returns {"path":..., "entries":[{"name":..., "dir":bool}]}, folders first
static std::string list_dir(const std::string &path)
{
    std::error_code ec;
    fs::path p = to_path(path);
    if (!fs::is_directory(p, ec))
        return error_json("Not a folder: " + path);

    std::vector<std::pair<std::string, bool>> items;
    for (fs::directory_iterator it(p, fs::directory_options::skip_permission_denied, ec), end; !ec && it != end; it.increment(ec))
    {
        std::error_code e2;
        items.emplace_back(to_str(it->path().filename().u8string()), it->is_directory(e2));
    }
    if (ec && items.empty())
        return error_json("Cannot read folder: " + ec.message());

    std::sort(items.begin(), items.end(), [](auto &a, auto &b)
              { return a.second != b.second ? a.second > b.second : a.first < b.first; });

    std::ostringstream o;
    o << "{\"path\":\"" << json_escape(to_str(fs::absolute(p, ec).generic_u8string())) << "\",\"entries\":[";
    for (size_t i = 0; i < items.size(); ++i)
        o << (i ? "," : "") << "{\"name\":\"" << json_escape(items[i].first) << "\",\"dir\":" << (items[i].second ? "true" : "false") << "}";
    o << "]}";
    return o.str();
}

// Reads up to 64 KB of a file for display
static std::string read_file(const std::string &path)
{
    constexpr size_t kMax = 64 * 1024;
    std::ifstream f(to_path(path), std::ios::binary);
    if (!f)
        return error_json("Cannot open: " + path);
    std::string buf(kMax, '\0');
    f.read(buf.data(), kMax);
    buf.resize((size_t)f.gcount());
    if (buf.find('\0') != std::string::npos)
        return error_json("Binary file, not shown");
    std::error_code ec;
    auto size = fs::file_size(to_path(path), ec);
    return "{\"text\":\"" + json_escape(buf) + "\",\"size\":" + std::to_string(size) +
           ",\"truncated\":" + (size > kMax ? "true" : "false") + "}";
}

static std::string home_dir()
{
    const char *h = std::getenv("USERPROFILE");
    if (!h) h = std::getenv("HOME");
    return h ? h : fs::current_path().string();
}

int main()
{
    webui::window win;
    win.set_root_folder(resolve_assets_root("widgets_assets", ASSETS_ROOT));

    win.bind("get_home", [](webui::window::event *e)
             { e->return_string(to_str(fs::path(home_dir()).generic_u8string())); });
    win.bind("list_dir", [](webui::window::event *e)
             { e->return_string(list_dir(e->get_string())); });
    win.bind("read_file", [](webui::window::event *e)
             { e->return_string(read_file(e->get_string())); });
    win.bind("submit_form", [](webui::window::event *e)
             { e->return_string("C++ received: " + e->get_string()); });
    win.bind("exit_app", [](webui::window::event *)
             { webui::exit(); });

    win.show_browser("index.html", AnyBrowser);
    webui::wait();
    return 0;
}
