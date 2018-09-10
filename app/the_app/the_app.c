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

#include "http_test.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#ifndef VERSION
#   define VERSION "unknown"
#endif

static void print_help(void)
{
    printf("the_app %s\n", VERSION);
    printf("\n");
    printf("options:\n");
    printf("\n");
    printf("-n <integer>        specify number of iterations\n");
    printf("-H \"<header>\"       specify an additional header\n");
    printf("-V                  print the version\n");
    printf("-h                  print this message\n");
}

static void print_version(void)
{
    printf("%s\n", VERSION);
}

/* basic check for some elements of "Header-name: Header-value" pattern  */
static bool check_header(const char *arg)
{
    bool retval = false;
    char *before;
    char *after;
    const char pattern[] = ":";
    char buffer[strlen(arg)+1U];    /* VLA */
    
    (void)strcpy(buffer, arg);
    
    before = strtok(buffer, pattern);    
    after = strtok(NULL, pattern);    
    
    if((before != NULL) && (after != NULL) && (strtok(NULL, pattern) == NULL)){
    
        retval = true;
    }
    
    return retval;
}

int main(int argc, char **argv)
{
    int c;
    long iterations = 1;
    unsigned long num_headers = 0U;
    const char *headers[100U];
    const char *opts = "H:n:hV";
    struct http_test_result result;
    
    opterr = 0;
    
    while((c = getopt(argc, argv, opts)) != -1){
        
        switch(c){
        /* add HTML header */
        case 'H':
        
            if(!check_header(optarg)){
                
                fprintf(stderr, "-H argument must follow have pattern \"Header-name: Header-value\"\n");
                exit(EXIT_FAILURE);                
            }
            
            if(num_headers < (sizeof(headers)/sizeof(*headers))){
                
                headers[num_headers] = optarg;
                num_headers++;                
            }
            else{
                
                fprintf(stderr, "too many headers (limited to %lu)\n", sizeof(headers)/sizeof(*headers));
                exit(EXIT_FAILURE);                
            }            
            break;
            
        /* iterations */
        case 'n':
            if((sscanf(optarg, "%ld", &iterations) != 1U) || (iterations < 1)){
                
                fprintf(stderr, "-n argument must be an unsigned integer greater than zero\n");
                exit(EXIT_FAILURE);                
            }
            break;
        /* print help */
        case 'h':
            print_help();
            exit(EXIT_SUCCESS);
            break;
        /* print version */
        case 'V':
            print_version();
            exit(EXIT_SUCCESS);
            break;
            
        case '?':
            fprintf(stderr, "invalid switch '-%c'\n", optopt);
            exit(EXIT_FAILURE);                
            break;
        
        default:
            exit(EXIT_FAILURE);
            break;
        }        
    };
    
    if(!http_test("http://www.google.com/", headers, num_headers, (unsigned long)iterations, &result)){
        
        fprintf(stderr, "http_test() failed\n");
        exit(EXIT_FAILURE);
    }

    printf("SKTEST;%s;%lu;%f;%f;%f;%f\n", 
        result.ip, 
        result.response_code,
        result.lookup_time,
        result.connect_time,
        result.start_transfer_time,
        result.total_time
    );
    
    exit(EXIT_SUCCESS);
}
