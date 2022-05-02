#include "OICommand.h"


static const char OI_COMMAND_TAG[] = "OICommand";

void OICommand::add(OIMessage const& msg, OIFonction function)
{
    _commandTable.insert(
        std::multimap<OIMessage, OIFonction>::value_type(
            msg, 
            function
        )
    );
}

void OICommand::remove(OIMessage const& msg)
{
    _commandTable.erase(msg);
}

bool OICommand::exist(OIMessage const& msg)
{
    if (_commandTable.find(msg) != _commandTable.end()) {
        return true;
    }
    else {
        return false;
    }
}

uint32_t OICommand::run(OIMessage const& msg)
{
    if (CMD.exist(msg))
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
        ESP_LOGW(OI_COMMAND_TAG, "command does not exist: 0x%02x", msg.getType());
    }
    return 0;
}

void OICommand::list(void)
{
    for (auto it=_commandTable.begin(); it!=_commandTable.end(); ++it)
    {
        printf("Type: %d, Id: %d\n", it->first.getType(), it->first.getId());
    }
}

OICommand CMD;