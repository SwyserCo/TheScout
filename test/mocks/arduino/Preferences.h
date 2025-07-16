#pragma once
#include <string>
#include <map>

class Preferences {
public:
    bool begin(const char* name, bool readOnly = false) {
        return true;
    }

    void end() {}

    bool clear() {
        storage.clear();
        return true;
    }

    size_t putString(const char* key, const char* value) {
        storage[key] = value;
        return strlen(value);
    }

    std::string getString(const char* key, const char* defaultValue = "") {
        auto it = storage.find(key);
        if (it != storage.end()) {
            return it->second;
        }
        return defaultValue;
    }

    size_t putUInt(const char* key, uint32_t value) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%u", value);
        storage[key] = buf;
        return strlen(buf);
    }

    uint32_t getUInt(const char* key, uint32_t defaultValue = 0) {
        auto it = storage.find(key);
        if (it != storage.end()) {
            return (uint32_t)strtoul(it->second.c_str(), nullptr, 10);
        }
        return defaultValue;
    }

private:
    std::map<std::string, std::string> storage;
};
