/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Brushless.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "AnaloglsStandalone.h"
#include "AnaloglsSlave.h"
#include "AnaloglsControl.h"

#if defined(OI_ANALOG_LS)

#if defined(MODULE_STANDALONE)
class Analogls : public AnaloglsStandalone {};
#else 
class Analogls : public AnaloglsSlave {};
#endif

#else
class Analogls : public AnaloglsControl {};

#endif