import os
import subprocess
import sys
import platform
def run_command(command):
    """Run a command and print its output in real-time."""
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    for line in process.stdout:
        print(line, end="")
    for line in process.stderr:
        print(line, end="")
    process.stdout.close()
    process.stderr.close()
    return_code = process.wait()
    return return_code

def configure():
    """Run the CMake configuration step."""
    print("Configuring project with CMake...")
    command = ""
    if(platform.system() == "Windows"): 
        command = (
            'cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON '
            '-DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++" '
            '-S . -B build -G "MinGW Makefiles"'
        )

    else:
        # linux 
        command = (
            'cmake '
            '-S . -B build '
        )
    
    if run_command(command) != 0:
        print("CMake configuration failed.")
        sys.exit(1)

def build():
    """Run the CMake build step."""
    print("Building project with CMake...")
    command = "cmake --build build"
    if run_command(command) != 0:
        print("Build failed.")
        sys.exit(1)

if __name__ == "__main__":
    # Clear previous cache and configure
    if os.path.exists("CMakeCache.txt"):
        os.remove("CMakeCache.txt")
    configure()
    build()
    print("Build complete.")
