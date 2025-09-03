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

#include <signal.h>

#include "v8_api_access.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/signum-generic.h>

#include "utils.h"

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  *************************************************************
*/
static void handle_sigint(int sig) {
    (void)sig;
    server_running = 0;
    if (server_fd_global != -1) {
        close(server_fd_global);
        server_fd_global = -1;
    }
    printf("\nServer shutting down...\n");
    exit(0);
}

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  *************************************************************
*/
static void install_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
}

/*
  *************************************************************
  *                                                           *
  *    █████╗ ███████╗██████╗                                 *
  *   ██╔══██╗██╔════╝██╔══██╗                                *
  *   ███████║███████╗██████╔╝                                *
  *   ██╔══██║╚════██║██╔═══╝                                 *
  *   ██║  ██║███████║██║                                     *
  *   ╚═╝  ╚═╝╚══════╝╚═╝                                     *
  *                                                           *
  * Init and boilerplate                                      *
  *************************************************************
*/
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script.js>\n", argv[0]);
        return 1;
    }
    V8Engine *engine = v8_initialize(argc, argv);
    if (!engine) {
        fprintf(stderr, "Failed to initialize V8\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        fprintf(stderr, "Could not open script file: %s\n", argv[1]);
        v8_cleanup(engine);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *script = (char *)malloc(len + 1);
    fread(script, 1, len, f);
    script[len] = '\0';
    fclose(f);
    JSResult res = v8_execute_script(engine, script);
    if (res.type == JS_STRING) {
        free(res.value.str_result);
    }

    free(script);
    install_signal_handlers();
    start_server(engine);
    v8_cleanup(engine);
    return 0;
}
