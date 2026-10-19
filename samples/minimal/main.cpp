#include <iostream>
#include <string>
#include "webui.hpp"

int main()
{
    // Create a new WebUI window object
    webui::window win;

    // HTML content for the minimal application
    const std::string html = R"html(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <title>WebUI C++ Minimal Example</title>
            <script src="webui.js"></script>
            <style>
                body {
                    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
                    background: linear-gradient(135deg, #1e1e2e, #282a36);
                    color: #f8f8f2;
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    justify-content: center;
                    height: 100vh;
                    margin: 0;
                    user-select: none;
                }
                h1 {
                    margin-bottom: 8px;
                    color: #50fa7b;
                }
                p {
                    color: #8be9fd;
                    margin-top: 0;
                }
                .btn-container {
                    display: flex;
                    gap: 12px;
                    margin-top: 16px;
                }
                button {
                    background-color: #bd93f9;
                    color: #282a36;
                    border: none;
                    padding: 10px 24px;
                    font-size: 15px;
                    font-weight: bold;
                    border-radius: 8px;
                    cursor: pointer;
                    transition: transform 0.1s, background-color 0.2s;
                }
                button:hover {
                    background-color: #ff79c6;
                }
                button:active {
                    transform: scale(0.96);
                }
                #btn-exit {
                    background-color: #ff5555;
                    color: #f8f8f2;
                }
                #btn-exit:hover {
                    background-color: #ff6e6e;
                }
            </style>
        </head>
        <body>
            <h1>WebUI + C++</h1>
            <p>Minimal example using static linkage (<code>webui::static</code>)</p>
            <div class="btn-container">
                <button id="btn-count">Clicked 0 times</button>
                <button id="btn-exit">Exit</button>
            </div>
        </body>
        </html>
    )html";

    int click_count = 0;

    // Bind click event for the counter button
    win.bind("btn-count", [&click_count](webui::window::event *e)
             {
        click_count++;
        std::string script = "document.getElementById('btn-count').innerText = 'Clicked "
                           + std::to_string(click_count)
                           + (click_count == 1 ? " time" : " times") + "';";
        e->get_window().run(script); });

    // Bind click event for the exit button
    win.bind("btn-exit", [](webui::window::event * /*e*/)
             { webui::exit(); });

    // Show the window using embedded HTML
    win.show(html);

    // Wait until all opened windows are closed
    webui::wait();

    return 0;
}
