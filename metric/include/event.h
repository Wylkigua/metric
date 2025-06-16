#pragma once
#include <fstream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <expected>
#include <iostream>
#include <format>
#include <system_error>
#include <cstring>
#include <iomanip>
#include "metric.h"

class Event {
    std::ofstream file;
    std::string error_message;
    std::vector<std::unique_ptr<Metric>> metrics;

    std::condition_variable event_metric_trigger;
    std::jthread metric_thread;
    std::atomic<bool> running{true};
    std::atomic<bool> error{false};
    std::atomic<bool> writing{false};
    std::mutex mtx;

public:
    explicit Event(const std::string& file_name);
    ~Event() {event_stop();};
   
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event(Event&&) = delete;
    Event& operator=(Event&&) = delete;

    bool is_error() const { return error.load(); };
    void event_run();
    void event_stop();
    void event_add(std::unique_ptr<Metric> metric);

private:
    std::expected<void, std::string> write();
    inline std::string current_time() const;
};