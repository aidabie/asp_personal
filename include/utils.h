/**
* The MIT License (MIT)
*
* Copyright © 2025 <The VU Amsterdam ASP teaching team>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the “Software”), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute,
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL <The VU Amsterdam ASP teaching team> BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <signal.h>

#include "v8_api_access.h"
#include <time.h>

extern int server_fd_global;
extern volatile sig_atomic_t server_running;

JSObject parse_url(const char *url_path);

JSResult v8_execute_script_file(V8Engine *engine, const char *filename);

void telemetry_init();

void telemetry_increment_request_count();

int telemetry_get_request_count();

void telemetry_increment_200_responses();

int telemetry_get_200_responses();

void telemetry_get_start_time(struct timespec *out);

void start_server(V8Engine *engine);

#endif //SERVER_UTILS_H
