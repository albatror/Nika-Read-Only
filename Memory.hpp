#pragma once

#include <string>
#include <vector>
#include <sstream>   // For stringstream
// #include <fstream>   // No longer needed as getPidByName and its file operations are removed
// #include <filesystem> // No longer needed as getPidByName and its directory iteration are removed
#include <stdexcept> // For std::runtime_error
// #include <sys/uio.h> // No longer needed for process_vm_readv/writev
// #include <unistd.h>  // No longer directly needed, memflow should abstract OS interactions
#include <memory>    // For std::unique_ptr
#include <mutex>     // For std::mutex
#include <thread>    // For std::this_thread::sleep_for
#include <chrono>    // For std::chrono
#include <iostream>  // For std::cout in bruteforceDtb (temporary, consider replacing with a proper logging mechanism)
#include <algorithm> // For std::max/min if used by GetFurtherDistance (though not strictly needed for current GetFurtherDistance)
// #include <sys/types.h> // Potentially no longer needed if pid_t is not used directly and memflow handles PIDs
#include <cstring> // For strlen, memset (used in dummy memflow read and findPattern)
#include <cstdint> // For explicitly sized integer types (uint8_t, uint16_t, uint32_t)


// Custom types for pattern scanning and Windows type alignment
using BYTE   = uint8_t;         // Explicitly 8-bit unsigned type
using PBYTE  = BYTE*;
using WORD   = uint16_t;        // Explicitly 16-bit unsigned type
using PWORD  = WORD*;
using DWORD  = uint32_t;        // Explicitly 32-bit unsigned type
// using PDWORD = DWORD*; // If needed in the future


// memflow specific constants (assuming these would be part of memflow's API)
constexpr uint64_t Address_INVALID = (uint64_t)-1; // Common definition for invalid address
constexpr uint64_t MAX_PHYADDR = 0xFFFFFFFFF;      // Adjusted max physical address

// Forward declarations for memflow types - replace with actual includes
// TODO: Replace all dummy memflow structures and methods with actual library includes and calls.
namespace memflow { // DUMMY MEMFLOW IMPLEMENTATION - REPLACE WITH ACTUAL LIBRARY
    // Basic CSliceMut definition (replace with actual memflow version)
    template<typename T>
    struct CSliceMut {
        T* data;
        size_t len;

        CSliceMut(T* d, size_t l) : data(d), len(l) {}
    };

    // Result type placeholder (replace with actual memflow version)
    template<typename T_Ok, typename T_Err = int>
    class Result {
        union {
            T_Ok ok_val;
            T_Err err_val;
        };
        bool is_ok_flag;
    public:
        Result(T_Ok val) : ok_val(val), is_ok_flag(true) {}
        Result() : is_ok_flag(false) {} // Default to error for simplicity in dummy
        static Result<T_Ok, T_Err> Ok(T_Ok val) { return Result(val); }
        static Result<T_Ok, T_Err> Err(T_Err err) {
            Result res;
            res.is_ok_flag = false;
            res.err_val = err;
            return res;
        }
        bool is_ok() const { return is_ok_flag; }
        T_Okunwrap() const { if (!is_ok_flag) throw std::runtime_error("Called unwrap on Err value"); return ok_val; }
        T_Err err() const { if (is_ok_flag) throw std::runtime_error("Called err on Ok value"); return err_val; }
    };


    template<typename T = void> // Forward declaration
    class IntoProcessInstance;
    template<typename T = void> // Forward declaration
    class ConnectorInstance;
    template<typename T = void> // Forward declaration
    class OsInstance;

    // Dummy ProcessInfo structure (replace with actual memflow version)
    struct ProcessInfo {
        uint64_t address;
        uint64_t dtb = Address_INVALID;
        std::string name;
        uint64_t base = 0x400000;
        // Add other members as per actual memflow::ProcessInfo
    };

    // Dummy ModuleInfo structure (replace with actual memflow version)
    struct ModuleInfo {
        uint64_t base;
        uint64_t size;
        std::string name;
        // ... other fields
    };

