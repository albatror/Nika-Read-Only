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
        if (lastCorrectDtbPhysicalAddress != 0 && lastCorrectDtbPhysicalAddress != Address_INVALID) {
            proc.hProcess = kernel->get()->clone().into_process_by_info(info);
            if (proc.hProcess.is_valid()) {
                 if (testDtbValue(lastCorrectDtbPhysicalAddress)) {
                    // Successfully used last known DTB
                    // proc.baseaddr = info.base; // Assuming ProcessInfo has 'base'
                    // For dummy, let's use the base from hProcess itself or a fixed one
                    proc.baseaddr = proc.hProcess.base();
                    if (proc.baseaddr != 0) {
                        std::cout << "Successfully opened process with last known DTB: " << std::hex << lastCorrectDtbPhysicalAddress << std::endl;
                        return true; // Early exit if successful
                    }
                 }
            }
        }

        // If lastCorrectDtbPhysicalAddress didn't work or wasn't set, proceed with normal opening
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
        uint64_t dtb_hint = (info.dtb != Address_INVALID) ? info.dtb : 0x100000; // Default start if no hint
        if (bruteforceDtb(dtb_hint, 0x1000)) { // Use a common page size as step
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
        // conn and kernel unique_ptrs handle their own cleanup.
        // lastCorrectDtbPhysicalAddress is preserved.
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

    bool bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage) {
        std::lock_guard<std::mutex> lock(m); // Ensure thread safety for the operation
        if (!proc.hProcess.is_valid() || proc.baseaddr == 0) return false;

        std::cout << "Starting DTB bruteforce from: 0x" << std::hex << dtbStartPhysicalAddr
                  << " with step: 0x" << stepPage << std::endl;
        auto start_time = std::chrono::steady_clock::now();

        uint64_t current_dtb = dtbStartPhysicalAddr;
        uint64_t prev_dtb = current_dtb; // For GetFurtherDistance

        // Test the initial starting address
        if (testDtbValue(current_dtb)) {
            std::cout << "Initial DTB 0x" << std::hex << current_dtb << " is correct." << std::endl;
            return true;
        }

        // Define GetFurtherDistance (can be a static private method or lambda)
        // For simplicity as lambda here:
        auto GetFurtherDistance = [](uint64_t val, uint64_t prev_val, uint64_t step) -> uint64_t {
            uint64_t diff = (val > prev_val) ? (val - prev_val) : (prev_val - val);
            if (diff < step * 10) return step;
            if (diff < step * 100) return step * 10;
            if (diff < step * 1000) return step * 100;
            return step * 1000;
        };

        // Search downwards
        current_dtb = dtbStartPhysicalAddr - stepPage;
        prev_dtb = dtbStartPhysicalAddr;
        while (current_dtb > 0 && current_dtb < MAX_PHYADDR) { // Check current_dtb > 0 to prevent underflow issues with large steps
            if (testDtbValue(current_dtb)) {
                 std::cout << "Found DTB (downwards): 0x" << std::hex << current_dtb << std::endl;
                return true;
            }
            uint64_t step = GetFurtherDistance(current_dtb, prev_dtb, stepPage);
            prev_dtb = current_dtb;
            if (current_dtb < step) break; // Prevent underflow
            current_dtb -= step;

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 30) { // Timeout
                std::cout << "DTB bruteforce timeout (30s)." << std::endl;
                return false;
            }
        }

        // Search upwards
        current_dtb = dtbStartPhysicalAddr + stepPage;
        prev_dtb = dtbStartPhysicalAddr;
         while (current_dtb < MAX_PHYADDR) {
            if (testDtbValue(current_dtb)) {
                std::cout << "Found DTB (upwards): 0x" << std::hex << current_dtb << std::endl;
                return true;
            }
            uint64_t step = GetFurtherDistance(current_dtb, prev_dtb, stepPage);
            prev_dtb = current_dtb;
            if (MAX_PHYADDR - step < current_dtb) break; // Prevent overflow
            current_dtb += step;

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() > 30) { // Timeout
                std::cout << "DTB bruteforce timeout (30s)." << std::endl;
                return false;
            }
        }

        std::cout << "DTB bruteforce finished without finding a value." << std::endl;
        return false;
    }


    bool check_proc() {
        // No lock here, assuming it's called by public methods that hold the lock or it's safe.
        // If called externally, ensure locking. For internal calls from open_proc/testDtbValue, lock is already held.
        if (!proc.hProcess.is_valid() || proc.baseaddr == 0) {
            status = process_status::FOUND_NO_ACCESS; // Or NOT_FOUND if hProcess is invalid
            return false;
        }

        short c;
        // memflow::CSliceMut<uint8_t> slice((uint8_t*)&c, sizeof(c));
        // auto read_res = proc.hProcess.read_raw_into(proc.baseaddr, slice);
        // Using the class's Read method, which now uses hProcess.read_raw_into
        if (this->Read(proc.baseaddr, &c, sizeof(c))) { // Read a short
            status = process_status::FOUND_READY;
            return true;
        } else {
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

    static bool isMatch(const PBYTE addr, const char* pattern) {
        int patternIndex = 0;
        int addrIndex = 0;
        while (pattern[patternIndex]) {
            if (pattern[patternIndex] == '?') {
                patternIndex++;
                if (pattern[patternIndex] == '?') patternIndex++; // Skip ??
                addrIndex++;
            } else if (pattern[patternIndex] != ' ') {
                if (addr[addrIndex] != getByte(pattern, patternIndex)) {
                    return false;
                }
                addrIndex++;
            } else {
                patternIndex++; // Skip space
            }
        }
        return true;
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

        size_t patternLen = strlen(pattern);
        if (patternLen == 0) return (size_t)-1; // Return -1 for empty pattern

        // Calculate a more accurate minimum length for the pattern's byte representation
        size_t minPatternByteLen = 0;
        for (size_t k = 0; pattern[k];) {
            if (pattern[k] == '?') {
                minPatternByteLen++;
                k++;
                if (pattern[k] == '?') k++; // Skip second '?' for '??'
            } else if (pattern[k] != ' ') {
                minPatternByteLen++;
                k += 2; // Skip two hex chars
            } else {
                k++; // Skip space
            }
        }
        if (minPatternByteLen == 0) return (size_t)-1; // No actual bytes in pattern


        for (size_t i = 0; i <= len - minPatternByteLen; ++i) { // Ensure we don't read past end of rangeStart
            if (isMatch(rangeStart + i, pattern)) {
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
