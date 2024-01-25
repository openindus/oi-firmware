#include "FSM.h"


int Event::_counter = 0;
int State::_counter = 0;
int Transition::_counter = 0;


Event::Event()
{
    _type = ANY_RECEIVE_EVENT;

    _counter++;
    _number = _counter;
}

bool operator==(const Event &e1, const Event &e2)
{
    if (e1._number == e2._number)
        return true;
    else
        return false;
}

State::State(ActionFunction_t entry_action, ActionFunction_t do_activity, ActionFunction_t exit_action) : 
    _entry_action(entry_action),
    _do_activity(do_activity),
    _exit_action(exit_action)
{
    _counter++;
    _number = _counter;
}

State::State() : 
    State(NULL, NULL, NULL)
{

}

bool operator==(const State &s1, const State &s2)
{
    if (s1._number == s2._number)
        return true;
    else
        return false;
}

Transition::Transition(State* current_state, Event* event, ActionFunction_t action, State* next_state) :
    _current_state(current_state),
    _event(event),
    _action(action) ,
    _next_state(next_state)    
{
    _counter++;
}

Transition::Transition() : 
    Transition(NULL, NULL, NULL, NULL)
{
    
}

FSM::FSM() :
    _current_state(NULL),
    _event(NULL)
{
    _initialized = false;
    _task_handle = NULL;
    _xMutex = xSemaphoreCreateMutex();
}

FSM::~FSM()
{

}

void FSM::_task_function(void *pvParameters)
{
    FSM* fsm = (FSM*)pvParameters;

    while (1)
    {        
        xSemaphoreTake(fsm->_xMutex, portMAX_DELAY);
        Event* event(fsm->_event);
        xSemaphoreGive(fsm->_xMutex);

        if (event != NULL)
        {
            fsm->handleEvent(event);

            /* Reset Event */
            xSemaphoreTake(fsm->_xMutex, portMAX_DELAY);
            fsm->_event = NULL;
            xSemaphoreGive(fsm->_xMutex);
        }
        else
        {
            /* Activity */
            fsm->activity();
        }
        vTaskDelay(1);
    }
}

void FSM::trigger(Event* event)
{
    if (_initialized == true)
	{
        xSemaphoreTake(_xMutex, portMAX_DELAY);
        _event = event;
        xSemaphoreGive(_xMutex);
	}
}

void FSM::begin()
{
	if (_initialized == false)
	{
        xTaskCreate(&_task_function, "FSM Process", 4096, this, 3, &_task_handle);
        if (_task_handle)
        {
            _initialized = true;
        }		
	}
}

void FSM::end()
{
    if(_task_handle)
    {
        vTaskDelete(_task_handle);
        _task_handle = NULL;
        _initialized = false;
    }
}

void FSM::set(const Transition &transition)
{
    _transition_table.push_back(new Transition(transition));
}

void FSM::setTransition(State* current_state, Event* event, State* next_state)
{
    _transition_table.push_back(new Transition(current_state, event, NULL, next_state));
}

void FSM::setTransition(State* current_state, Event* event, ActionFunction_t action, State* next_state)
{
    _transition_table.push_back(new Transition(current_state, event, action, next_state));
}

void FSM::setInitialState(State* state)
{
    _current_state = state;
}

void FSM::handleEvent(Event* event)
{
    for (int i(0); i<_transition_table.size(); i++)
    {
        if ((_transition_table[i]->_current_state == _current_state) && (_transition_table[i]->_event == event))
        {
            /* Exit / Action*/
            if (_current_state->_exit_action != NULL)
            {
                _current_state->_exit_action(NULL);
            }
            
            /* Transition */
            _current_state = _transition_table[i]->_next_state;                    
            
            if (_transition_table[i]->_action != NULL)
            {
                _transition_table[i]->_action(NULL);
            }

            /* Entry / Action*/
            if (_current_state->_entry_action != NULL)
            {
                _current_state->_entry_action(NULL);
            }

            return;
        }
    }
}

void FSM::activity()
{
    if (_current_state != NULL)
    {
        /* Do / Action*/
        if (_current_state->_do_activity != NULL)
        {
            _current_state->_do_activity(NULL);
        }
    }
}