    // Dummy ConnectorInventory (replace with actual memflow version)
    class ConnectorInventory {
    public:
        static ConnectorInventory& instance() {
            static ConnectorInventory inst;
            return inst;
        }
        std::unique_ptr<ConnectorInstance<>> create_connector(const std::string& name, const std::string& args = "") {
            if (name == "kvm") {
                return std::make_unique<ConnectorInstance<>>();
            }
            return nullptr;
        }
    };

    // Dummy OsChain (replace with actual memflow version)
    class OsChain {
    public:
        static std::unique_ptr<OsInstance<>> new_n(std::unique_ptr<ConnectorInstance<>> conn, const std::string& os_name) {
            if (conn && os_name == "linux") {
                return std::make_unique<OsInstance<>>();
            }
            return nullptr;
        }
    };

    // Dummy IntoProcessInstance methods (replace with actual memflow version)
    template<typename T>
    class IntoProcessInstance {
    public:
        bool is_valid_instance = false; // internal state for dummy
        IntoProcessInstance() : is_valid_instance(false) {} // Default constructor
        IntoProcessInstance(bool valid) : is_valid_instance(valid) {} // Constructor to set validity

        bool is_valid() const { return is_valid_instance; }

        // Dummy base_info_opt or similar method to get base address
        // This is highly dependent on actual memflow API
        uint64_t base() const { return is_valid_instance ? 0x400000 : 0; } // Simplified base address

        Result<void> set_dtb(uint64_t dtb_val, uint64_t /* Address_INVALID_val */) {
            if (is_valid_instance) {
                // In a real scenario, this would interact with the process memory object
                // For dummy, just assume it works if instance is "valid"
                // std::cout << "Dummy: Setting DTB to " << std::hex << dtb_val << std::endl;
                return Result<void>::Ok({});
            }
            return Result<void>::Err(-1); // Indicate error
        }

        Result<void> read_raw_into(uint64_t address, CSliceMut<uint8_t> buffer) {
            if (is_valid_instance && address != 0 && buffer.data != nullptr && buffer.len > 0) {
                // std::cout << "Dummy: Reading " << buffer.len << " bytes from 0x" << std::hex << address << std::endl;
                // Simulate a read by zeroing the buffer or filling with a pattern
                // For check_proc, it just needs to succeed or fail.
                if (address < MAX_PHYADDR) { // Simplistic check
                    memset(buffer.data, 0, buffer.len); // Simulate successful read
                    return Result<void>::Ok({});
                }
            }
            return Result<void>::Err(-1); // Indicate error
        }

        Result<void> write_raw_into(uint64_t address, CSliceMut<const uint8_t> buffer) { // Note: CSliceMut<const uint8_t> for write
             if (is_valid_instance && address != 0 && buffer.data != nullptr && buffer.len > 0) {
                // std::cout << "Dummy: Writing " << buffer.len << " bytes to 0x" << std::hex << address << std::endl;
                 if (address < MAX_PHYADDR) {
                    return Result<void>::Ok({});
                }
            }
            return Result<void>::Err(-1);
        }

        // Dummy for module_by_name
        Result<ModuleInfo> module_by_name(const std::string& /*module_name*/) {
            if (is_valid_instance) {
                // Return a dummy module
                return Result<ModuleInfo>::Ok({base(), 0x100000, "dummy_module.dll"});
            }
            return Result<ModuleInfo>::Err(-1);
        }
    };

    // Dummy ConnectorInstance (replace with actual memflow version)
    template<typename T>
    class ConnectorInstance {
    public:
    };

    // Dummy OsInstance (replace with actual memflow version)
    template<typename T>
    class OsInstance {
    public:
        std::vector<ProcessInfo> process_info_list_by_name(const std::string& name) {
            if (name == "target_process.exe") { // Example target process name
                 // Return a dummy process with a plausible DTB for testing bruteforce
                return { {0x1000, 0x12345000 /*dummy dtb*/, name, 0x400000} };
            }
            return {};
        }

        // Make into_process_by_info return a configurable instance
        IntoProcessInstance<> into_process_by_info(ProcessInfo /*info*/) {
            // For dummy purposes, assume it succeeds and returns a "valid" instance
            return IntoProcessInstance<>(true);
        }

        // For clone().into_process_by_info style
        OsInstance<T>* get() { return this; } // Simplistic get for unique_ptr style access
        OsInstance<T>& clone() { return *this; } // Dummy clone

