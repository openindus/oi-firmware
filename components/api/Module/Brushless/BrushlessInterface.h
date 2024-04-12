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

#include "Global.h"

class BrushlessInterface
{
public:

    /**
     * @brief Set the speed of the brushless motor 
     * The argument is the value of the speed to apply (need to be between 0 and 510 because of ledc init parameter, 0 = 0% and 510 = 100%)
     * 
     * @param speed speed to apply (need to be between 0 and 510 because of ledc init parameter, 0 = 0% and 510 = 100%)
     */
    virtual void setSpeed(uint32_t speed) = 0;

    /**
     * @brief Set the brake of the brushless motor 
     * The argument is the state of the brake (On or Off)
     * 
     * @param brake boolean value for the status of the brake
     */
    virtual void setBrake(bool brake);

    /**
     * @brief Set the direction of the brushless motor 
     * The argument is the state of the direction (Clockwise or Anti-clockwise)
     * 
     * @param direction boolean value for the status of the direction
     */
    virtual void setDirection(bool direction);

    /**
     * @brief Get the speed of the brushless motor with BLDC driver signal
     * 
     * @return float speed in rpm
     */
    virtual float getSpeed(void);

    /**
     * @brief Get the direction of the brushless motor 
     * 
     * @return bool direction (clockwise revolution = 1 or anti-clockwise revolution = 0)
     */
    virtual bool getDirection(void);

    /**
     * @brief Get the speed of the brushless motor with encoder reading
     * 
     * @return float speed in rpm
     */
    virtual float getSpeedEncoder(void);

    /**
     * @brief Get the position of the brushless motor 
     * 
     * @return float a position in radians
     */
    virtual float getPosition(void);
};
