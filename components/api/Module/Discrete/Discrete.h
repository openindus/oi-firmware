/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Discrete.h
 * @brief Functions for Discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "DiscreteStandalone.h"
#include "DiscreteSlave.h"
#include "DiscreteControl.h"
#include "DiscreteCLI.h"

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_STANDALONE)
#define Discrete DiscreteStandalone
#elif (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && !defined(MODULE_STANDALONE)
#define Discrete DiscreteSlave
#else 
#define Discrete DiscreteControl
#endif