        ProcessInfo process_info_by_address(uint64_t addr) {
            return {addr, 0x12345000, "dummy_process_at_addr", addr};
        }
    };
} // namespace memflow

enum class process_status { NOT_FOUND, FOUND_NO_ACCESS, FOUND_READY };

struct Process {
    memflow::IntoProcessInstance<> hProcess;
    uint64_t baseaddr = 0;
};

class Memory {
public:
    Memory() : status(process_status::NOT_FOUND) {
        // m_pid removed
    }

    ~Memory() {
        close_proc();
    }

    bool open_proc(const char* name) {
        std::lock_guard<std::mutex> lock(m);
        if (!conn) {
            conn = memflow::ConnectorInventory::instance().create_connector("kvm");
        }

        if (!kernel && conn) {
            kernel = memflow::OsChain::new_n(std::move(conn), "linux");
        }

        if (!kernel) {
            status = process_status::NOT_FOUND;
            return false;
        }

        // kernel.get()->process_info_by_name(name, &info) is not standard C++ vector style
        // Assuming process_info_list_by_name is preferred as used before
        auto process_list = kernel->process_info_list_by_name(name);
        if (process_list.empty()) {
            status = process_status::NOT_FOUND;
            return false;
        }
        memflow::ProcessInfo info = process_list[0]; // Take the first one

        // Attempt to use lastCorrectDtbPhysicalAddress if available
        if (this->lastCorrectDtbPhysicalAddress != 0 && this->lastCorrectDtbPhysicalAddress != Address_INVALID) {
            // Create a process instance to test the last known DTB
            memflow::IntoProcessInstance<> temp_hProcess = kernel->get()->clone().into_process_by_info(info);
            if (temp_hProcess.is_valid()) {
                // Temporarily assign to member proc for testDtbValue and check_proc to work
                Process prev_proc_state = proc; // Save current proc state
                process_status prev_status_state = status;

                proc.hProcess = temp_hProcess;
                // proc.baseaddr is not strictly needed for testDtbValue, but check_proc uses it.
                // We can assign a temporary base from info, or rely on check_proc to be robust.
                // For this test, we are primarily interested in DTB validity affecting reads.
                // A successful testDtbValue will set lastCorrectDtbPhysicalAddress and FOUND_READY.
                // check_proc itself will try to read from proc.baseaddr if set.
                // Let's ensure baseaddr is set for check_proc to have a target.
                proc.baseaddr = info.base; // Use base from ProcessInfo for the check

                if (testDtbValue(this->lastCorrectDtbPhysicalAddress)) {
                    // testDtbValue calls check_proc, which sets status.
                    // If it's FOUND_READY, it means lastCorrectDtbPhysicalAddress worked.
                    // proc.baseaddr would have been set by the initial info.base for the check.
                    // Now, ensure proc.baseaddr is from the potentially more accurate hProcess.base()
                    proc.baseaddr = proc.hProcess.base(); // Re-confirm base address from the active hProcess
                     if (proc.baseaddr == 0) { // If base couldn't be determined from hProcess
                        auto main_module_res = proc.hProcess.module_by_name(name);
                        if (main_module_res.is_ok()) proc.baseaddr = main_module_res.unwrap().base;
                    }

                    if (proc.baseaddr != 0) {
                        std::cout << "Successfully re-opened process with last known DTB: " << std::hex << this->lastCorrectDtbPhysicalAddress << std::endl;
                        // status is already FOUND_READY due to testDtbValue -> check_proc
                        return true; // Early exit
                    }
                }
                // If testDtbValue failed or baseaddr is 0, restore previous proc state and fall through
                proc = prev_proc_state;
                status = prev_status_state;
            }
            // If temp_hProcess wasn't valid or testDtbValue failed, reset proc for clean full discovery
            // This is implicitly handled by restoring prev_proc_state if it was invalid, or falling through.
            // Ensure proc is clean if we proceed
            proc.hProcess = memflow::IntoProcessInstance<>(false);
            proc.baseaddr = 0;
            // status will be reset by subsequent operations or remain as it was if restoring.
        }

        // Full discovery path / If lastCorrectDtbPhysicalAddress didn't work or wasn't set
        proc.hProcess = kernel->get()->clone().into_process_by_info(info);
        if (!proc.hProcess.is_valid()) {
            status = process_status::FOUND_NO_ACCESS;
            return false;
        }

        // proc.baseaddr = info.base; // Assuming info from process_info_list_by_name contains the base
        // Or, more robustly, get it from the process instance itself, e.g. after module loading
        // For this example, we'll use the dummy base from hProcess.
        proc.baseaddr = proc.hProcess.base();
        if (proc.baseaddr == 0) { // If base address couldn't be determined
             // Fallback: try to get it from module_by_name, e.g. the main executable
            auto main_module_res = proc.hProcess.module_by_name(name); // name might need to be full like "r5apex.exe"
            if (main_module_res.is_ok()) {
                proc.baseaddr = main_module_res.unwrap().base;
            } else {
                status = process_status::FOUND_NO_ACCESS; // Cannot determine base address
                return false;
            }
        }

        // Now bruteforce DTB for the newly acquired process handle
        // The DTB from 'info' might be a good starting point or a hint.
        // uint64_t dtb_hint = (info.dtb != Address_INVALID) ? info.dtb : 0x100000; // Default start if no hint
        if (this->bruteforceDtb(0x0, 0x100000)) { // Changed arguments as per subtask
            std::cout << "DTB found and set after bruteforce." << std::endl;
            status = process_status::FOUND_READY; // Should be set by testDtbValue via check_proc
            return true;
        } else {
            std::cout << "Failed to find a working DTB." << std::endl;
            status = process_status::FOUND_NO_ACCESS; // Or some other error status
            close_proc(); // Clean up if DTB bruteforce fails
            return false;
        }
    }

