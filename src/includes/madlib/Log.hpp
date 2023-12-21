#pragma once

#include "Printer.hpp"
#include "time.hpp"
#include "files.hpp"

using namespace std;

namespace madlib {

    #define LOG(...) logger.date().writeln(__VA_ARGS__)
    #define DBG(...) logger.date().writeln(__FILE_LINE__, " ", COLOR_DEBUG, __VA_ARGS__, COLOR_DEFAULT)

    class Log: public Printer {
    protected:
        string filename;

    public:
        Log(const string& f = "app.log"): Printer(), filename(f) {}

        Log& date() {
            write(__DATE_TIME__);
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        Log& writeln(const Args&... args) {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            write(message + "\n");
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        Log& write(const Args&... args) {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            print(message);
            
            return *this;
        }

        virtual void print(const string& message) override {
            file_put_contents(filename, message, true);
        }

    } logger;

}