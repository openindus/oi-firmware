/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessInterface.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "BrushlessStandalone.h"
#include "BrushlessSlave.h"
#include "BrushlessControl.h"

#if defined(OI_BRUSHLESS) && defined(MODULE_STANDALONE)
#define Brushless BrushlessStandalone
#elif defined(OI_BRUSHLESS) && !defined(MODULE_STANDALONE)
#define Brushless BrushlessSlave
#else 
#define Brushless BrushlessControl
#endif