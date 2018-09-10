#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmocka.h"

#include <curl/curl.h>

#include "http_test.h"

static void http_test_shall_init_and_teardown_when_successful(void **user)
{
    CURL *curl = (CURL *)(42);
    const char url[] = "dummy";
    char ip[] = "ip";
    struct http_test_result result;
    
    will_return_always(curl_easy_setopt, CURLE_OK);
    
    expect_function_call(curl_global_init);
    will_return(curl_global_init, CURLE_OK);
    
    will_return(curl_easy_init, curl);
    
    expect_value(curl_easy_perform, curl, curl);
    will_return(curl_easy_perform, CURLE_OK);
    
    /* brittle unfortunately... */
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_NAMELOOKUP_TIME
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_CONNECT_TIME
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_STARTTRANSFER_TIME
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_TOTAL_TIME
    will_return(curl_easy_getinfo, ip);     //CURLINFO_PRIMARY_IP
    will_return(curl_easy_getinfo, 200L);     //CURLINFO_RESPONSE_CODE
  
    expect_value(curl_easy_cleanup, curl, curl);
    
    expect_function_call(curl_global_cleanup);
    
    assert_true( http_test(url, NULL, 0U, 1U, &result) );

    assert_true( result.lookup_time == 0.0 );
    assert_true( result.connect_time == 0.0 );
    assert_true( result.start_transfer_time == 0.0 );
    assert_true( result.total_time == 0.0 );
    assert_string_equal(ip, result.ip);
    assert_int_equal(200L, result.response_code);
}

static void http_test_shall_init_and_teardown_when_unsuccessful(void **user)
{
    CURL *curl = (CURL *)(42);
    const char url[] = "dummy";
    struct http_test_result result;
    
    will_return_always(curl_easy_setopt, CURLE_OK);
    
    expect_function_call(curl_global_init);
    will_return(curl_global_init, CURLE_OK);
    
    will_return(curl_easy_init, curl);
    
    expect_value(curl_easy_perform, curl, curl);
    will_return(curl_easy_perform, CURLE_COULDNT_CONNECT);
    
    expect_value(curl_easy_cleanup, curl, curl);
    
    expect_function_call(curl_global_cleanup);
  
    assert_false( http_test(url, NULL, 0U, 1U, &result) );
}

static void http_test_shall_fail_for_zero_iterations(void **user)
{
    const char url[] = "dummy";
    struct http_test_result result;
    
    assert_false( http_test(url, NULL, 0U, 0U, &result) );
}

static void http_test_shall_return_an_average_of_samples_when_successful(void **user)
{
    CURL *curl = (CURL *)(42);
    const char url[] = "dummy";
    char ip[] = "ip";
    struct http_test_result result;
    
    will_return_always(curl_easy_setopt, CURLE_OK);
    
    expect_function_call(curl_global_init);
    will_return(curl_global_init, CURLE_OK);
    
    will_return(curl_easy_init, curl);
    
    expect_value(curl_easy_perform, curl, curl);
    will_return(curl_easy_perform, CURLE_OK);
    
    /* brittle unfortunately... */
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_NAMELOOKUP_TIME
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_CONNECT_TIME
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_STARTTRANSFER_TIME
    will_return(curl_easy_getinfo, 0.0);    //CURLINFO_TOTAL_TIME
 
    expect_value(curl_easy_perform, curl, curl);
    will_return(curl_easy_perform, CURLE_OK);
  
    will_return(curl_easy_getinfo, 2.0);    //CURLINFO_NAMELOOKUP_TIME
    will_return(curl_easy_getinfo, 4.0);    //CURLINFO_CONNECT_TIME
    will_return(curl_easy_getinfo, 6.0);    //CURLINFO_STARTTRANSFER_TIME
    will_return(curl_easy_getinfo, 8.0);    //CURLINFO_TOTAL_TIME
 
    will_return(curl_easy_getinfo, ip);     //CURLINFO_PRIMARY_IP
    will_return(curl_easy_getinfo, 200L);   //CURLINFO_RESPONSE_CODE
    
    expect_value(curl_easy_cleanup, curl, curl);
    
    expect_function_call(curl_global_cleanup);
  
    assert_true( http_test(url, NULL, 0U, 2U, &result) );

    assert_true( result.lookup_time == 1.0 );
    assert_true( result.connect_time == 2.0 );
    assert_true( result.start_transfer_time == 3.0 );
    assert_true( result.total_time == 4.0 );
    assert_string_equal(ip, result.ip);
    assert_int_equal(200L, result.response_code);
}

static void http_test_shall_fail_when_url_is_invalid(void **user)
{
    CURL *curl = (CURL *)(42);
    const char url[] = "dummy";
    struct http_test_result result;
    
    expect_function_call(curl_global_init);
    will_return(curl_global_init, CURLE_OK);
    
    will_return(curl_easy_init, curl);
    
    will_return(curl_easy_setopt, CURLE_URL_MALFORMAT);
    
    expect_value(curl_easy_cleanup, curl, curl);
    
    expect_function_call(curl_global_cleanup);
  
    assert_false( http_test(url, NULL, 0U, 1U, &result) );
}

/* not sure curl ever rejects headers */
static void http_test_shall_fail_when_headers_are_invalid(void **user)
{
    CURL *curl = (CURL *)(42);
    struct curl_slist *list = (struct curl_slist *)(42);
    const char url[] = "dummy";
    struct http_test_result result;
    const char *headers[] = {
        "one",
        "two"
    };
    
    expect_function_call(curl_global_init);
    will_return(curl_global_init, CURLE_OK);
    
    will_return(curl_easy_init, curl);
    
    will_return_count(curl_slist_append, list, sizeof(headers)/sizeof(*headers));
    
    /* CURLOPT_URL
     * CURLOPT_WRITEDATA
     * CURLOPT_FORBID_REUSE
     * CURLOPT_DNS_CACHE_TIMEOUT
     * 
     * */
    will_return_count(curl_easy_setopt, CURLE_OK, 4);
    
    will_return(curl_easy_setopt, CURLE_BAD_FUNCTION_ARGUMENT);
    
    expect_value(curl_slist_free_all, self, list);
    
    expect_value(curl_easy_cleanup, curl, curl);
    
    expect_function_call(curl_global_cleanup);
  
    assert_false( http_test(url, headers, sizeof(headers)/sizeof(*headers), 1U, &result) );
}

static void http_test_shall_fail_when_headers_list_allocation_fails(void **user)
{
    CURL *curl = (CURL *)(42);
    const char url[] = "dummy";
    struct http_test_result result;
    const char *headers[] = {
        "one",
        "two"
    };
    
    expect_function_call(curl_global_init);
    will_return(curl_global_init, CURLE_OK);
    
    will_return(curl_easy_init, curl);
    
    will_return(curl_slist_append, NULL);
        
    expect_value(curl_easy_cleanup, curl, curl);
    
    expect_function_call(curl_global_cleanup);
  
    assert_false( http_test(url, headers, sizeof(headers)/sizeof(*headers), 1U, &result) );
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(http_test_shall_init_and_teardown_when_successful),
        cmocka_unit_test(http_test_shall_init_and_teardown_when_unsuccessful),
        cmocka_unit_test(http_test_shall_fail_for_zero_iterations),
        cmocka_unit_test(http_test_shall_return_an_average_of_samples_when_successful),
        cmocka_unit_test(http_test_shall_fail_when_url_is_invalid),
        cmocka_unit_test(http_test_shall_fail_when_headers_are_invalid),
        cmocka_unit_test(http_test_shall_fail_when_headers_list_allocation_fails),        
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
