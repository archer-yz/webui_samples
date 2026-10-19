#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include "webui.hpp"
#include "../common/assets_root.hpp"

// ASSETS_ROOT is set by CMake to the folder holding index.html and chart.umd.js
// (copied next to the executable), so the app works with no internet access.
#ifndef ASSETS_ROOT
#define ASSETS_ROOT "assets"
#endif

static std::string build_json(std::mt19937 &rng)
{
    std::normal_distribution<double> noise(0.0, 1.0);
    std::uniform_real_distribution<double> uni(10.0, 100.0);
    std::ostringstream o;

    o << "{\"line\":{\"labels\":[";
    for (int i = 0; i < 50; ++i) o << (i ? "," : "") << "\"t" << i << "\"";
    o << "],\"series\":{\"sin\":[";
    for (int i = 0; i < 50; ++i) o << (i ? "," : "") << std::sin(i * 0.2) + 0.1 * noise(rng);
    o << "],\"cos\":[";
    for (int i = 0; i < 50; ++i) o << (i ? "," : "") << std::cos(i * 0.2) + 0.1 * noise(rng);
    o << "]}},\"bar\":{\"labels\":[\"A\",\"B\",\"C\",\"D\",\"E\"],\"values\":[";
    for (int i = 0; i < 5; ++i) o << (i ? "," : "") << uni(rng);
    o << "]},\"pie\":{\"labels\":[\"CPU\",\"GPU\",\"IO\"],\"values\":[";
    for (int i = 0; i < 3; ++i) o << (i ? "," : "") << uni(rng);
    o << "]},\"scatter\":[";
    for (int i = 0; i < 60; ++i)
    {
        double x = noise(rng);
        o << (i ? "," : "") << "{\"x\":" << x << ",\"y\":" << 0.8 * x + 0.5 * noise(rng) << "}";
    }
    o << "]}";
    return o.str();
}

int main()
{
    webui::window win;
    std::mt19937 rng{std::random_device{}()};

    // Serve local files (chart.umd.js) through WebUI's built-in server
    win.set_root_folder(resolve_assets_root("richchart_assets", ASSETS_ROOT));

    win.bind("refresh", [&](webui::window::event *e)
             { e->get_window().run("setData(" + build_json(rng) + ");"); });

    // win.show_browser("index.html", AnyBrowser);
    win.show_wv("index.html");
    webui::wait();
    return 0;
}
