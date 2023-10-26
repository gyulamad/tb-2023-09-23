#pragma once

#include "../../ManualTestApplication.hpp"

class ChartManualTest6: public ManualTestApplication {
protected:

    static void draw(void* context) {
        ChartManualTest6* that =
            (ChartManualTest6*)((Area*)context)
                ->getRoot()->getGFX().getContext();

        that->chart.draw();
        that->closeBtn.draw();
    }

    Frame frame = Frame(gfx, 10, 10, 1580, 880, BUTTON_PUSHED, black);
    Chart chart = Chart(frame);

public:

    void init() override {
        ManualTestApplication::init();

        gui.child(frame);
        frame.onDrawHandlers.push_back(draw);

        // Define parameters and desired time range
        const string symbol = "MONTECARLO";
        const double volumeMean = 100;  // Initial volume
        const double volumeStdDeviation = 10;
        const double priceMean = 100;  // Initial price
        const double priceStdDeviation = 10;
        const double timeLambda = MS_PER_MIN;
        const ms_t startTime = datetime_to_ms("2021-01-01");
        const ms_t endTime = datetime_to_ms("2021-01-07");
        const ms_t period = period_to_ms("1h");
        const unsigned int seed = 6000;

        // Create a MonteCarloHistory object with the specified parameters
        MonteCarloHistory history(
            symbol, startTime, endTime, period,
            volumeMean, volumeStdDeviation,
            priceMean, priceStdDeviation,
            timeLambda, seed
        );
        history.init();

        TradeHistoryChartPlugin candlesPlugin;
        candlesPlugin.project(chart, &history);
    }
};