#include <curl/curl.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmocka.h"

#include <string.h>
#include <stdlib.h>


CURLcode curl_global_init(long flags)
{
    function_called();
    return mock();
}

CURL *curl_easy_init(void)
{
    return mock_ptr_type(CURL *);    
}

CURLcode curl_easy_perform(CURL *curl)
{
    check_expected(curl);
    return mock();
}

void curl_easy_cleanup(CURL *curl)
{
    check_expected(curl);
}

#undef curl_easy_setopt
CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)
{
    return mock();
}

#undef curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
    va_list arg;
    void *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, void *);
    va_end(arg);

    switch(info){
    case CURLINFO_RESPONSE_CODE:
        *((long *)paramp) = mock();
        break;
    case CURLINFO_PRIMARY_IP:
        *((char **)paramp) = mock_ptr_type(char *);
        break;
    case CURLINFO_CONNECT_TIME:
    case CURLINFO_STARTTRANSFER_TIME:
    case CURLINFO_TOTAL_TIME:
    case CURLINFO_NAMELOOKUP_TIME:
        *((double *)paramp) = mock();
        break;
    default:
        abort();
        break;
    }

    return CURLE_OK;
}

void curl_global_cleanup(void)
{
    function_called();
}

struct curl_slist *curl_slist_append(struct curl_slist *self, const char *value)
{
    return mock_ptr_type(struct curl_slist *);
}

void curl_slist_free_all(struct curl_slist *self)
{
    check_expected(self);
}
