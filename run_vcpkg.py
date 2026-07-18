import subprocess
import sys

def run_vcpkg_cmd(args):
    vcpkg_path = r"C:\vcpkg\vcpkg.exe"
    cmd = [vcpkg_path] + args
    print(f"Running command: {' '.join(cmd)}")
    
    try:
        res = subprocess.run(
            cmd,
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
        print(f"Failed to run vcpkg: {e}")

if __name__ == "__main__":
    # Check currently installed packages
    run_vcpkg_cmd(["list"])
