#include "include/metric.h"


RamMemMetric::RamMemMetric() : Metric("RamMemMetric") {}

std::expected<std::string, std::string> RamMemMetric::metric() {
    auto result = read_process_mem()
        .and_then([this](unsigned long process_mem) {
            return read_total_mem()
                .transform([process_mem](unsigned long total_mem) {
                    double using_process_mem = (static_cast<double>(process_mem) / total_mem) * 100.;
                    return std::format("{:.3f}", using_process_mem);
                });
        });
    if (!result) {
        return std::unexpected(result.error());
    }
    return result.value();
}


std::expected<unsigned long, std::string>
RamMemMetric::read_process_mem() {
    pid_t pid = getpid();
    unsigned long resident_page_size{};
    std::string file("/proc/" + std::to_string(pid) + "/statm");
    std::ifstream mem_proc_file(file);
    if (!mem_proc_file) {
        return std::unexpected(
            std::format("{}: {} ", file, std::strerror(errno)));
    }
    mem_proc_file >> resident_page_size >> resident_page_size;
    unsigned long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    double resident_size_kb = resident_page_size * page_size_kb;
    if (resident_size_kb <= 0) {
        return std::unexpected(
            std::format("using resident size kb: {}", resident_page_size));
    }
    return resident_size_kb;

}
std::expected<unsigned long, std::string>
RamMemMetric::read_total_mem() {
    std::ifstream mem_total_file("/proc/meminfo");
    if (!mem_total_file) {
        return std::unexpected(
            std::format("/proc/meminfo: {} ", std::strerror(errno)));
    }
    unsigned long total_mem_size_kb{};
    mem_total_file.seekg(std::size("MemTotal"), std::ios::beg) >> total_mem_size_kb;
    if (total_mem_size_kb <= 0) {
        return std::unexpected(std::format("error read total mem : {}", total_mem_size_kb));
    }
    return total_mem_size_kb;
}