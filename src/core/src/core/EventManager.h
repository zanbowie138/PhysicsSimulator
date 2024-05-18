#include <functional>
#include <utils/Logger.h>

extern Utils::Logger logger;

class EventManager {
    bool value;
    std::function<void()> callback;

public:
    EventManager(bool initialValue, std::function<void()> callbackFunction)
        :value(initialValue), callback(callbackFunction) 
    {
        LOG(logger, LOG_INFO) << "EventManager created with initial value: " << initialValue << ".\n";
    }

    void setValue(bool newValue) {
        if (newValue != value) {
            value = newValue;
            callback();
            LOG(logger, LOG_INFO) << "EventManager value changed to: " << newValue << ".\n";
        }
    }
};

// Usage:
// auto handler = EventManager(false, []() { std::cout << "Value changed!\n"; });
// handler.setValue(true);  // This will trigger the callback and print "Value changed!"
