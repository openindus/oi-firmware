#include "OIConsole.h"

#include <string.h>

static const char* OI_CONSOLE_TAG = "OIConsole";

#if defined CONFIG_OI_CORE
#define PROMPT_STR "OICore"
#elif defined CONFIG_OI_DISCRETE || defined CONFIG_OI_DISCRETE_VERTICAL
#define PROMPT_STR "OIDiscrete"
#elif defined CONFIG_OI_STEPPER
#define PROMPT_STR "OIStepper"
#elif defined CONFIG_OI_MIXED
#define PROMPT_STR "OIMixed"
#else
#define PROMPT_STR ""
#endif


void OIConsole::start(void)
{
    ESP_LOGI(OI_CONSOLE_TAG, "console start");

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    repl_config.prompt = PROMPT_STR ">";

    /* init console REPL environment */
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    /* register commands */
    _registerRestart();
    _registerSetId();
    _registerGetId();
    _registerLogLevel();
    _registerPing();
    _registerDownloadMode();
    _registerListen();
    _registerGetAllTypes();
    _registerCmd();

    printf("\n ==================================================\n");
    printf(" |                                                |\n");
    printf(" |       Welcome to the OpenIndus System          |\n");
    printf(" |                                                |\n");
    printf(" ==================================================\n\n");

    /* start console REPL */
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}


/************************************************/
/*                   restart                    */
/************************************************/

int OIConsole::_restart(int argc, char **argv)
{
    esp_restart();
    return 0;
}

void OIConsole::_registerRestart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "restart the cpu",
        .hint = NULL,
        .func = &_restart,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                   set-id                     */
/************************************************/

static struct {
    struct arg_int *id;
    struct arg_end *end;
} setIdArgs;

int OIConsole::_setId(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setIdArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setIdArgs.end, argv[0]);
        return 1;
    }

    int id = setIdArgs.id->ival[0];

    if ((id >= 0x00) && (id <0xFE))
    {
        (System.getModule())->setHardwareId(id);
    }
    else
    {
        ESP_LOGE(OI_CONSOLE_TAG, "bad id");
    }

    return 0;
}

