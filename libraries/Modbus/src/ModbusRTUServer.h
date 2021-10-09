/*
  This file is part of the ArduinoModbus library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _MODBUS_RTU_SERVER_H_INCLUDED
#define _MODBUS_RTU_SERVER_H_INCLUDED

#include "ModbusServer.h"
#ifdef ARDUINO_RS485
#include <ArduinoRS485.h>
#else
#include "OIRS.h"
#endif

class ModbusRTUServerClass : public ModbusServer {
public:
  ModbusRTUServerClass();
  #ifdef ARDUINO_RS485
  ModbusRTUServerClass(RS485Class& rs485);
  #else
  ModbusRTUServerClass(OIRS& rs485);
  #endif
  virtual ~ModbusRTUServerClass();

  /**
   * Start the Modbus RTU server with the specified parameters
   *
   * @param id (slave) id of the server
   * @param baudrate Baud rate to use
   * @param config serial config. to use defaults to SERIAL_8N1
   *
   * Return 1 on success, 0 on failure
   */
  int begin(int id, unsigned long baudrate, uint32_t config = SERIAL_8N1);
  #ifdef ARDUINO_RS485
  int begin(RS485Class& rs485, int id, unsigned long baudrate, uint32_t config = SERIAL_8N1);
  #else
  int begin(OIRS& rs485, int id, unsigned long baudrate, uint32_t config = SERIAL_8N1);
  #endif

  /**
   * Poll interface for requests
   */
  virtual void poll();

private:
  #ifdef ARDUINO_RS485
  RS485Class* _rs485 = &RS485;
  #else
  OIRS* _rs485 = &RS485;
  #endif

};

extern ModbusRTUServerClass ModbusRTUServer;

#endif
