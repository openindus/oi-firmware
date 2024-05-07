// /**
//  * Copyright (C) OpenIndus, Inc - All Rights Reserved
//  *
//  * This file is part of OpenIndus Library.
//  *
//  * Unauthorized copying of this file, via any medium is strictly prohibited
//  * Proprietary and confidential
//  * 
//  * @file StepperCLI.h
//  * @brief OpenIndus console
//  *
//  * For more information on OpenIndus:
//  * @see https://openindus.com
//  */

// #include "StepperCLI.h"

// #if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

// int StepperCLI::init(void)
// {
//     _registerDigitalRead();
//     _registerAttachInterrupt();
//     _registerDetachInterrupt();
//     _registerLimitSwitch();
//     _registerStepResolution();
//     _registerSetMaxSpeed();
//     _registerGetPosition();
//     _registerGetSpeed();
//     _registerStop();
//     _registerMoveAbsolute();
//     _registerMoveRelative();
//     _registerRun();
//     _registerHoming();

//     return 0;
// }

// /** 'digital-read' */

// static struct {
//     struct arg_int *din;
//     struct arg_end *end;
// } digitalReadArgs;

// static int dRead(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &digitalReadArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, digitalReadArgs.end, argv[0]);
//         return 1;
//     }

//     DigitalInputNum_t din = (DigitalInputNum_t)(digitalReadArgs.din->ival[0] - 1);

//     printf("%d\n", StepperStandalone::digitalRead(din));

//     return 0;
// }

// void StepperCLI::_registerDigitalRead(void)
// {
//     digitalReadArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
//     digitalReadArgs.end     = arg_end(2);

//     const esp_console_cmd_t cmd = {
//         .command = "digital-read",
//         .help = "Get DIN level",
//         .hint = NULL,
//         .func = &dRead,
//         .argtable = &digitalReadArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'attach-interrupt' */

// static struct {
//     struct arg_int *din;
//     struct arg_int *mode;
//     struct arg_end *end;
// } attachInterruptArgs;

// static void callback(void* arg)
// {
//     printf("isr\n");
// }

// static int attachInterrupt(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &attachInterruptArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, attachInterruptArgs.end, argv[0]);
//         return 1;
//     }

//     DigitalInputNum_t din = (DigitalInputNum_t)(attachInterruptArgs.din->ival[0] - 1);
//     InterruptMode_t mode = (InterruptMode_t)(attachInterruptArgs.mode->ival[0]);

//     StepperStandalone::attachInterrupt(din, callback, mode, NULL);

//     return 0;
// }

// void StepperCLI::_registerAttachInterrupt(void)
// {
//     attachInterruptArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
//     attachInterruptArgs.mode    = arg_int1(NULL, NULL, "MODE", "[1: rising, 2: falling, 3: change]");
//     attachInterruptArgs.end     = arg_end(4);

//     const esp_console_cmd_t cmd = {
//         .command = "attach-interrupt",
//         .help = "attach interrupt",
//         .hint = NULL,
//         .func = &attachInterrupt,
//         .argtable = &attachInterruptArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'detach-interrupt' */

// static struct {
//     struct arg_int *din;
//     struct arg_end *end;
// } detachInterruptArgs;

// static int detachInterrupt(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &detachInterruptArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, detachInterruptArgs.end, argv[0]);
//         return 1;
//     }

//     DigitalInputNum_t din = (DigitalInputNum_t)(detachInterruptArgs.din->ival[0] - 1);

//     StepperStandalone::detachInterrupt(din);

//     return 0;
// }

// void StepperCLI::_registerDetachInterrupt(void)
// {
//     detachInterruptArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
//     detachInterruptArgs.end     = arg_end(2);

//     const esp_console_cmd_t cmd = {
//         .command = "detach-interrupt",
//         .help = "detach interrupt",
//         .hint = NULL,
//         .func = &detachInterrupt,
//         .argtable = &detachInterruptArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'limit-switch' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *din;
//     struct arg_int *logic;
//     struct arg_end *end;
// } limitSwitchArgs;

// static int limitSwitch(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &limitSwitchArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, limitSwitchArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(limitSwitchArgs.motor->ival[0] - 1);
//     DigitalInputNum_t din = (DigitalInputNum_t)(limitSwitchArgs.din->ival[0] - 1);