    void close_proc() {
        std::lock_guard<std::mutex> lock(m);
        proc.hProcess = memflow::IntoProcessInstance<>(false); // Invalidate the process handle
        proc.baseaddr = 0;
        status = process_status::NOT_FOUND;
        this->lastCorrectDtbPhysicalAddress = 0; // Clear cached DTB address
        // conn and kernel unique_ptrs handle their own cleanup automatically.
    }

    bool testDtbValue(const uint64_t& dtb_val) {
        std::lock_guard<std::mutex> lock(m); // Ensure thread safety if called externally
        if (!proc.hProcess.is_valid()) return false;

        auto set_dtb_res = proc.hProcess.set_dtb(dtb_val, Address_INVALID);
        if (!set_dtb_res.is_ok()) {
            // std::cerr << "Failed to set DTB: " << std::hex << dtb_val << std::endl;
            return false;
        }

        if (check_proc()) { // check_proc updates status internally
            if (status == process_status::FOUND_READY) { // Double check status from check_proc
                lastCorrectDtbPhysicalAddress = dtb_val;
                // std::cout << "DTB " << std::hex << dtb_val << " confirmed." << std::endl;
                return true;
            }
        }
        return false;
    }

    // GetFurtherDistance helper method, as in memory.cpp
    static uint64_t GetFurtherDistance(uint64_t A, uint64_t Min, uint64_t Max) {
        uint64_t distanceToMin = (A > Min) ? (A - Min) : (Min - A); // abs(A - Min)
        uint64_t distanceToMax = (A > Max) ? (A - Max) : (Max - A); // abs(A - Max)
        return (distanceToMin > distanceToMax) ? distanceToMin : distanceToMax;
    }

