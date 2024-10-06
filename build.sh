#!/bin/bash
cleanUp(){
# Check if the process is running
if pgrep -f webapp > /dev/null; then
    echo -e "\033[32m[+] The 'webapp' process is running.\033[0m"
    # Kill the process
    pkill -f webapp
    echo -e "\033[31m[-] Killed the 'webapp' process.\033[0m"
else
    echo -e "\033[33m[!] The 'webapp' process is not running.\033[0m"
fi
}
cleanUp 

echo -e "\033[34m[+] Build the Library\033[0m"
cd "./build" && cmake .. && cmake --build . ; # build the library
echo "[+] Build The test Project ";

echo -e "\033[34m[+] Build The test Project\033[0m"
cd "../" &&  cd "./test/build" && cmake .. && cmake --build . ; # build the test Project 

echo -e "\033[34m[+] Run The Server\033[0m"

cd "../"  && ./build/webapp
