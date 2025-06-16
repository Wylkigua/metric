#pragma once
#include <string>
#include <expected>
#include <fstream>
#include <cstring>
#include <format>
#include <unistd.h>

class Metric {
public:
    const std::string name;
    Metric(const std::string& name) : name(name) {}
    virtual std::expected<std::string, std::string> metric() = 0;
    virtual ~Metric() = default;
};

class RamMemMetric : public Metric {
public:
    RamMemMetric();
    std::expected<std::string, std::string>
    metric() override final;

    std::expected<unsigned long, std::string>
    read_process_mem();
    std::expected<unsigned long, std::string>
    read_total_mem();
};