#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <set>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <ctime>
#include <ratio>
#include <memory>
#include <sstream>

using namespace std::chrono;
using std::chrono::steady_clock;
using std::chrono::system_clock;

class TimeKeeper {
public:
    using timer_id = std::uint64_t;
    using millisec = std::int64_t;
    //using millisec = std::chrono::milliseconds;
    using user_data_type = void*;
    using handler_type = std::function<void (user_data_type)>;

    static timer_id constexpr no_timer = timer_id (0);
    static TimeKeeper &getInstance ();

    TimeKeeper (TimeKeeper const &) = delete;             // Copy construct
    TimeKeeper (TimeKeeper &&) = delete;                  // Move construct
    TimeKeeper &operator=(TimeKeeper const &) = delete;  // Copy assign
    TimeKeeper &operator=(TimeKeeper &&) = delete;       // Move assign

    /************* APIs ****************************/
    timer_id set_timer (handler_type handler, user_data_type user_data, millisec timeout);
    bool clear_timer (timer_id id);
    void kill ();
    inline ~TimeKeeper () {};

    /* Only for unit testing*/
    void wait_for_looper_to_join ();


private:
    inline TimeKeeper () {
        exit  = false;
        nextId = no_timer + 1;
        //priority_queue();
        looper_thread = nullptr;
    }

    using Lock = std::mutex;
    using ScopedLock = std::unique_lock<Lock>;
    using ConditionVar = std::condition_variable;

    using Clock = std::chrono::system_clock;
    using Timestamp = std::chrono::time_point<steady_clock>;
    using Duration = std::chrono::milliseconds;

    timer_id nextId;

    struct Timer {
        explicit Timer (timer_id id = 0) : id {id}, handler{nullptr}, user_data{nullptr}, running {false}{};

        Timer (Timer &&r);

        //Timer &operator=(Timer &&r) = delete;

        Timer (timer_id id, Timestamp next, handler_type handler, user_data_type user_data);

        inline Timer (Timer const &r) {
            id = r.id;
            expiring_at_ms = r.expiring_at_ms;
            handler = r.handler;
            user_data = r.user_data;
            running = r.running;
        }

        inline Timer &operator=(Timer const &r) {
            id = r.id;
            expiring_at_ms = r.expiring_at_ms;
            handler = r.handler;
            user_data = r.user_data;
            running = r.running;
            return *this;
        }

        timer_id id;
        Timestamp expiring_at_ms;
        handler_type handler;
        user_data_type user_data;

        bool running;
    };

    struct Compare {
        bool operator()(std::shared_ptr<Timer> const &a, std::shared_ptr<Timer> const &b) const noexcept {
            return a->expiring_at_ms < b->expiring_at_ms;
        }
    };

    void loop ();

    using Queue = std::multiset<std::shared_ptr<Timer>, Compare>;
    using TimerMap = std::unordered_map<timer_id, std::shared_ptr<Timer>>;

    TimerMap active_timers_map;
    Queue priority_queue;

    bool exit;
    std::mutex looper_mutex;
    std::condition_variable looper_cv;
    std::thread* looper_thread;

};

inline TimeKeeper::Timer::Timer (TimeKeeper::Timer &&r) : id {std::move (r.id)},
    expiring_at_ms{ std::move (r.expiring_at_ms) },
    handler{ std::move (r.handler) },
    user_data{ std::move (r.user_data) },
    running{ std::move (r.running) } {}

inline TimeKeeper::Timer::Timer (timer_id id, Timestamp expiring_at_ms, handler_type handler, user_data_type user_data) : id (id),
    expiring_at_ms{ expiring_at_ms },
    handler {std::move (handler)},
    user_data {user_data},
    running{ false } {}