//     if (limitSwitchArgs.logic->count > 0) {
//         DigitalInputLogic_t logic = (DigitalInputLogic_t)(limitSwitchArgs.logic->ival[0]);
//         StepperStandalone::attachLimitSwitch(motor, din, logic);
//     } else {
//         StepperStandalone::attachLimitSwitch(motor, din);
//     }

//     return 0;
// }

// void StepperCLI::_registerLimitSwitch(void)
// {
//     limitSwitchArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     limitSwitchArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
//     limitSwitchArgs.logic   = arg_int0(NULL, NULL, "logic", "[0: Active low, 1: Active high]");
//     limitSwitchArgs.end     = arg_end(4);

//     const esp_console_cmd_t cmd = {
//         .command = "attach-limit-switch",
//         .help = "set limit switch",
//         .hint = NULL,
//         .func = &limitSwitch,
//         .argtable = &limitSwitchArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'step-resolution' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *res;
//     struct arg_end *end;
// } stepResolutionArgs;

// static int stepResolution(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &stepResolutionArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, stepResolutionArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(stepResolutionArgs.motor->ival[0] - 1);
//     MotorStepResolution_t res = (MotorStepResolution_t)(stepResolutionArgs.res->ival[0]);

//     StepperStandalone::setStepResolution(motor, res);

//     return 0;
// }

// void StepperCLI::_registerStepResolution(void)
// {
//     stepResolutionArgs.motor    = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     stepResolutionArgs.res      = arg_int1(NULL, NULL, "RESOLUTION", "[0: Full step, 1: Half step, 2: 1/4 step, 3: 1/8 step, 4: 1/16 step, 5: 1/32 step, 6: 1/64 step, 7: 1/128 step]");
//     stepResolutionArgs.end      = arg_end(3);

//     const esp_console_cmd_t cmd = {
//         .command = "step-resolution",
//         .help = "set step resolution",
//         .hint = NULL,
//         .func = &stepResolution,
//         .argtable = &stepResolutionArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'set-speed' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *speed;
//     struct arg_end *end;
// } setMaxSpeedArgs;

// static int setMaxSpeed(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &setMaxSpeedArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, setMaxSpeedArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(setMaxSpeedArgs.motor->ival[0] - 1);
//     float speed = (float)(setMaxSpeedArgs.speed->ival[0]);

//     StepperStandalone::setMaxSpeed(motor, speed);

//     return 0;
// }

// void StepperCLI::_registerSetMaxSpeed(void)
// {
//     setMaxSpeedArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     setMaxSpeedArgs.speed  = arg_int1(NULL, NULL, "SPEED", "motor max speed in step/s");
//     setMaxSpeedArgs.end    = arg_end(3);

//     const esp_console_cmd_t cmd = {
//         .command = "set-speed",
//         .help = "set motor speed",
//         .hint = NULL,
//         .func = &setMaxSpeed,
//         .argtable = &setMaxSpeedArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'get-position' */

// static struct {
//     struct arg_int *motor;
//     struct arg_end *end;
// } getPositionArgs;

// static int getPosition(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &getPositionArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, getPositionArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(getPositionArgs.motor->ival[0] - 1);

//     printf("%d\n", StepperStandalone::getPosition(motor));

//     return 0;
// }

// void StepperCLI::_registerGetPosition(void)
// {
//     getPositionArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     getPositionArgs.end    = arg_end(2);

//     const esp_console_cmd_t cmd = {
//         .command = "get-position",
//         .help = "get current motor position",
//         .hint = NULL,
//         .func = &getPosition,
//         .argtable = &getPositionArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'get-speed' */

// static struct {
//     struct arg_int *motor;
//     struct arg_end *end;
// } getSpeedArgs;

// static int getSpeed(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &getSpeedArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, getSpeedArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(getSpeedArgs.motor->ival[0] - 1);

//     printf("%f\n", StepperStandalone::getSpeed(motor));

//     return 0;
// }

// void StepperCLI::_registerGetSpeed(void)
// {
//     getSpeedArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     getSpeedArgs.end    = arg_end(2);

//     const esp_console_cmd_t cmd = {
//         .command = "get-speed",
//         .help = "get current motor speed",
//         .hint = NULL,
//         .func = &getSpeed,
//         .argtable = &getSpeedArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'stop' */

