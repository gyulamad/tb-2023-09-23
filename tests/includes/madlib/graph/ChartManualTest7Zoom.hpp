#pragma once

#include "../../ManualTestApplication.hpp"

#include "../../../../src/includes/madlib/graph/Chart.hpp"
#include "../../../../src/includes/madlib/graph/SlideBar.hpp"
#include "../../../../src/includes/madlib/graph/ScrollBar.hpp"
#include "../../../../src/includes/madlib/graph/IntervalBar.hpp"

using namespace madlib::graph;
using namespace madlib::trading;

class ChartManualTest7Zoom: public ManualTestApplication {
protected:

    // Define parameters and desired time range
    const string symbol = "MONTECARLO";
    // const double volumeMean = 100;  // Initial volume
    // const double volumeStdDeviation = 10;
    // const double priceMean = 100;  // Initial price
    // const double priceStdDeviation = 10;
    // const double timeLambda = MS_PER_MIN;
    const ms_t startTime = datetime_to_ms("2021-01-01");
    const ms_t endTime = datetime_to_ms("2021-01-07");
    const ms_t period = period_to_ms("1h");
    const unsigned int seed = 6000;




    // test sliders...

    SlideBar slideBarHorizontal = SlideBar(gfx, 10, 60, 200, HORIZONTAL);
    ScrollBar scrollBarHorizontal = ScrollBar(gfx, 10, 90, 200, HORIZONTAL);
    IntervalBar intervalBarHorizontal = IntervalBar(gfx, 10, 120, 200, HORIZONTAL);

    SlideBar slideBarVertical = SlideBar(gfx, 260, 60, 200, VERTICAL);
    ScrollBar scrollBarVertical = ScrollBar(gfx, 290, 60, 200, VERTICAL);
    IntervalBar intervalBarVertical = IntervalBar(gfx, 320, 60, 200, VERTICAL);

    Factory<CandleHistory> candleHistoryFactory = Factory<CandleHistory>();
    CandleHistory* history = nullptr;
    CandleHistoryChart* chart = nullptr;

public:

    using ManualTestApplication::ManualTestApplication;

    virtual ~ChartManualTest7Zoom() {
        delete chart;
    }

    virtual void init() override {
        ManualTestApplication::init();
        gui.setTitle("ChartManualTest7Zoom");

        // Create a MonteCarloTradeCandleHistory object with the specified parameters
        history = candleHistoryFactory.createInstance(
            "build/src/shared/trading/history/MonteCarloTradeCandleHistory/"
            "MonteCarloTradeCandleHistory.so",
            // new TradeCandleHistory::Args({
            symbol, 
            startTime, endTime, period
                // volumeMean, volumeStdDeviation,
                // priceMean, priceStdDeviation,
                // timeLamda, seed
            // })
        );
        Progress progress;
        history->load(progress);
        progress.close();
        
        chart = new CandleHistoryChart(
            gfx, 300, 300, 1000, 300, history
        );

        mainFrame.child(slideBarHorizontal);
        mainFrame.child(scrollBarHorizontal);
        mainFrame.child(intervalBarHorizontal);
        mainFrame.child(slideBarVertical);
        mainFrame.child(scrollBarVertical);
        mainFrame.child(intervalBarVertical);

        mainFrame.child(*chart);
    }
};
