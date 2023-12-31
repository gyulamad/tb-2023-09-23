#include "../../../../includes/madlib/trading/CandleHistory.hpp"

namespace madlib::trading::history {
    
    class BitstampCandleHistory: public CandleHistory {
    protected:

        enum BitstampCandlesCsvField { UNIX = 0, DATE, SYMBOL, OPEN, HIGH, LOW, CLOSE, VOLUME_BASE, VOLUME_QUOTED };
        static const string datPath;
        static const string datFileTpl;
        static const string csvPath;
        static const string csvFileTpl;
            
        static vector<Candle> bitstamp_read_candle_history_dat(const string& datFile) {
            return vector_load<Candle>(datFile);
        }

        static vector<Candle> bitstamp_parse_candle_history_csv(
            Progress& progress,
            const string& csvFile,
            const string& datFile, 
            bool readOutFileIfExists = false,
            bool throwIfOutFileExists = false,
            bool skipIfOutFileIsNewer = true
        ) {
            if (!file_exists(csvFile)) 
                throw ERROR("File not found: " + csvFile);
            if (file_exists(datFile)) {
                if (throwIfOutFileExists) throw ERROR("File already exists: " + datFile);
                if (skipIfOutFileIsNewer && file_get_mtime(csvFile) < file_get_mtime(datFile)) {
                    if (readOutFileIfExists) return bitstamp_read_candle_history_dat(datFile);
                    else return {};
                }
                if (readOutFileIfExists) return bitstamp_read_candle_history_dat(datFile);
            }
            LOG(
                string("Parsing Bitstamp candle history data:\n")
                    + "csv input file   : " + csvFile + "\n"
                    + "output data file : " + datFile
            );
            vector<string> csvData = str_split("\n", file_get_contents(csvFile));

            vector<Candle> candles;
            const size_t csvDataSize = csvData.size();
            const size_t headerLines = 2;
            
            string firstLine = str_trim(csvData.at(headerLines));
            vector<string> firstSplits = str_split(",", firstLine);
            string firstDate = str_trim(firstSplits.at(BitstampCandlesCsvField::DATE));
            string secondLine = str_trim(csvData.at(headerLines + 1));
            vector<string> secondSplits = str_split(",", secondLine);
            string secondDate = str_trim(secondSplits.at(BitstampCandlesCsvField::DATE));        
            ms_t period = datetime_to_ms(firstDate) - datetime_to_ms(secondDate);

            ms_t t = now();
            for (size_t i = csvDataSize - 1; i >= headerLines; i--) {
                string line = str_trim(csvData.at(i));
                if (line.empty()) continue;
                if (t < now() - 5 * second) {
                    t = now();
                    string msg = "Parse CSV: [" + line + "]";
                    if (!progress.update(msg)) throw ERROR("User canceled");
                    LOG(msg);
                }
                vector<string> fields = str_split(",", line);
                if (!is_numeric(str_trim(fields.at(BitstampCandlesCsvField::UNIX)))) break;            
                double open = parse<double>(str_trim(fields.at(BitstampCandlesCsvField::OPEN)));
                double close = parse<double>(str_trim(fields.at(BitstampCandlesCsvField::CLOSE)));
                double low = parse<double>(str_trim(fields.at(BitstampCandlesCsvField::LOW)));
                double high = parse<double>(str_trim(fields.at(BitstampCandlesCsvField::HIGH)));
                double volume = parse<double>(str_trim(fields.at(BitstampCandlesCsvField::VOLUME_BASE)));
                ms_t start = datetime_to_ms(str_trim(fields.at(BitstampCandlesCsvField::DATE)));
                ms_t end = start + period - 1;
                Candle candle(open, close, low, high, volume, start, end);
                candles.push_back(candle);
            }
            LOG("Writing: [" + datFile + "]");
            const string path = path_extract(datFile);
            if (!file_exists(path)) {
                file_create_path(path);
            }
            vector_save(datFile, candles);
            return candles;
        }

        static void bitstamp_download_candle_history_csv_all(
            Progress& progress,
            const string& symbol,
            int fromYear,
            int toYear,
            const string& period = "minute", 
            bool overwrite = true
        ) {
            const string filename = str_replace(
                "Bitstamp_{symbol}_{year}_{period}.csv",
                {
                    {"{symbol}", symbol},
                    {"{period}", period},
                }
            );
            const string path = __DIR__ + "/download/cryptodatadownload.com/bitstamp/";
            if (!file_exists(path)) {
                file_create_path(path);
            }
            for (int year = fromYear; year <= toYear; year++) {
                if(!progress.update(year, fromYear, toYear, false)) throw ERROR("User canceled");
                progress.update("Download data " + to_string(year) + "...");
                const string filenameYear = str_replace(filename, "{year}", to_string(year));
                const string filepath = path + filenameYear;
                if (!overwrite && file_exists(filepath)) {
                    progress.update("Download data " + to_string(year) + " (skip)");
                    LOG("File already exists: ", filepath, " (skiping...)");
                    continue;
                }
                const string url = "https://www.cryptodatadownload.com/cdd/" + filenameYear;
                const string command = str_replace(
                    "curl -o {filepath} {url}",
                    {
                        {"{filepath}", filepath},
                        {"{url}", url},
                    }
                );
                LOG("Execute: ", command);
                const string output = exec(command);
                LOG(output);
            }
        }

