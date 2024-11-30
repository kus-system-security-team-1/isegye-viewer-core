# test_proc_info.py

import os
import sys
import time
from isegye_viewer_core_proc_info import BasicProcInfo, DiskIOCounters

def main():
    # Create an instance of BasicProcInfo
    proc_info = BasicProcInfo()

    # 1. Get all process IDs
    print("Retrieving all process IDs...")
    all_pids = proc_info.getAllProcesses()
    print(f"Total processes found: {len(all_pids)}\n")

    # Display first 10 PIDs for brevity
    print("First 10 PIDs:\n")
    for pid in all_pids[:10]:  # Changed to start from index 0
        print(f"PID: {pid}")
    print("\n")

    # 2. Select a target PID
    # For testing purposes, we'll use the current Python process PID
    target_pid = os.getpid()
    print(f"Selected PID for testing: {target_pid}\n")

    # 3. Get Process Name
    print("Fetching process name...")
    process_name = proc_info.getProcessName(target_pid)
    if process_name:
        print(f"Process Name: {process_name}")
    else:
        print("Failed to retrieve process name.")
    print("\n")

    # 4. Get Process Owner
    print("Fetching process owner...")
    process_owner = proc_info.getProcessOwner(target_pid)
    if process_owner:
        print(f"Process Owner: {process_owner}")
    else:
        print("Failed to retrieve process owner.")
    print("\n")

    # 5. Get Virtual Memory Usage
    print("Fetching virtual memory usage...")
    virtual_mem = proc_info.getVirtualMemUsage(target_pid)
    print(f"Virtual Memory Usage: {virtual_mem} bytes")
    print("\n")

    # 6. Get Current CPU Usage
    print("Fetching current CPU usage...")
    cpu_usage = proc_info.getCurrentCpuUsage(35044)
    print(f"CPU Usage: {cpu_usage:.5f}%")
    print("\n")

    # 7. Get Disk I/O
    print("Fetching disk I/O counters...")
    disk_io = proc_info.getDiskIo(target_pid)
    if isinstance(disk_io, DiskIOCounters):
        print(f"Read Operation Count: {disk_io.read_operation_count}")
        print(f"Write Operation Count: {disk_io.write_operation_count}")
        print(f"Other Operation Count: {disk_io.other_operation_count}")
        print(f"Read Transfer Count: {disk_io.read_transfer_count}")
        print(f"Write Transfer Count: {disk_io.write_transfer_count}")
        print(f"Other Transfer Count: {disk_io.other_transfer_count}")
    else:
        print("Failed to retrieve disk I/O counters.")
    print("\n")

    # 8. Get Process Modules (DLLs)
    print("Fetching loaded DLLs (modules) for the process...")
    modules = proc_info.getProcessModules(target_pid)
    if modules:
        print(f"Total modules loaded: {len(modules)}")
        # Display first 10 modules for brevity
        print("First 10 modules:")
        for mod in modules[:10]:
            print(mod)
    else:
        print("Failed to retrieve process modules.")
    print("\n")

    # 9. Check if a Specific DLL is Loaded
    # Example 1: Check if the first module is loaded (should always be True)
    if modules:
        module_to_check = modules[0]
        print(f"Checking if '{module_to_check}' is loaded in process {target_pid}...")
        is_loaded = proc_info.isModuleLoaded(target_pid, module_to_check)
        print(f"Is '{module_to_check}' loaded: {is_loaded}\n")

    # Example 2: Check if a non-existing DLL is loaded
    module_to_check = 'nonexistent_module.dll'
    print(f"Checking if '{module_to_check}' is loaded in process {target_pid}...")
    is_loaded = proc_info.isModuleLoaded(target_pid, module_to_check)
    print(f"Is '{module_to_check}' loaded: {is_loaded}\n")

if __name__ == "__main__":
    main()