    bool bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage) {
        std::lock_guard<std::mutex> lock(m);
        if (!proc.hProcess.is_valid() || proc.baseaddr == 0) {
            // std::cerr << "bruteforceDtb: Process not ready or base address is zero." << std::endl;
            return false;
        }

        // Initial checks from memory.cpp
        if (stepPage == 0) { // Prevent division by zero and infinite loop
            // std::cerr << "bruteforceDtb: stepPage cannot be zero." << std::endl;
            return false;
        }
        // If dtbStartPhysicalAddr is not page aligned for the main 0x1000 step, it might be an issue.
        // The original code returns false directly.
        if ((dtbStartPhysicalAddr & 0xFFF) != 0 && stepPage == 0x1000) {
            // std::cerr << "bruteforceDtb: dtbStartPhysicalAddr is not page aligned for 0x1000 step." << std::endl;
            return false;
        }
        if (dtbStartPhysicalAddr > MAX_PHYADDR) {
            // std::cerr << "bruteforceDtb: dtbStartPhysicalAddr is beyond MAX_PHYADDR." << std::endl;
            return false;
        }

        auto start_time = std::chrono::steady_clock::now();
        uint64_t originalStartAddr = dtbStartPhysicalAddr; // Save for recursive call if needed

        // Alignment logic based on lastCorrectDtbPhysicalAddress (from memory.cpp)
        if (this->lastCorrectDtbPhysicalAddress != 0 && this->lastCorrectDtbPhysicalAddress != Address_INVALID) {
            dtbStartPhysicalAddr -= dtbStartPhysicalAddr % stepPage;
            dtbStartPhysicalAddr += this->lastCorrectDtbPhysicalAddress % stepPage;

            // Ensure it stays within bounds after adjustment
            if (dtbStartPhysicalAddr >= MAX_PHYADDR) {
                 dtbStartPhysicalAddr = (MAX_PHYADDR & ~(stepPage -1)); // Align to page below MAX_PHYADDR
                 if (dtbStartPhysicalAddr >= stepPage) dtbStartPhysicalAddr -=stepPage; else dtbStartPhysicalAddr = 0; // Go one page lower
                 if (dtbStartPhysicalAddr > MAX_PHYADDR) dtbStartPhysicalAddr = 0; // Safety net
            }
        }

        std::cout << "Starting DTB bruteforce from: 0x" << std::hex << dtbStartPhysicalAddr
                  << " (aligned from original 0x" << originalStartAddr << ")"
                  << " with step: 0x" << stepPage << std::endl;

        uint64_t furtherDistance = GetFurtherDistance(dtbStartPhysicalAddr, 0ULL, MAX_PHYADDR);
        size_t maxStep = (stepPage > 0) ? (furtherDistance / stepPage) : 0;
        // The original doesn't add 1 if maxStep is 0 but furtherDistance > 0.
        // It means if dtbStartPhysicalAddr is e.g. 0x1000 and stepPage is 0x2000, maxStep would be 0.

        bool result = false;
        uint64_t guessDtbAddr;

        // Test dtbStartPhysicalAddr itself (step = 0)
        if (testDtbValue(dtbStartPhysicalAddr)) {
            result = true;
            // Log success and duration, then return true
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time);
            std::cout << "DTB found (initial/aligned guess): 0x" << std::hex << this->lastCorrectDtbPhysicalAddress << " in " << duration.count() << "ms" << std::endl;
            return true;
        }

        for (size_t current_step_count = 1; current_step_count <= maxStep; ++current_step_count) {
            uint64_t offset = current_step_count * stepPage;

            // Guess upwards
            if (dtbStartPhysicalAddr <= MAX_PHYADDR - offset) { // Check for overflow before adding
                guessDtbAddr = dtbStartPhysicalAddr + offset;
                // The check `guessDtbAddr < MAX_PHYADDR` is mostly redundant if the above is correct, but safe.
                if (guessDtbAddr < MAX_PHYADDR && testDtbValue(guessDtbAddr)) {
                    result = true;
                    break;
                }
            }

            // Guess downwards
            // Ensure dtbStartPhysicalAddr >= offset to prevent uint64_t underflow (which wraps around)
            if (dtbStartPhysicalAddr >= offset) {
                guessDtbAddr = dtbStartPhysicalAddr - offset;
                if (testDtbValue(guessDtbAddr)) {
                    result = true;
                    break;
                }
            }

            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count() > 30) {
                std::cout << "DTB bruteforce timeout (30s) for stepPage 0x" << std::hex << stepPage << std::endl;
                // Do not recursively call here on timeout for the current stepPage, let it finish or be handled by outer logic.
                // The recursive call is only if this entire run (all steps for this stepPage) fails.
                return false; // Timeout for this bruteforceDtb call
            }
        }

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time);
        if (result) {
            std::cout << "DTB found: 0x" << std::hex << this->lastCorrectDtbPhysicalAddress << " in " << duration.count() << "ms" << std::endl;
        } else {
            std::cout << "DTB bruteforce round finished without finding a value in " << duration.count() << "ms for stepPage 0x" << std::hex << stepPage << std::endl;
        }

        // If not found and current stepPage was not the default 0x1000, try again with 0x1000.
        // The original memory.cpp uses the *initial* (pre-alignment) dtbStartPhysicalAddr for this recursive call.
        // This implementation will use the potentially aligned dtbStartPhysicalAddr from this run,
        // or we could pass `originalStartAddr` if that's preferred. The prompt implies using current dtbStartPhysicalAddr.
        if (!result && stepPage != 0x1000) {
            std::cout << "Trying bruteforce with default stepPage 0x1000 from 0x" << std::hex << originalStartAddr << "..." << std::endl;
            return bruteforceDtb(originalStartAddr, 0x1000);
        }
        return result;
    }

    bool check_proc() {
        // No lock here, assuming it's called by public methods that hold the lock or it's safe.
        // If called externally, ensure locking. For internal calls from open_proc/testDtbValue, lock is already held.
        if (!proc.hProcess.is_valid() || proc.baseaddr == 0) {
            status = process_status::FOUND_NO_ACCESS; // Or NOT_FOUND if hProcess is invalid
            return false;
        }

        short c;
        // The Read<short> method takes the output variable by reference as its second argument.
        // The third argument is the "whatAreYouReading" string.
        if (this->Read<short>(proc.baseaddr, c, "Process Sanity Check (MZ Header)")) {
            if (c == 0x5A4D) { // 'MZ' header (DOS Magic Number)
                status = process_status::FOUND_READY;
                return true; // Process is accessible and has the expected MZ header.
            } else {
                // Read was successful, but the content is not what we expect for an MZ header.
                // This could mean it's a valid process, but not a standard executable,
                // or we're pointing at the wrong memory.
                status = process_status::FOUND_NO_ACCESS; // Or a new status like FOUND_BUT_INVALID_HEADER
                return false; // Indicate that the specific check failed.
            }
        } else {
            // Read itself failed.
            status = process_status::FOUND_NO_ACCESS;
            return false;
        }
    }

    process_status get_proc_status() const {
        return status;
    }

    uint64_t get_proc_baseaddr() const {
        return proc.baseaddr;
    }

    // Boolean-returning Read/Write methods
    template <typename T>
    bool Read(uint64_t address, T &outValue, const std::string& whatAreYouReading = "requested data") {
        // Optional: Log attempt to read 'whatAreYouReading' from address
        if (!this->_Read(address, &outValue, sizeof(T))) { // Calls private bool _Read(uint64_t, void*, size_t)
            // Optional: Log failure for 'whatAreYouReading'
            return false;
        }
        return true;
    }

    template <typename T>
    bool Write(uint64_t address, const T &value, const std::string& whatAreYouWriting = "data") {
        // Optional: Log attempt to write 'whatAreYouWriting' to address
        if (!this->_Write(address, (void*)&value, sizeof(T))) { // Calls private bool _Write(uint64_t, void*, size_t)
            // Optional: Log failure for 'whatAreYouWriting'
            return false;
        }
        return true;
    }

    template <typename T>
    bool ReadArray(uint64_t address, T outArray[], size_t arrayLen, const std::string& whatAreYouReading = "array data") {
        if (arrayLen == 0) return true; // Or false/throw, depending on desired behavior for zero length.
        // Optional: Log attempt to read 'whatAreYouReading'
        if (!this->_Read(address, outArray, sizeof(T) * arrayLen)) { // Calls private bool _Read(uint64_t, void*, size_t)
            // Optional: Log failure
            return false;
        }
        return true;
    }

    template <typename T>
    bool WriteArray(uint64_t address, const T valueArray[], size_t arrayLen, const std::string& whatAreYouWriting = "array data") {
        if (arrayLen == 0) return true; // Or false/throw.
        // Optional: Log attempt to write 'whatAreYouWriting'
        if (!this->_Write(address, (void*)valueArray, sizeof(T) * arrayLen)) { // Calls private bool _Write(uint64_t, void*, size_t)
            // Optional: Log failure
            return false;
        }
        return true;
    }

    // Throwing Read/Write methods (convenience wrappers around boolean methods)
    template <typename T>
    T ReadValue(uint64_t address, const std::string& whatAreYouReading = "requested value") { // Renamed to avoid overload conflict if types are similar
        T buffer;
        if (!Read(address, buffer, whatAreYouReading)) { // Calls bool Read<T>(address, outValue, ...)
            throw std::runtime_error("Failed to read memory for " + whatAreYouReading + " at address: " + convertPointerToHexString(address));
        }
        return buffer;
    }

    template <typename T>
    void WriteValue(uint64_t address, const T &value, const std::string& whatAreYouWriting = "value") { // Renamed & returns void
        if (!Write(address, value, whatAreYouWriting)) { // Calls bool Write<T>(address, value, ...)
            throw std::runtime_error("Failed to write " + std::to_string(sizeof(T)) + " bytes for " + whatAreYouWriting + " at address: " + convertPointerToHexString(address));
        }
    }

    // ReadInt: Boolean-returning version
    bool ReadInt(uint64_t address, int &outValue, const std::string& whatAreYouReading = "integer") {
        return this->Read<int>(address, outValue, whatAreYouReading); // Calls bool Read<T>
    }

    // ReadInt: Value-returning (throwing) version
    int ReadIntValue(uint64_t address, const std::string& whatAreYouReading = "integer") { // Renamed from ReadInt
        // This correctly calls the throwing ReadValue<T>
        return ReadValue<int>(address, whatAreYouReading);
    }

    // ReadString: Boolean-returning version
    bool ReadString(uint64_t address, int size, std::string &outString, const std::string& whatAreYouReading = "string") {
        if (size <= 0) {
            outString.clear(); // Ensure outString is in a defined state on failure due to invalid size.
            return false;
        }
        std::vector<char> buffer_vec(size);
        if (!this->_Read(address, buffer_vec.data(), size)) { // Calls private _Read
            outString.clear(); // Ensure outString is in a defined state on read failure.
            return false;
        }
        // Use strnlen to find the actual length of the string within the buffer,
        // in case of early null terminators or if the buffer isn't fully used by the string.
        size_t actual_len = strnlen(buffer_vec.data(), static_cast<size_t>(size));
        outString.assign(buffer_vec.data(), actual_len);
        return true;
    }

    // ReadString: Value-returning (throwing) version
    std::string ReadStringValue(uint64_t address, int size, const std::string& whatAreYouReading = "string") { // Renamed from ReadString
        std::string tempString;
        // Calls the new bool ReadString(...)
        if (!ReadString(address, size, tempString, whatAreYouReading)) {
            throw std::runtime_error("Failed to read string [" + whatAreYouReading + "] at address: " + convertPointerToHexString(address));
        }
        return tempString;
    }

    bool IsValidPointer(uint64_t pointer) {
        return pointer > 0x00010000 && pointer < MAX_PHYADDR; // Use MAX_PHYADDR
    }

    std::string convertPointerToHexString(uint64_t pointer) { // Changed uintptr_t to uint64_t
        std::stringstream stream;
        stream << "0x" << std::hex << pointer;
        return stream.str();
    }

