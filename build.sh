#!/bin/bash

echo -e "\033[34m[+] Build the Handsome web server Library\033[0m"
cmake -S . -B build && cmake --build build ; # build the library

