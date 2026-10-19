#include <iostream>
#include <string>
#include "webui.hpp"
#include "../common/assets_root.hpp"

// ASSETS_ROOT is set by CMake to the folder holding index.html.
#ifndef ASSETS_ROOT
#define ASSETS_ROOT "assets"
#endif

int main()
{
    webui::window win;
    win.set_root_folder(resolve_assets_root("webview2_assets", ASSETS_ROOT));
    win.set_size(760, 480);
    win.set_position(200, 150);

    win.bind("ping", [](webui::window::event *e)
             { e->return_string("C++ got: " + e->get_string()); });

    win.bind("resize", [](webui::window::event *e)
             { e->get_window().set_size(1000, 640); });

    win.bind("quit", [](webui::window::event *)
             { webui::exit(); });

    // show_wv() opens a native WebView2 window instead of a browser tab.
    // It fails if the WebView2 runtime is missing, so fall back to a browser.
    if (!win.show_wv("index.html"))
    {
        std::cerr << "WebView2 unavailable, falling back to a browser\n";
        win.show_browser("index.html", AnyBrowser);
    }

    webui::wait();
    return 0;
}