private:
    bool _Read(uint64_t address, void* pBuff, size_t size) { // Renamed from Read
        if (size == 0 || !proc.hProcess.is_valid() || status != process_status::FOUND_READY) {
            return false;
        }
        memflow::CSliceMut<uint8_t> slice(static_cast<uint8_t*>(pBuff), size);
        auto result = proc.hProcess.read_raw_into(address, slice);
        if (!result.is_ok()) {
            // Optional: Log error from result.err()
            // std::cerr << "memflow read error: " << result.err() << std::endl;
            status = process_status::FOUND_NO_ACCESS; // Update status on read failure
            return false;
        }
        return true;
    }

    bool _Write(uint64_t address, void* pBuff, size_t size) { // Renamed from Write
         if (size == 0 || !proc.hProcess.is_valid() || status != process_status::FOUND_READY) {
            return false;
        }
        // For writing, data should be const.
        memflow::CSliceMut<const uint8_t> slice(static_cast<const uint8_t*>(pBuff), size);
        auto result = proc.hProcess.write_raw_into(address, slice); // Assuming write_raw_into takes CSliceMut<const uint8_t>
                                                                  // or just CSlice<uint8_t> if memflow API uses that.
                                                                  // The dummy is write_raw_into(CSliceMut<const uint8_t>)
        if (!result.is_ok()) {
            // Optional: Log error
            // std::cerr << "memflow write error: " << result.err() << std::endl;
            status = process_status::FOUND_NO_ACCESS; // Update status on write failure
            return false;
        }
        return true;
    }

    // m_pid is removed.
    // getPidByName is removed.

    // Helper functions for findPattern
    // getByte remains the same as its logic is independent of preprocessing
    static BYTE getByte(const char* pattern, int& index) {
        BYTE byte = 0;
        char c1 = pattern[index++];
        char c2 = pattern[index++];
        if (c1 >= '0' && c1 <= '9') byte = (c1 - '0') << 4;
        else if (c1 >= 'a' && c1 <= 'f') byte = (c1 - 'a' + 10) << 4;
        else if (c1 >= 'A' && c1 <= 'F') byte = (c1 - 'A' + 10) << 4;
        if (c2 >= '0' && c2 <= '9') byte |= (c2 - '0');
        else if (c2 >= 'a' && c2 <= 'f') byte |= (c2 - 'a' + 10);
        else if (c2 >= 'A' && c2 <= 'F') byte |= (c2 - 'A' + 10);
        return byte;
    }

    // Updated isMatch to use preprocessed pattern and mask
    static bool isMatch(const PBYTE addr, const std::vector<BYTE>& pattern_bytes, const std::vector<char>& pattern_mask) {
        for (size_t n = 0; n < pattern_bytes.size(); ++n) {
            if (pattern_mask[n] == '?') {
                continue; // Wildcard
            }
            if (addr[n] != pattern_bytes[n]) {
                return false; // Mismatch
            }
        }
        return true; // All non-wildcard bytes matched
    }

    Process proc;
    process_status status;
    std::unique_ptr<memflow::ConnectorInstance<>> conn;
    std::unique_ptr<memflow::OsInstance<>> kernel;
    uint64_t lastCorrectDtbPhysicalAddress = 0;
    std::mutex m;

