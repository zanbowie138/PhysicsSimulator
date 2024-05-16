#include <functional>

class EventManager {
    bool value;
    std::function<void()> callback;

public:
    EventManager(bool initialValue, std::function<void()> callbackFunction)
        :value(initialValue), callback(callbackFunction) {}

    void setValue(bool newValue) {
        if (newValue != value) {
            value = newValue;
            callback();
        }
    }
};

// Usage:
// auto handler = EventManager(false, []() { std::cout << "Value changed!\n"; });
// handler.setValue(true);  // This will trigger the callback and print "Value changed!"
