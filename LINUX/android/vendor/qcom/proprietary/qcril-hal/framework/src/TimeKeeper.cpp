#include <iostream>
#include <chrono>
#include <ctime>
#include <ratio>
#include <sstream>
#include <iomanip>

#include "framework/TimeKeeper.h"
#include "framework/Log.h"
using namespace std::chrono;

using std::chrono::steady_clock;
using std::chrono::system_clock;


void print_remaining_time (std::chrono::time_point<steady_clock> p) {
    auto now = steady_clock::now ();
    auto diff = p - now;
    auto secs = std::chrono::duration_cast<seconds>(diff);
#ifndef QMI_RIL_UTF
    std::cout << "\n Remaining " << secs.count();
#endif
}

void print_time (std::chrono::time_point<system_clock> p) {
    std::time_t ttp = std::chrono::system_clock::to_time_t (p);
#ifndef QMI_RIL_UTF
    std::cout << "\n time: " << std::ctime (&ttp);
#endif
}

TimeKeeper &TimeKeeper::getInstance () {
    static TimeKeeper sInstance;
    return sInstance;
}

void TimeKeeper::loop () {
    ScopedLock lock (looper_mutex);
    while (!exit) {
        if (priority_queue.empty ()) {
            Log::getInstance().d ("[Timer-Looper]: waiting...");
            looper_cv.wait (lock, [this] { return exit || !priority_queue.empty (); });
            continue;
        }
        else {
            auto queueHead = priority_queue.begin ();
            std::shared_ptr<Timer> timer = *queueHead;
            auto now = steady_clock::now ();
            //print_time (now);
            //print_time (timer->expiring_at_ms);
            print_remaining_time (timer->expiring_at_ms);
            if (now >= timer->expiring_at_ms) {
                Log::getInstance().d ("[Timer-Looper]: Timer expired. Invoke callback for timer_id = " + std::to_string(timer->id));
                active_timers_map.erase(timer->id);
                priority_queue.erase (queueHead);
                timer->running = true;
                lock.unlock ();
                timer->handler (timer->user_data);
                lock.lock ();
                timer->running = false;

            }
            else {
                looper_cv.wait_until (lock, timer->expiring_at_ms);
            }
        }

    }

    if (exit) {
        Log::getInstance().d ("[Timer-Looper]: Exiting, " + std::to_string (priority_queue.size ()) + " timers still pending.");
        priority_queue.clear ();
        active_timers_map.clear();
        return;
    }
}

TimeKeeper::timer_id TimeKeeper::set_timer (handler_type handler, user_data_type user_data, millisec timeout)
{
    ScopedLock lock (looper_mutex);

    if (looper_thread == nullptr) {
        looper_thread = new std::thread (&TimeKeeper::loop, this);
    }
    timer_id id = nextId++;

    std::chrono::time_point<steady_clock> expected_expiry = steady_clock::now () + std::chrono::milliseconds (timeout);
    std::shared_ptr<Timer> shared_timer = std::make_shared<Timer>(id, expected_expiry, std::move (handler), user_data);
    Queue::iterator place = priority_queue.insert(shared_timer);
    active_timers_map[id] = shared_timer;

    bool req_wakeup = (place == priority_queue.begin ());
    lock.unlock ();

    if (req_wakeup) looper_cv.notify_all ();

    return id;
}

bool TimeKeeper::clear_timer (timer_id id)
{
    bool result = true;
    {
        std::lock_guard<std::mutex> lock (looper_mutex);

        auto i = active_timers_map.find (id);
        if (i == active_timers_map.end ()) {
            Log::getInstance().d("[Timer-Looper]: Timer not found = " + std::to_string(id));
            return false;
        }

        std::shared_ptr<Timer> timer = i->second;
        if(timer) {
          if (timer->running) {
              Log::getInstance().d("[Timer-Looper]: Callback in progress for timer = " + std::to_string(id)
                      + ", can not cancel.");
              return false;
          }
          else {
              auto pos = priority_queue.find(timer);
              active_timers_map.erase (i);
              priority_queue.erase(pos);
              Log::getInstance().d("[Timer-Looper]: Timer deleted  = " + std::to_string(id));
          }
        } else {
            Log::getInstance().d("[Timer-Looper]: timer is NULL.");
        }
    }

    /* Notify only when we are able to delete timer*/
    looper_cv.notify_all ();

    return result;
}

void TimeKeeper :: kill () {
    {
        std::lock_guard<std::mutex> lock (looper_mutex);
        exit = true;
    }
    looper_cv.notify_all ();

    wait_for_looper_to_join ();
}

void TimeKeeper::wait_for_looper_to_join () {
    if (looper_thread) {
        Log::getInstance().d ("[TimerThread]: Waiting for timer thread to join.");
        Log::getInstance().d ("[Timer-Looper]: " + std::to_string (priority_queue.size ()) + " timers still pending.");
        looper_thread->join ();
        Log::getInstance().d ("[TimerThread]: timer thread joined.");
        delete looper_thread;
        looper_thread = nullptr;
    }
}