public: // Adding new public methods here, separate from existing ones for clarity
    // Pattern Scanning
    static size_t findPattern(PBYTE rangeStart, size_t len, const char* pattern) { // Changed return type to size_t
        if (!rangeStart || len == 0 || !pattern) {
            return (size_t)-1; // Return -1 if inputs are invalid
        }

        std::vector<BYTE> patt_bytes;
        std::vector<char> patt_mask;
        int pattern_idx = 0;

        while(pattern[pattern_idx]) {
            if (pattern[pattern_idx] == '?') {
                patt_bytes.push_back(0x00); // Dummy byte for wildcard
                patt_mask.push_back('?');
                pattern_idx++;
                if (pattern[pattern_idx] == '?') { // Handle '??'
                    pattern_idx++;
                }
            } else if (pattern[pattern_idx] != ' ') {
                // Need to pass pattern_idx by reference to getByte
                // but getByte increments it. So, use a temporary index for getByte.
                int temp_pattern_idx = pattern_idx;
                patt_bytes.push_back(getByte(pattern, temp_pattern_idx));
                patt_mask.push_back('x');
                pattern_idx = temp_pattern_idx; // Update pattern_idx based on getByte's consumption
            } else {
                pattern_idx++; // Skip space
            }
        }

        size_t pattern_actual_len = patt_bytes.size();
        if (pattern_actual_len == 0) {
            return (size_t)-1; // No valid pattern bytes parsed
        }

        for (size_t i = 0; i <= len - pattern_actual_len; ++i) {
            if (isMatch(rangeStart + i, patt_bytes, patt_mask)) {
                return i; // Return the offset from rangeStart
            }
        }
        return (size_t)-1; // Pattern not found
    }

    // ScanPointer
    uint64_t ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level) {
        uint64_t current_address = ptr_address;
        try {
            for (int i = 0; i < level; ++i) {
                if (current_address == 0) return 0; // Null pointer encountered
                // Read the value at the current address (which is a pointer to the next address)
                current_address = this->ReadValue<uint64_t>(current_address, "pointer in chain"); // Corrected to call ReadValue
                if (current_address == 0) return 0; // Null pointer after read
                current_address += offsets[i];
            }
            return current_address;
        } catch (const std::runtime_error& e) {
            // std::cerr << "ScanPointer failed: " << e.what() << std::endl;
            // Optionally log the error. Return 0 to indicate failure.
            return 0;
        }
    }
};
