/* Copyright (c) 2018 Cameron Harper
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * */

#ifndef HTTP_TEST_H
#define HTTP_TEST_H

#include <stdbool.h>

struct http_test_result {

    /** string format IP address */
    char ip[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
    
    /** HTTP response code */
    long response_code;
    
    double lookup_time;             /**< seconds */
    double connect_time;            /**< seconds */
    double start_transfer_time;     /**< seconds */
    double total_time;              /**< seconds */
};

/** Perform one or more GETs and return response time information if 
 * test completes.
 * 
 * @param[in] url
 * @param[in] headers       array of HTML headers
 * @param[in] num_headers   number of entries in `headers`
 * @param[in] iterations    number of GETs to perform (>=1)
 * @param[out] out          response time information
 * 
 * @return true if test completes
 * 
 * */
bool http_test(const char *url, const char **headers, unsigned long num_headers, unsigned long iterations, struct http_test_result *out);

#endif
