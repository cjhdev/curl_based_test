curl_based_test
===============

This repository contains a library and application implemented 
according to the test-and-metrics-test requirements.

I've made the following assumptions in addition to the published requirements:

- Library component shall return IP address and return code for the 
  final GET in a sequence of one or more iterations

[![Build Status](https://travis-ci.org/cjhdev/curl_based_test.svg?branch=master)](https://travis-ci.org/cjhdev/curl_based_test)

## Building and Running the Application

You will need the following:

- libcurl with headers in the system include path
- getopt
- gcc
- make

Then from project root:

~~~
cd app/the_app && make
./app/the_app/bin/the_app
~~~

### Extra Switches

In addition to the mandatory switches, I've added:

#### `-V`

Print version string to stdout.

#### `-h`

Print usage to stdout.

## Building and Running the Tests

~~~
cd test && make build_and_run
~~~

Or just look at the most recent TravisCI run.

## Layout

The layout is intended to decouple a suite of applications from reusable
library components. 

~~~
$ tree -I vendor
.
├── app
│   └── the_app
│       ├── bin
│       ├── bla
│       ├── build
│       ├── makefile
│       └── the_app.c
├── include
│   └── http_test.h
├── LICENSE
├── readme.md
├── src
│   └── http_test.c
├── test
│   ├── bin
│   ├── build
│   ├── makefile
│   ├── mock_curl.c
│   └── tc_http_test.c
└── VERSION
~~~

Folders are as follows:

- `include` contains library headers
- `src` contains library source
- `app` contains applications, where each application is sorted
  into a subfolder
- `test` contains all test code
- `vendor` contains third party code suitable to copy in as-is

Top level files are as follows:

- readme.md
- VERSION (of the library)
    - Apps may have their own version numbers
- LICENSE
    - All public code should have clearly stated T&Cs
- .travis.yaml
    - Required for TravisCI

## Code Style

- C99 according to the CFLAGS `-c99 -pedantic`
- Only static or automatic memory allocation (I'm using VLAs)
- Local functions are declared static

## Test Strategy

There are a lot of ways to test this project. Since this is a demo, 
my requirement is minimum effort.

I chose to write some tests against the `http_test()` interface (the library
component) using the cmocka framework. The idea is to mock out the 
libcurl interfaces so that I can control which branches in the function
are executed.

The tests are implemented [here](test/tc_http_test.c), the mocks are implemented [here](test/mock_curl.c).
The tests can be built and run by the [makefile](test/makefile). For a laugh I get TravisCI to build and 
run the tests on every change made to the master branch.

My tests assert the following behaviour:

- `http_test()` shall always cleanup libcurl if it was previously initialised
- `http_test()` shall return an average of test samples if the return value is true
- `http_test()` shall return false if number of iterations is zero
- `http_test()` shall return false if libcurl rejects URL
- `http_test()` shall return false if libcurl rejects the headers (not sure if this ever happens)
- `http_test()` shall return false if libcurl header list allocation fails

libcurl is a nuisance to mock with cmocka since it uses many calls
to common get/set functions AND those functions accept variadic arguments. 

Many calls to the same function means that the standard cmocka mocking
feature becomes brittle since if I want to check a parameter or return
a value, that value has to be loaded in the order it will be required.

Variadic arguments are tricky since I can't use the cmocka argument queues. 
I also have to unpack the variadic argument in the mock which 
makes the mocks more complicated.

## License

curl_based_test has an MIT license
