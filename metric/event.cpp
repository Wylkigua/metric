#include "include/event.h"


Event::Event(const std::string& file_name) : file(file_name){
    if (!file.is_open()) {
        throw std::system_error(
            errno,
            std::system_category(),
            std::format("error open file: {}", file_name)
        );
    }
    metric_thread = std::jthread([this]{
        if(auto result = write(); !result) {
            error_message = result.error();
            error.store(true);
        }
    });
}

void Event::event_run() {
    writing.store(true);
    running.store(true);
    event_metric_trigger.notify_all();
}

void Event::event_stop() {
    writing.store(false);
    running.store(false);
    event_metric_trigger.notify_all();
}

void Event::event_add(std::unique_ptr<Metric> metric) {
    std::scoped_lock lock(mtx);
    metrics.push_back(std::move(metric));
}

std::expected<void, std::string>
Event::write() {
    while(running) {
        std::unique_lock lock(mtx);
        event_metric_trigger.wait(lock, [this]{
            return writing.load() || !running.load(); 
        });
        if (!running) break;

        std::string buf;
        bool success = false;
        for (auto it = metrics.begin(); it != metrics.end(); ++it) {
            if (auto result = it->get()->metric(); result) {
                success = true;
                buf.append(std::format(R"("{}" {})", 
                    it->get()->name, *result));
            } else {
                std::cerr << it->get()->name << ": " << result.error() << "\n";
            }
            if (metrics.end() - it > 1) {
                buf.append(" ");
            }
        }
        lock.unlock();
        if (success) {
            file << current_time();
            buf.append("\n");
            file << buf << std::flush;
        }

        if (file.fail()) {
            file.clear();
            return std::unexpected(std::strerror(errno));
        }
        writing = false;
    }
    return {};
}

std::string
Event::current_time() const {
    auto now_time = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_time);
    std::tm tm = *std::localtime(&now_time_t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S ");
    return oss.str();

}