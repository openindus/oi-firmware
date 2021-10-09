#include "OIFunction.h"


static const char OI_FUNCTION_TAG[] = "OIFunction";

void OIFunction::add(OIMessage const& msg, OICommand function)
{
    _commandTable.insert(
        std::multimap<OIMessage, OICommand>::value_type(
            msg, 
            function
        )
    );
}

void OIFunction::remove(OIMessage const& msg)
{
    _commandTable.erase(msg);
}

bool OIFunction::exist(OIMessage const& msg)
{
    if (_commandTable.find(msg) != _commandTable.end()) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t OIFunction::run(OIMessage const& msg)
{
    if (Fct.exist(msg))
    {
        for (auto it=_commandTable.equal_range(msg).first; it!=_commandTable.equal_range(msg).second; ++it)
        {
            if (_commandTable.count(msg) > 1)
            {
                if ((*it).first.getId() == msg.getId())
                {
                    return (*it).second(msg);
                }
            }
            else 
            {
                return (*it).second(msg);
            }
        }
    }
    else
    {
        ESP_LOGW(OI_FUNCTION_TAG, "command does not exist: 0x%02x", msg.getType());
    }
    return 0;
}

void OIFunction::list(void)
{
    for (auto it=_commandTable.begin(); it!=_commandTable.end(); ++it)
    {
        printf("Type: %d, Id: %d\n", it->first.getType(), it->first.getId());
    }
}

OIFunction Fct;