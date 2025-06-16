#include <metric.h>
#include <event.h>

using namespace std::chrono_literals;

int main(int argc, char const *argv[]) {
    Event event("metric.txt");
    event.event_add(std::make_unique<RamMemMetric>(RamMemMetric()));
    while (!event.is_error()) {
        std::this_thread::sleep_for(1s);
        event.event_run();
    }
    return 0;
}
