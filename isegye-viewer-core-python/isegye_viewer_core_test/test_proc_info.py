# test_proc_info_pytest.py

import os
import pytest
import time
from isegye_viewer_core_proc_info import BasicProcInfo, DiskIOCounters

@pytest.fixture(scope="module")
def proc_info_fixture():
    """
    Fixture to create a BasicProcInfo instance and retrieve the current process PID.
    This fixture is executed once per test module.
    """
    proc = BasicProcInfo()
    pid = os.getpid()
    return proc, pid

def test_get_all_processes(proc_info_fixture):
    """
    Test to ensure that all process IDs are retrieved successfully.
    """
    proc, _ = proc_info_fixture
    all_pids = proc.getAllProcesses()
    
    assert isinstance(all_pids, list), "Expected getAllProcesses to return a list."
    assert len(all_pids) > 0, "The list of process IDs should not be empty."

def test_get_process_name(proc_info_fixture):
    """
    Test to verify that the process name is retrieved correctly for a given PID.
    """
    proc, pid = proc_info_fixture
    process_name = proc.getProcessName(pid)
    
    assert isinstance(process_name, str), "Expected getProcessName to return a string."
    assert len(process_name) > 0, "Process name should not be empty."

def test_get_process_owner(proc_info_fixture):
    """
    Test to verify that the process owner is retrieved correctly for a given PID.
    """
    proc, pid = proc_info_fixture
    process_owner = proc.getProcessOwner(pid)
    
    assert isinstance(process_owner, str), "Expected getProcessOwner to return a string."
    assert len(process_owner) > 0, "Process owner should not be empty."

def test_get_virtual_mem_usage(proc_info_fixture):
    """
    Test to ensure that virtual memory usage is retrieved correctly for a given PID.
    """
    proc, pid = proc_info_fixture
    virtual_mem = proc.getVirtualMemUsage(pid)
    
    assert isinstance(virtual_mem, int), "Expected getVirtualMemUsage to return an integer."
    assert virtual_mem > 0, "Virtual memory usage should be greater than zero."

def test_get_current_cpu_usage(proc_info_fixture):
    """
    Test to verify that the current CPU usage is retrieved correctly for a given PID.
    A short delay is added to allow for CPU usage calculation.
    """
    proc, pid = proc_info_fixture
    time.sleep(1)  # Delay to allow CPU usage calculation
    cpu_usage = proc.getCurrentCpuUsage(pid)
    
    assert isinstance(cpu_usage, float), "Expected getCurrentCpuUsage to return a float."
    assert cpu_usage >= 0.0, "CPU usage should not be negative."

def test_get_disk_io(proc_info_fixture):
    """
    Test to ensure that disk I/O counters are retrieved correctly for a given PID.
    """
    proc, pid = proc_info_fixture
    disk_io = proc.getDiskIo(pid)
    
    assert isinstance(disk_io, DiskIOCounters), "Expected getDiskIo to return a DiskIOCounters instance."
    assert disk_io.read_operation_count >= 0, "ReadOperationCount should not be negative."
    assert disk_io.write_operation_count >= 0, "WriteOperationCount should not be negative."
    assert disk_io.other_operation_count >= 0, "OtherOperationCount should not be negative."
    assert disk_io.read_transfer_count >= 0, "ReadTransferCount should not be negative."
    assert disk_io.write_transfer_count >= 0, "WriteTransferCount should not be negative."
    assert disk_io.other_transfer_count >= 0, "OtherTransferCount should not be negative."

def test_get_process_modules(proc_info_fixture):
    """
    Test to verify that the list of loaded modules (DLLs) is retrieved correctly for a given PID.
    """
    proc, pid = proc_info_fixture
    modules = proc.getProcessModules(pid)
    
    assert isinstance(modules, list), "Expected getProcessModules to return a list."
    assert len(modules) > 0, "The list of process modules should not be empty."

def test_is_module_loaded_existing(proc_info_fixture):
    """
    Test to check if a known loaded module is correctly identified as loaded.
    """
    proc, pid = proc_info_fixture
    modules = proc.getProcessModules(pid)
    
    if modules:
        module_to_check = modules[0]
        is_loaded = proc.isModuleLoaded(pid, module_to_check)
        assert is_loaded, f"Module '{module_to_check}' should be loaded."
    else:
        pytest.skip("No modules found to test isModuleLoaded with an existing module.")

def test_is_module_loaded_nonexistent(proc_info_fixture):
    """
    Test to ensure that a nonexistent module is correctly identified as not loaded.
    """
    proc, pid = proc_info_fixture
    module_to_check = 'nonexistent_module.dll'
    is_loaded = proc.isModuleLoaded(pid, module_to_check)
    
    assert not is_loaded, f"Module '{module_to_check}' should not be loaded."

def test_restart_process_by_pid(proc_info_fixture):
    """
    Test to verify the functionality of restarting a process by PID.
    WARNING: This test is skipped to prevent terminating essential processes.
    """
    proc, pid = proc_info_fixture
    pytest.skip("Restarting a process is a disruptive operation and is skipped in automated tests.")

