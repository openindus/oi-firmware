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
#include "CoreCLI.h"

#if defined(OI_CORE) && defined(MODULE_STANDALONE)
#define Core CoreStandalone
#elif defined(OI_CORE) && !defined(MODULE_STANDALONE)
#define Core CoreMaster
#endif