        static void bitstamp_parse_candle_history_csv_all(
            Progress& progress,
            const string& symbol,
            int fromYear,
            int toYear,
            const string& period = "minute"
        ) {
            const map<string, string> repl = {
                {"{symbol}", symbol},
                {"{period}", period},
            };
            string _datFileTpl = str_replace(datFileTpl, repl);
            string _csvFileTpl = str_replace(csvFileTpl, repl);            
            for (int year = fromYear; year <= toYear; year++) {
                if (!progress.update("Parse data " + to_string(year) + "...")) throw ERROR("User canceled");
                const string csvFile = str_replace(_csvFileTpl, "{year}", to_string(year));
                const string datFile = str_replace(_datFileTpl, "{year}", to_string(year));
                bitstamp_parse_candle_history_csv(progress, csvFile, datFile, false, false);
                progress.update(year, fromYear, toYear, false);
            }
        }

    public:

        using CandleHistory::CandleHistory;

        // virtual void init(void*) override {} 
        
        virtual ~BitstampCandleHistory() {};

        virtual void load(Progress& progress) override {
            if (period != MS_PER_MIN) throw ERROR("Period works only on minutes charts"); // TODO: aggregate to other periods
            int fromYear = parse<int>(ms_to_datetime(startTime).substr(0, 4));
            int toYear = parse<int>(ms_to_datetime(endTime).substr(0, 4));
            const map<string, string> repl = {
                {"{symbol}", symbol},
                {"{period}", "minute"},
            };
            string _datFileTpl = str_replace(datFileTpl, repl);
            candles.clear();
            for (int year = fromYear; year <= toYear; year++) {
                if (!progress.update("Loading data " + to_string(year) + "...")) throw ERROR("User canceled");
                const string datFile = str_replace(_datFileTpl, "{year}", to_string(year));
                if (!file_exists(datFile)) {
                    string _csvFileTpl = str_replace(csvFileTpl, repl);
                    const string csvFile = str_replace(_csvFileTpl, "{year}", to_string(year));
                    if (!file_exists(csvFile)) {
                        bitstamp_download_candle_history_csv_all(progress, symbol, fromYear, toYear, "minute");
                    }
                    bitstamp_parse_candle_history_csv(progress, csvFile, datFile);
                }
                vector<Candle> yearCandles = bitstamp_read_candle_history_dat(datFile);
                for (const Candle& candle: yearCandles) {
                    if (startTime <= candle.getStart() && endTime >= candle.getEnd())
                        candles.push_back(candle);
                }
                progress.update(year, fromYear, toYear, false);
            }
            progress.close();
        }

        // Note: see more at https://www.cryptodatadownload.com/data/bitstamp/
        virtual void reload(Progress& progress) override {
            if (period != MS_PER_MIN) throw ERROR("Period works only on minutes charts"); // TODO: aggregate to other periods
            
            const bool override = zenity_question("Override", "Do you want to override if data already exists?");
        
            int fromYear = parse<int>(ms_to_date(startTime).substr(0, 4));
            int toYear = parse<int>(ms_to_date(endTime).substr(0, 4));
            bitstamp_download_candle_history_csv_all(progress, symbol, fromYear, toYear, "minute", override);
            bitstamp_parse_candle_history_csv_all(progress, symbol, fromYear, toYear, "minute");
            progress.close();
        }

    };
    const string BitstampCandleHistory::datPath = __DIR__ + "/data/";
    const string BitstampCandleHistory::datFileTpl = datPath + "{symbol}_{year}_{period}.dat";
    const string BitstampCandleHistory::csvPath = __DIR__ + "/download/cryptodatadownload.com/bitstamp/";
    const string BitstampCandleHistory::csvFileTpl = csvPath + "/Bitstamp_{symbol}_{year}_{period}.csv";
            
    extern "C" BitstampCandleHistory* createBitstampCandleHistory(
        const string& symbol,
        const ms_t startTime,
        const ms_t endTime,
        const ms_t period
    ) {
        return new BitstampCandleHistory(symbol, startTime, endTime, period);
    }
}