void OIConsole::_registerSetId(void)
{
    setIdArgs.id = arg_int1(NULL, NULL, "<id>", "board id");
    setIdArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "set-id",
        .help = "set the current board id",
        .hint = NULL,
        .func = &_setId,
        .argtable = &setIdArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                   get-id                     */
/************************************************/

int OIConsole::_getId(int argc, char **argv)
{
    printf("%d\n", (System.getModule())->getId());
    return 0;
}

void OIConsole::_registerGetId(void)
{
    const esp_console_cmd_t cmd = {
        .command = "get-id",
        .help = "get the current board id",
        .hint = NULL,
        .func = &_getId,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                 log-level                    */
/************************************************/

static struct {
    struct arg_int *level;
    struct arg_end *end;
} logLevelArgs;

int OIConsole::_logLevel(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &logLevelArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, logLevelArgs.end, argv[0]);
        return 1;
    }

    int level = logLevelArgs.level->ival[0];

    if ((level >= 0) && (level <= 5)) 
    {
        esp_log_level_set("*", (esp_log_level_t) level);
    }
    else 
    {
        ESP_LOGE(OI_CONSOLE_TAG, "bad log level");
    }

    return 0;
}

void OIConsole::_registerLogLevel(void)
{
    logLevelArgs.level = arg_int1(NULL, NULL, "<level>", "log level (0 = NONE ... 5 = VERBOSE)");
    logLevelArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "log-level",
        .help = "set the log level",
        .hint = NULL,
        .func = &_logLevel,
        .argtable = &logLevelArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                   ping                       */
/************************************************/

static struct {
    struct arg_int *id;
    struct arg_end *end;
} pingArgs;

int OIConsole::_ping(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &pingArgs);
    printf("%u\n", nerrors);
    if (nerrors != 0) {
        arg_print_errors(stderr, pingArgs.end, argv[0]);
        return 1;
    }

    int id = pingArgs.id->ival[0];
    int ping = 0;
    int64_t time1 = 0, time2 = 0;

    if ((id >= 0) && (id <= 254)) 
    {
        if (id == (System.getModule())->getId())
        {
            ESP_LOGW(OI_CONSOLE_TAG, "You can't png the current module");
            ping = 0xAA;
        }
        else
        {
            OIMessage msg;
            msg.setType(CMD_PING);
            msg.setId((System.getModule())->getId());

            time1 = esp_timer_get_time();
            ping = BusRS.getMessage(msg, id); 
            time2 = esp_timer_get_time();
        }
    }
    else 
    {
        ESP_LOGE(OI_CONSOLE_TAG, "bad id");
    }

    if (ping == 0xAA)
    {
        ESP_LOGI(OI_CONSOLE_TAG, "ping module id: %u time: %lld us", id, time2 - time1);
    }
    else
    {
        ESP_LOGW(OI_CONSOLE_TAG, "cannot ping module id: %u", id);
    }

    return 0;
}

void OIConsole::_registerPing(void)
{
    pingArgs.id = arg_int1(NULL, NULL, "<id>", "board id");
    pingArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "ping",
        .help = "ping a board",
        .hint = NULL,
        .func = &_ping,
        .argtable = &pingArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                download mode                 */
/************************************************/

static struct {
    struct arg_int *id;
    struct arg_end *end;
} downloadModeArgs;

int OIConsole::_downloadMode(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &downloadModeArgs);
    printf("%u\n", nerrors);
    if (nerrors != 0) {
        arg_print_errors(stderr, pingArgs.end, argv[0]);
        return 1;
    }

    OIMessage msg;
    msg.setType(CMD_DOWNLOAD_MODE);
    msg.setId((System.getModule())->getId());
    
    if (downloadModeArgs.id->count == 0)
    {
        Fct.run(msg);
    }
    else
    {
        int id = downloadModeArgs.id->ival[0];
        
        if ((id >= 0) && (id <= 254)) 
        {
            if (id == (System.getModule())->getId())
            {
                Fct.run(msg);
            }
            else
            {
                
                BusRS.setMessage(msg, id); 
            }
        }
        else 
        {
            ESP_LOGE(OI_CONSOLE_TAG, "bad id");
        }
    }

    return 0;
}

void OIConsole::_registerDownloadMode(void)
{
    downloadModeArgs.id = arg_int0("i", "id", "<ID>", "destination module id");
    downloadModeArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "download-mode",
        .help = "enter download mode",
        .hint = NULL,
        .func = &_downloadMode,
        .argtable = &downloadModeArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                  listen                      */
/************************************************/

static struct {
    struct arg_str *mode; /* ON or OFF */
    struct arg_end *end;
} listenArgs;

int OIConsole::_listen(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &listenArgs);
    printf("%u\n", nerrors);
    if (nerrors != 0) {
        arg_print_errors(stderr, listenArgs.end, argv[0]);
        return 1;
    }

    OIMessage msg;
    msg.setType(CMD_SYSTEM);
    msg.setId((System.getModule())->getId());
    
    if (listenArgs.mode->count == 1)
    {
        if (strcmp(listenArgs.mode->sval[0], "ON") == 0)
        {
            msg.setConf(257);
        }
        else
        {
            msg.setConf(256);
        }

        BusTWAI.sendMessage(msg, 255);
    }
    else
    {
        ESP_LOGE(OI_CONSOLE_TAG, "Please specify ON or OFF");
    }

    return 0;
}

void OIConsole::_registerListen(void)
{
    listenArgs.mode = arg_str0(NULL, NULL, "MODE", "listen mode (ON or OFF)");
    listenArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "listen",
        .help = "set listen to on or off",
        .hint = NULL,
        .func = &_listen,
        .argtable = &listenArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*               get all types                  */
/************************************************/

int OIConsole::_getAllTypes(int argc, char **argv)
{
    OIMessage msg;
    
    /* TYPE */
    msg.setType(CMD_GET_TYPE_ALL);

    /* ID */
    msg.setId((System.getModule())->getId());

    BusRS.sendMessage(msg, 0xFF);

    return 0;
}

void OIConsole::_registerGetAllTypes(void)
{
    const esp_console_cmd_t cmd = {
        .command = "get-all-types",
        .help = "get a list of all boards on the bus with their type",
        .hint = NULL,
        .func = &_getAllTypes,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/************************************************/
/*                   command                    */
/************************************************/

static struct {
    struct arg_str *bus;
    struct arg_int *id;
    struct arg_int *type;
    struct arg_int *conf;
    struct arg_int *data;
    struct arg_end *end;
} commandArgs;

int OIConsole::_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &commandArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, commandArgs.end, argv[0]);
        return 1;
    }

    OIMessage msg;

    /* TYPE */
    msg.setType(static_cast<uint8_t>(commandArgs.type->ival[0]));

    /* ID */
    msg.setId((System.getModule())->getId());

    /* CONF */
    if (commandArgs.conf->count > 0) {
        msg.setConf(static_cast<uint16_t>(commandArgs.conf->ival[0]));
    }

    /* DATA */
    if (commandArgs.data->count > 0) {
        msg.setData(static_cast<uint32_t>(commandArgs.data->ival[0]));
    }

    if (commandArgs.id->count > 0)
    {
        uint8_t id = static_cast<uint8_t>(commandArgs.id->ival[0]);

        if (id == msg.getId())
        {
            if ((msg.getType() & MASK_GET) == TYPE_GET)
            {
                printf("%d\n", Fct.run(msg));
            }
            else
            {
                Fct.run(msg);
            }
        }
        else
        {
            if ((msg.getType() & MASK_SET) == TYPE_SET)
            {
                BusRS.setMessage(msg, id);
            }
            else if ((msg.getType() & MASK_GET) == TYPE_GET)
            {
                printf("%d\n", BusRS.getMessage(msg, id));
            }
            else if ((msg.getType() & MASK_SEND) == TYPE_SEND)
            {
                /* CHECK BUS */
                if (commandArgs.bus->count == 1)
                {
                    if(strcmp(commandArgs.bus->sval[0], "CAN") == 0)
                    {
                        BusTWAI.sendMessage(msg, id);
                    }
                    else if (strcmp(commandArgs.bus->sval[0], "RS") == 0)
                    {
                        BusRS.sendMessage(msg, id);
                    }
                    else
                    {
                        BusTWAI.sendMessage(msg, id);
                    }
                }
                else
                {
                    BusTWAI.sendMessage(msg, id);
                }
            }
        }
    }
    else
    {
        if ((msg.getType() & MASK_GET) == TYPE_GET)
        {
            printf("%d\n", Fct.run(msg));
        }
        else
        {
            Fct.run(msg);
        }
    }

    return 0;
}

void OIConsole::_registerCmd(void)
{
    commandArgs.bus     = arg_str0("b", "bus", "{CAN,RS}", "specify the com bus (only for messages of type SEND)");
    commandArgs.id      = arg_int0("i", "id", "<ID>", "destination module id");
    commandArgs.type    = arg_int1(NULL, NULL, "<TYPE>", "type of the command");
    commandArgs.conf    = arg_int0(NULL, NULL, "CONF", "configuration [0, 0xFFFF]");
    commandArgs.data    = arg_int0(NULL, NULL, "DATA", "data [0, 0xFFFFFFFF]");
    commandArgs.end     = arg_end(6);

    const esp_console_cmd_t cmd = {
        .command = "cmd",
        .help = "run a module command ",
        .hint = NULL,
        .func = &_cmd,
        .argtable = &commandArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

OIConsole Console;