// static struct {
//     struct arg_int *motor;
//     struct arg_end *end;
// } stopArgs;

// static int stop(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &stopArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, stopArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(stopArgs.motor->ival[0] - 1);

//     StepperStandalone::stop(motor);

//     return 0;
// }

// void StepperCLI::_registerStop(void)
// {
//     stopArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     stopArgs.end    = arg_end(2);

//     const esp_console_cmd_t cmd = {
//         .command = "stop",
//         .help = "stop the motor",
//         .hint = NULL,
//         .func = &stop,
//         .argtable = &stopArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'move-absolute' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *pos;
//     struct arg_end *end;
// } moveAbsoluteArgs;

// static int moveAbsolute(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &moveAbsoluteArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, moveAbsoluteArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(moveAbsoluteArgs.motor->ival[0] - 1);
//     uint32_t position = (uint32_t)(moveAbsoluteArgs.pos->ival[0]);

//     StepperStandalone::moveAbsolute(motor, position);

//     return 0;
// }

// void StepperCLI::_registerMoveAbsolute(void)
// {
//     moveAbsoluteArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     moveAbsoluteArgs.pos    = arg_int1(NULL, NULL, "POSITION", "position in step");
//     moveAbsoluteArgs.end    = arg_end(3);

//     const esp_console_cmd_t cmd = {
//         .command = "move-absolute",
//         .help = "move absolute",
//         .hint = NULL,
//         .func = &moveAbsolute,
//         .argtable = &moveAbsoluteArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'move-relative' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *pos;
//     struct arg_end *end;
// } moveRelativeArgs;

// static int moveRelative(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &moveRelativeArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, moveRelativeArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(moveRelativeArgs.motor->ival[0] - 1);
//     uint32_t position = (uint32_t)(moveRelativeArgs.pos->ival[0]);

//     StepperStandalone::moveRelative(motor, position);

//     return 0;
// }

// void StepperCLI::_registerMoveRelative(void)
// {
//     moveRelativeArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     moveRelativeArgs.pos    = arg_int1(NULL, NULL, "POSITION", "position in step");
//     moveRelativeArgs.end    = arg_end(3);

//     const esp_console_cmd_t cmd = {
//         .command = "move-relative",
//         .help = "move relative",
//         .hint = NULL,
//         .func = &moveRelative,
//         .argtable = &moveRelativeArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'run' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *dir;
//     struct arg_int *speed;
//     struct arg_end *end;
// } runArgs;

// static int run(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &runArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, runArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(runArgs.motor->ival[0] - 1);
//     MotorDirection_t direction = (MotorDirection_t)(runArgs.dir->ival[0]);
//     float speed = (float)(runArgs.speed->ival[0]);

//     StepperStandalone::run(motor, direction, speed);

//     return 0;
// }

// void StepperCLI::_registerRun(void)
// {
//     runArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     runArgs.dir     = arg_int1(NULL, NULL, "DIRECTION", "[0: Forward, 1: Reverse]");
//     runArgs.speed   = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
//     runArgs.end     = arg_end(4);

//     const esp_console_cmd_t cmd = {
//         .command = "run",
//         .help = "run",
//         .hint = NULL,
//         .func = &run,
//         .argtable = &runArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// /** 'homing' */

// static struct {
//     struct arg_int *motor;
//     struct arg_int *speed;
//     struct arg_end *end;
// } homingArgs;

// static int homing(int argc, char **argv)
// {
//     int nerrors = arg_parse(argc, argv, (void **) &homingArgs);
//     if (nerrors != 0) {
//         arg_print_errors(stderr, homingArgs.end, argv[0]);
//         return 1;
//     }

//     MotorNum_t motor = (MotorNum_t)(homingArgs.motor->ival[0] - 1);
//     float speed = (float)(homingArgs.speed->ival[0]);

//     StepperStandalone::homing(motor, speed);

//     return 0;
// }

// void StepperCLI::_registerHoming(void)
// {
//     homingArgs.motor    = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
//     homingArgs.speed    = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
//     homingArgs.end      = arg_end(3);

//     const esp_console_cmd_t cmd = {
//         .command = "homing",
//         .help = "homing",
//         .hint = NULL,
//         .func = &homing,
//         .argtable = &homingArgs
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
// }

// #endif
