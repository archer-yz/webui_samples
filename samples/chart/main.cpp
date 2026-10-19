#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include "webui.hpp"

// The chart is drawn with a plain <canvas> (no external libraries, works offline).
// C++ generates the data and pushes it to JavaScript via window::run().
static const std::string html = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>WebUI C++ Chart Example</title>
    <script src="webui.js"></script>
    <style>
        body { font-family: "Segoe UI", Arial, sans-serif; background: #282a36; color: #f8f8f2;
               display: flex; flex-direction: column; align-items: center; margin: 0; padding: 20px; }
        h2 { color: #50fa7b; margin: 0 0 12px; }
        canvas { background: #1e1e2e; border-radius: 8px; }
        .row { margin-top: 12px; display: flex; gap: 12px; }
        button { background: #bd93f9; border: none; padding: 8px 20px; border-radius: 6px;
                 font-weight: bold; cursor: pointer; }
        button:hover { background: #ff79c6; }
    </style>
</head>
<body>
    <h2>C++ data &rarr; WebUI canvas chart</h2>
    <canvas id="chart" width="700" height="380"></canvas>
    <div class="row">
        <button onclick="gen_sine()">Sine wave</button>
        <button onclick="gen_random()">Random walk</button>
        <button onclick="gen_stream()">Advance stream</button>
        <button onclick="clear_stream()">Reset stream</button>
    </div>
    <script>
        // Called from C++ with a title and an array of numbers
        function drawChart(title, data) {
            const c = document.getElementById('chart'), g = c.getContext('2d');
            const W = c.width, H = c.height, m = 45;
            g.clearRect(0, 0, W, H);
            let min = Math.min(...data), max = Math.max(...data);
            if (min === max) { min -= 1; max += 1; }
            const x = i => m + i * (W - 2 * m) / Math.max(data.length - 1, 1);
            const y = v => H - m - (v - min) * (H - 2 * m) / (max - min);

            g.strokeStyle = '#44475a'; g.fillStyle = '#8be9fd'; g.font = '12px sans-serif';
            g.lineWidth = 1;
            for (let k = 0; k <= 5; k++) {
                const v = min + (max - min) * k / 5, yy = y(v);
                g.beginPath(); g.moveTo(m, yy); g.lineTo(W - m, yy); g.stroke();
                g.fillText(v.toFixed(2), 4, yy + 4);
            }
            g.fillStyle = '#f8f8f2'; g.font = '14px sans-serif'; g.fillText(title, m, 22);

            g.strokeStyle = '#50fa7b'; g.lineWidth = 2; g.beginPath();
            data.forEach((v, i) => i ? g.lineTo(x(i), y(v)) : g.moveTo(x(i), y(v)));
            g.stroke();
        }
    </script>
</body>
</html>
)html";

static void plot(webui::window &w, const std::string &title, const std::vector<double> &v)
{
    std::ostringstream os;
    os << "drawChart('" << title << "',[";
    for (size_t i = 0; i < v.size(); ++i)
        os << (i ? "," : "") << v[i];
    os << "]);";
    w.run(os.str());
}

int main()
{
    webui::window win;
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<double> noise(0.0, 1.0);
    double phase = 0.0;
    std::vector<double> stream;
    size_t stream_index = 0;

    win.bind("gen_sine", [&](webui::window::event *e)
             {
        std::vector<double> v;
        for (int i = 0; i < 100; ++i)
            v.push_back(std::sin(i * 0.1 + phase) + 0.1 * noise(rng));
        phase += 0.5;
        plot(e->get_window(), "Sine wave + noise", v); });

    win.bind("gen_random", [&](webui::window::event *e)
             {
        std::vector<double> v{0.0};
        for (int i = 1; i < 100; ++i)
            v.push_back(v.back() + noise(rng));
        plot(e->get_window(), "Random walk", v); });

    // Each click appends a batch of samples and keeps a sliding window of 100 points
    win.bind("gen_stream", [&](webui::window::event *e)
             {
        for (int i = 0; i < 20; ++i) {
            stream.push_back(std::sin(stream_index++ * 0.15) + 0.2 * noise(rng));
            if (stream.size() > 100) stream.erase(stream.begin());
        }
        plot(e->get_window(), "Stream (click to advance)", stream); });

    win.bind("clear_stream", [&](webui::window::event *e)
             {
        stream.clear();
        stream_index = 0;
        plot(e->get_window(), "Stream reset", {0.0, 0.0}); });

    win.show(html);
    webui::wait();
    return 0;
}
