#pragma once

#include "../../../../src/includes/madlib/madlib.hpp"
#include "../../../../src/includes/madlib/graph/graph.hpp"
#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/trading/trading.hpp"

using namespace std;
using namespace madlib;
using namespace madlib::graph;
using namespace madlib::trading;

GFX* chart_manual_test4_gfxPtr;
Chart* chart_manual_test4_chartPtr;
Button* chart_manual_test4_closeOkBtnPtr;

void chart_manual_test4_draw(void* /*context*/) {
    //chart_manual_test4_chartPtr->draw();
    chart_manual_test4_closeOkBtnPtr->draw();
}

void chart_manual_test4_close(void*, unsigned int, int, int) {
    chart_manual_test4_gfxPtr->close = true;
}

int chart_manual_test4_history()
{
    GFX gfx;
    chart_manual_test4_gfxPtr = &gfx;
    GUI gui(gfx, 800, 600, "chart_manual_test4_history");


    Button closeOkBtn(gfx, 15, 15, 100, 30, "Ok");
    chart_manual_test4_closeOkBtnPtr = &closeOkBtn;
    closeOkBtn.setBackgroundColor(green);
    closeOkBtn.setTextColor(white);
    closeOkBtn.addTouchHandler(chart_manual_test4_close);
    gui.child(closeOkBtn);


    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    // const double volumeMean = 50;  // Initial volume
    // const double volumeStdDeviation = 5;
    // const double priceMean = 100;  // Initial price
    // const double priceStdDeviation = 5;
    // const double timeLamda = MS_PER_MIN * 10;  // Mean time in milliseconds (60 seconds)
    const ms_t startTime = datetime_to_ms("2023-01-01"); // Current time as the start time
    const ms_t endTime = datetime_to_ms("2023-01-02"); // 300 seconds in the future
    const ms_t period = period_to_ms("1h");
    // const unsigned int seed = 3;

    // Create a MonteCarloTradeCandleHistory object with the specified parameters
    // MonteCarloTradeCandleHistory::Args context({
    //     symbol, 
    //     startTime, endTime, period,
    //     volumeMean, volumeStdDeviation,
    //     priceMean, priceStdDeviation,
    //     timeLamda, seed
    // });
    // MonteCarloTradeCandleHistory history(&context);
    // history.init();

    SharedFactory sharedFactory = SharedFactory();

    TradeCandleHistory* history = (TradeCandleHistory*)sharedFactory.create(
        "build/src/shared/trading/history/MonteCarloTradeCandleHistory", 
        "MonteCarloTradeCandleHistory",
        new TradeCandleHistory::Args({
            symbol, 
            startTime, endTime, period,
            // volumeMean, volumeStdDeviation,
            // priceMean, priceStdDeviation,
            // timeLamda, seed
        })
    );

    // Zoom zoom;
    CandleHistoryChart chart(
        gfx, 10, 10, 780, 580,
        *history
    );
    gui.child(chart);
    chart_manual_test4_chartPtr = &chart;
    chart.addDrawHandler(chart_manual_test4_draw);

    gui.loop();
    
    return 0;
}
