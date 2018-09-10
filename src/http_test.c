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
#include <string.h>
#include <curl/curl.h>
#include <assert.h>

bool http_test(const char *url, const char **headers, unsigned long num_headers, unsigned long iterations, struct http_test_result *out)
{
    bool retval = false;
    CURL *curl;
    struct curl_slist *list = NULL;
    unsigned long i;
 
    if(iterations > 0U){
     
        (void)memset(out, 0, sizeof(*out));
     
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
        curl = curl_easy_init();
        
        if(curl != NULL){
            
            for(i=0U; i < num_headers; i++){
                        
                list = curl_slist_append(list, headers[i]);
                
                if(list == NULL){
                    
                    break;
                }
            }
            
            if((num_headers == 0U) || (list != NULL)){
            
                if(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK){
                    
                    /* ensure libcurl doesn't write stuff to stdout */
                    (void)curl_easy_setopt(curl, CURLOPT_WRITEDATA, fopen("/dev/null", "w+"));
                    
                    /* don't reuse connections (so average of many samples makes sense) */
                    (void)curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
                    
                    /* disable dns caching (so average of many lookup samples makes sense) */
                    (void)curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 0L); 

                    if(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list) == CURLE_OK){

                        for(i=0U; i < iterations; i++){

                            if(curl_easy_perform(curl) == CURLE_OK){
                                
                                double lookup_time;
                                double connect_time;
                                double start_transfer_time;
                                double total_time;
                                
                                (void)curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &lookup_time); 
                                (void)curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_time); 
                                (void)curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &start_transfer_time); 
                                (void)curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time); 

                                out->lookup_time += lookup_time;
                                out->connect_time += connect_time;
                                out->start_transfer_time += start_transfer_time;
                                out->total_time += total_time;
                                
#if 0                               
                                printf("%f %f %f %f\n", lookup_time, connect_time, start_transfer_time, total_time);
#endif                                
                            }
                            else{
                                
                                break;
                            }
                        }
                        
                        if(i == iterations){

                            const char *ip;
                            
                            (void)curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip); 

                            /* see comment in struct stats */
                            assert(strlen(ip) < sizeof(out->ip));

                            (void)strncpy(out->ip, ip, sizeof(out->ip)-1U);

                            (void)curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &out->response_code); 
                            
                            out->lookup_time /= iterations;
                            out->connect_time /= iterations;
                            out->start_transfer_time /= iterations;
                            out->total_time /= iterations;
                        
                            retval = true;
                        }
                    }
                }
                
                if(list != NULL){
                    
                    curl_slist_free_all(list);                
                }
            }
            
            curl_easy_cleanup(curl);
        }
        
        curl_global_cleanup();
    }
        
    return retval;
}

