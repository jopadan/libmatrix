//
// Copyright (c) 2010-2012 Linaro Limited
//
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the MIT License which accompanies
// this distribution, and is available at
// http://www.opensource.org/licenses/mit-license.php
//
// Contributors:
//     Alexandros Frantzis <alexandros.frantzis@linaro.org>
//     Jesse Barker <jesse.barker@linaro.org>
//
#ifndef LOG_H_
#define LOG_H_

class Log
{
public:
    static void init(bool do_debug = false) { do_debug_ = do_debug; }
    // Emit an informational message
    static void info(const char *fmt, ...);
    // Emit a debugging message
    static void debug(const char *fmt, ...);
    // Emit an error message
    static void error(const char *fmt, ...);
    // Explicit flush of the log buffer
    static void flush();
    // A prefix constant that informs the logging infrastructure that the log
    // message is a continuation of a previous log message to be put on the
    // same line.
    static const std::string continuation_prefix;
    // A constant for identifying the log messages as originating from a
    // particular application.
    static std::string appname;
private:
    static bool do_debug_;
};

#endif /* LOG_H_ */
