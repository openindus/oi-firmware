/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Core.h
 * @brief Functions for Core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "CoreStandalone.h"
#include "CoreMaster.h"

#if defined(CONFIG_CORE)

#if defined(CONFIG_MODULE_STANDALONE)
class Core : public CoreStandalone {};
#else 
class Core : public CoreMaster {};
#endif

#endif