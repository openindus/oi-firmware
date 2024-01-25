#ifndef FSM_H
#define FSM_H

#include <functional>
#include <vector>
#include "Arduino.h"


class Event
{
public:
    Event();

    friend bool operator==(const Event &e1, const Event &e2);

    typedef enum
    {
        ANY_RECEIVE_EVENT,
        CALL_EVENT,
        CHANGE_EVENT,
        SIGNAL_EVENT,
        TIME_EVENT
    } EventType_t;

private:
    EventType_t _type;

    static int _counter;
    int _number;
};


typedef std::function<void(void* parameter)> ActionFunction_t;

class State
{
public:
    State(ActionFunction_t entry_action, ActionFunction_t do_activity, ActionFunction_t exit_action);
    State();

    friend bool operator==(const State &s1, const State &s2);

    typedef enum
    {
        SIMPLE_STATE,
        EXTENDED_STATE
    } StateType_t;

    ActionFunction_t _entry_action;
    ActionFunction_t _do_activity;
    ActionFunction_t _exit_action;

    Event _event;
    ActionFunction_t _on_event_action;

private:
    StateType_t _type;
    static int _counter;
    int _number;
};


class Transition
{
public:
    Transition(State* current_state, Event* event, ActionFunction_t action, State* next_state);
    Transition();

    State* _current_state;    
    Event* _event;
    ActionFunction_t _action;
    State* _next_state;    

private:
    static int _counter;
    int _number;
};


class FSM
{
public:
    FSM();
    ~FSM();

    void begin();
    void end();

    void set(const Transition &transition);
    void setTransition(State* current_state, Event* event, State* next_state);
    void setTransition(State* current_state, Event* event, ActionFunction_t action, State* next_state);    
    void setInitialState(State* state);

    void trigger(Event* event);

private:
    bool _initialized;

    static void _task_function(void *pvParameter);
    TaskHandle_t _task_handle;
    SemaphoreHandle_t _xMutex;

    std::vector<Transition*> _transition_table;
    State* _current_state;   
    Event* _event;

    void handleEvent(Event* event);
    void activity();
};

#endif