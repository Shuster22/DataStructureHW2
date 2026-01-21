import os
import argparse
import subprocess
import glob
import sys
import time
import re

COMPILATION_FLAGS = ["-std=c++14", "-DNDEBUG", "-Wall"]
TIMEOUT = 10

# Timeout for large stress tests (tests 5-8)
# Calculated based on: ~100K ops/sec * 3x safety margin
# test5: ~1M ops -> ~30s, test6: ~2M ops -> ~60s, test7: ~5M ops -> ~150s, test8: ~10M ops -> ~300s
# Using 600s (10 min) as conservative upper bound for all stress tests
STRESS_TEST_TIMEOUT = 600  # 10 minutes for very large tests

# Memory leak threshold: if memory increases by more than this percentage, report a potential leak
MEMORY_LEAK_THRESHOLD_MB = 50  # 50 MB increase threshold


def get_process_memory_mb(pid):
    """Get process memory usage in MB using PowerShell (Windows built-in)."""
    try:
        # Use PowerShell to get process memory (WorkingSet in bytes)
        cmd = f"powershell -Command \"(Get-Process -Id {pid}).WorkingSet / 1MB\""
        result = subprocess.run(
            cmd,
            shell=True,
            capture_output=True,
            text=True,
            timeout=2
        )
        if result.returncode == 0:
            mem_str = result.stdout.strip()
            # Extract number (might have decimal)
            match = re.search(r'(\d+\.?\d*)', mem_str)
            if match:
                return float(match.group(1))
    except:
        pass
    return None


def check_memory_before_after(exe_file, input_file, result_file, timeout):
    """Run the process and check memory usage during execution (Valgrind-style)."""
    process = None
    try:
        with open(input_file, "r") as stdin, open(result_file, "w") as stdout:
            process = subprocess.Popen(
                [exe_file],
                stdin=stdin,
                stdout=stdout,
                stderr=subprocess.PIPE
            )
            
            # Monitor memory usage of the child process
            try:
                # Get initial memory usage
                mem_before = get_process_memory_mb(process.pid)
                if mem_before is None:
                    mem_before = 0
                mem_peak = mem_before  # Initialize peak to starting memory
                
                # Poll process and track peak memory
                start_time = time.time()
                while process.poll() is None:
                    # Check for timeout
                    if time.time() - start_time > timeout:
                        raise subprocess.TimeoutExpired(process.args, timeout)
                    
                    # Get current memory usage
                    current_mem = get_process_memory_mb(process.pid)
                    if current_mem is not None and current_mem > mem_peak:
                        mem_peak = current_mem
                    
                    time.sleep(0.1)  # Check every 100ms
                
                # Process has completed, check return code
                if process.returncode != 0:
                    return False, mem_before, mem_peak
                
                # Return peak memory (the process will have terminated by now)
                return True, mem_before, mem_peak
                
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
                return False, None, None
    except Exception as e:
        if process:
            try:
                process.kill()
                process.wait()
            except:
                pass
        raise e


def run_test(exe_file, test_id, tests_dir):
    input_file = os.path.join(tests_dir, f"test{test_id}.in")
    expected_output_file = os.path.join(tests_dir, f"test{test_id}.out")
    result_file = os.path.join(tests_dir, f"test{test_id}.res")

    if not os.path.isfile(input_file):
        print(f"Input file for test {test_id} not found")
        return False

    if not os.path.isfile(expected_output_file):
        print(f"Expected output file for test {test_id} not found")
        return False

    # Calculate timeout dynamically based on test size
    # For stress tests (5-8), use longer timeout
    # Formula: operations / 50000 * 3 (conservative: 50K ops/sec with 3x safety margin)
    # Capped at STRESS_TEST_TIMEOUT for very large tests
    if test_id >= 5:
        # Stress tests: use fixed timeout (already conservative)
        timeout = STRESS_TEST_TIMEOUT
    else:
        # Small tests: 10s is more than enough
        timeout = TIMEOUT

    # Run test with memory checking
    try:
        success, mem_before, mem_after = check_memory_before_after(
            exe_file, input_file, result_file, timeout
        )
        if not success:
            print(f"Test {test_id} Failed: Runtime error")
            return False
        
        # Report memory usage (Valgrind-style)
        if mem_before is not None and mem_after is not None:
            mem_increase = mem_after - mem_before
            if mem_increase > MEMORY_LEAK_THRESHOLD_MB:
                print(f"Test {test_id} Warning: Potential memory leak detected! "
                      f"Peak memory: {mem_after:.2f} MB (start: {mem_before:.2f} MB, "
                      f"increase: {mem_increase:.2f} MB)")
            elif test_id >= 5:  # Stress tests - show memory info
                print(f"Test {test_id} Memory OK: Peak {mem_after:.2f} MB "
                      f"(increase: {mem_increase:.2f} MB)")
    except subprocess.TimeoutExpired:
        print(f"Test {test_id} Failed: Timeout")
        return False
    except Exception as e:
        print(f"Test {test_id} Failed: Runtime error ({e})")
        return False

    with open(result_file, "r") as res, open(expected_output_file, "r") as expected:
        if res.read().strip() == expected.read().strip():
            print(f"Test {test_id} Passed")
            return True
        else:
            print(f"Test {test_id} Failed: Wrong output")
            return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--tests_dir", default="./tests/")
    parser.add_argument("--code_dir", default="./")
    parser.add_argument("--compiler_path", default="g++")
    parser.add_argument("--clean", action="store_true")
    parser.add_argument("--abort_on_fail", action="store_true")
    parser.add_argument("-t", "--tests", type=int, nargs="*", default=None)
    args = parser.parse_args()

    source_files = glob.glob(os.path.join(args.code_dir, "*.cpp"))
    if not source_files:
        print("No .cpp files found")
        return -1

    exe_file = os.path.abspath(os.path.join(args.code_dir, "main.exe"))

    compile_cmd = [args.compiler_path] + COMPILATION_FLAGS + ["-o", exe_file] + source_files

    try:
        subprocess.run(compile_cmd, check=True)
    except subprocess.CalledProcessError:
        print("Compilation failed")
        return -1

    if args.clean:
        for f in os.listdir(args.tests_dir):
            if f.endswith(".res"):
                os.remove(os.path.join(args.tests_dir, f))
        return 0

    if args.tests is None:
        tests = sorted(
            int(f.split("test")[1].split(".in")[0])
            for f in os.listdir(args.tests_dir)
            if f.endswith(".in")
        )
    else:
        tests = args.tests

    for test_id in tests:
        ok = run_test(exe_file, test_id, args.tests_dir)
        if not ok and args.abort_on_fail:
            return 0

    return 0


if __name__ == "__main__":
    main()
