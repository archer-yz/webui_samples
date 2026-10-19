#include <atomic>
#include <chrono>
#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "webui.hpp"
#include "../common/assets_root.hpp"

// ASSETS_ROOT is set by CMake to the folder holding index.html and plotly.min.js
// (copied next to the executable), so the app works with no internet access.
#ifndef ASSETS_ROOT
#define ASSETS_ROOT "assets"
#endif

static void arr(std::ostringstream &o, const std::vector<double> &v)
{
    o << '[';
    for (size_t i = 0; i < v.size(); ++i)
        o << (i ? "," : "") << v[i];
    o << ']';
}

static void grid(std::ostringstream &o, int n, double (*f)(double, double))
{
    o << '[';
    for (int r = 0; r < n; ++r)
    {
        o << (r ? "," : "") << '[';
        for (int c = 0; c < n; ++c)
            o << (c ? "," : "") << f((r - n / 2) * 0.25, (c - n / 2) * 0.25);
        o << ']';
    }
    o << ']';
}

static std::string build_json(std::mt19937 &rng)
{
    std::normal_distribution<double> noise(0.0, 1.0);
    std::vector<double> x, s, c, sx, sy;
    for (int i = 0; i < 100; ++i)
    {
        x.push_back(i * 0.1);
        s.push_back(std::sin(i * 0.1) + 0.1 * noise(rng));
        c.push_back(std::cos(i * 0.1) + 0.1 * noise(rng));
    }
    for (int i = 0; i < 60; ++i)
    {
        sx.push_back(noise(rng));
        sy.push_back(0.8 * sx.back() + 0.5 * noise(rng));
    }

    std::ostringstream o;
    o << "{\"x\":";   arr(o, x);
    o << ",\"sin\":"; arr(o, s);
    o << ",\"cos\":"; arr(o, c);
    o << ",\"sx\":";  arr(o, sx);
    o << ",\"sy\":";  arr(o, sy);
    o << ",\"z\":";
    grid(o, 30, [](double a, double b)
         { return std::sin(a) * std::cos(b) + std::exp(-(a * a + b * b) / 4); });
    o << ",\"surf\":";
    grid(o, 40, [](double a, double b)
         { double r = std::sqrt(a * a + b * b) + 1e-6; return std::sin(r * 2) / r; });
    o << '}';
    return o.str();
}

int main()
{
    webui::window win;
    std::mt19937 rng{std::random_device{}()};
    std::atomic<bool> live{false};
    std::thread worker;

    win.set_root_folder(resolve_assets_root("plotly_assets", ASSETS_ROOT));

    win.bind("refresh", [&](webui::window::event *e)
             { e->get_window().run("setData(" + build_json(rng) + ");"); });

    // JS -> C++ with an argument: a clicked scatter point arrives as "x,y"
    win.bind("point_clicked", [](webui::window::event *e)
             {
        std::string p{e->get_string()};
        e->get_window().run("setStatus('C++ received point: " + p + "');"); });

    // Streaming from a background thread, appended point by point
    win.bind("start_live", [&](webui::window::event *)
             {
        if (live) return;
        if (worker.joinable()) worker.join();
        live = true;
        worker = std::thread([&live, &win] {
            double t = 10.0;
            while (live) {
                win.run("appendLive(" + std::to_string(t) + "," + std::to_string(std::sin(t)) +
                      "," + std::to_string(std::cos(t)) + ");");
                t += 0.1;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }); });

    win.bind("stop_live", [&](webui::window::event *)
             { live = false; });

    // win.show_browser("index.html", AnyBrowser);
    win.show_wv("index.html");
    webui::wait();

    live = false;
    if (worker.joinable())
        worker.join();
    return 0;
}
