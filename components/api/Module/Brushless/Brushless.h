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

#include "BrushlessStandalone.h"
#include "BrushlessSlave.h"
#include "BrushlessControl.h"

#if defined(CONFIG_BRUSHLESS)
#if defined(CONFIG_MODULE_STANDALONE)
class Brushless : public BrushlessStandalone {};
#else
class Brushless : public BrushlessSlave {};
#endif
#else
 class Brushless : public BrushlessControl {};
#endif