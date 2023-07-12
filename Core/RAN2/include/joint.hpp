#ifndef RAN2_MCU_CPP_JOINT_HPP
#define RAN2_MCU_CPP_JOINT_HPP

#include "driver.hpp"
#include "movement.hpp"
#include "sensors.hpp"
#include <iostream>
#include <memory>
#include "../../Inc/gpio.h"

using namespace drivers;

class Joint{
public:
    Joint(std::unique_ptr<Driver> driver, std::shared_ptr<Endstop> sensor, uint16_t gear_teeth, DIRECTION homing_direction);
    void setHomingSteps(uint16_t homing_steps);
    void setHomingVelocity(float homing_velocity);
    void setHomingAcceleration(float homing_acceleration);
    void setMaxVelocity(float max_velocity);
    void setMaxAcceleration(float max_acceleration);
    void setMinPosition(float min_position);
    void setMaxPosition(float max_position);
    void setOffset(float offset);
    void setBaseAngle(float base_angle);

    void move2Pos(float position);
    void homeJoint();
private:
    unsigned int degrees2Steps(float degrees);
    void moveBySteps(unsigned int steps, DIRECTION direction, float max_velocity, float max_acceleration);
    std::unique_ptr<Driver> driver;
    std::shared_ptr<Endstop> endstop;

    // Variables
    float min_pos = 0;
    float max_pos = 360;
    float joint_position = 0;
    float offset = 0;
    float base_angle = 0;

    // Homing Variables
    DIRECTION homing_direction;
    float homing_velocity = 0.25;
    float homing_acceleration = 0.5;
    unsigned int homing_steps = 200;

    // Movement variables
    float max_velocity = 0.7;
    float max_acceleration = 0.75;

    Movement movement;

    uint16_t gear_teeth;
    bool homed = false;
};

#endif // RAN2_MCU_CPP_DRIVER_HPP