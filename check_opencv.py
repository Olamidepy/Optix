import subprocess
import os

def run_cmake():
    cmake_path = "C:/Qt/Tools/CMake_64/bin/cmake.exe"
    cwd = r"c:\Users\ACER\Optix"
    build_dir = os.path.join(cwd, "build", "Desktop_MinGW_Manual_Debug")
    
    cmd = [cmake_path, "-S", cwd, "-B", build_dir]
    print(f"Running command: {' '.join(cmd)}")
    
    try:
        # Run process, redirecting stdin to DEVNULL to avoid any descriptor errors
        res = subprocess.run(
            cmd,
            cwd=cwd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            stdin=subprocess.DEVNULL
        )
        print("--- STDOUT ---")
        print(res.stdout)
        print("--- STDERR ---")
        print(res.stderr)
        print(f"Exit code: {res.returncode}")
    except Exception as e:
        print(f"Failed to run process: {e}")

if __name__ == "__main__":
    run_cmake()
