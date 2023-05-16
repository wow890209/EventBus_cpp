#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <type_traits>

class EventBase {
public:
    virtual ~EventBase() = default;
};

template <typename Derived>
class Event : public EventBase {
public:
    using EventType = Derived;
};

class HelloEvent : public Event<HelloEvent> {
public:
    HelloEvent(std::string name) : name(name) {}
    const std::string name;
};

class CryEvent : public Event<CryEvent> {
public:
};

void handleHelloEvent(const HelloEvent& event) {
    std::cout << event.name << std::endl;
}

void handleCryEvent(const CryEvent& event) {
    std::cout << "cry" << std::endl;
}

class EventBus {
public:
    template<typename... EventTypes>
    explicit EventBus(EventTypes... eventTypes) {
        (addEvent<EventTypes>(eventTypes), ...);
    }

    template<typename EventType, typename... Args>
    void addEvent(Args&&... args) {
        events.push_back(std::make_unique<EventType>(std::forward<Args>(args)...));
    }

    const std::vector<std::unique_ptr<EventBase>>& getEvents() const {
        return events;
    }

private:
    std::vector<std::unique_ptr<EventBase>> events;
};

template <typename EventType, typename Handler>
void processEvent(const std::unique_ptr<EventBase>& event, Handler&& handler) {
    const EventType* derived_event = dynamic_cast<const EventType*>(event.get());
    if (derived_event) {
        handler(*derived_event);
    }
}

void processEvents(const EventBus& eventBus) {
    const auto& events = eventBus.getEvents();

    for (const auto& event : events) {
        processEvent<HelloEvent>(event, handleHelloEvent);
        processEvent<CryEvent>(event, handleCryEvent);
    }
}

EventBus actor(){
    return EventBus(HelloEvent("infinite"));
}

int main() {
    auto eventBus = actor();

    processEvents(eventBus);

    return 0;
}
