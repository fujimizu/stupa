use inc::Module::Install;
name 'Stupa-Evhttp-Client';
all_from 'lib/Stupa/Evhttp/Client.pm';

# requires '';

tests 't/*.t';
author_tests 'xt';

build_requires 'Test::More';
use_test_base;
auto_include;
WriteAll;
