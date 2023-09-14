#ifndef RAN2_MCU_CPP_JOINT_HPP
#define RAN2_MCU_CPP_JOINT_HPP

#include "driver.hpp"
#include "sensors.hpp"
#include <iostream>
#include <memory>
#include "gpio.h"
#include "magnetic_encoder.hpp"

/*  Operation_status information
 *
 *  Module codes used in this module:
 *  - Joints (Numbers from 1 to 6)    0x09 - 0x0e
 *
 *  Operation result codes:
 *  Result                                                                              Code
 *  Operation ended successfully                                                        0x00
 *  Operation continue                                                                  0x01
 *
 *  Encoder present, but degPerRotation is not equal to 1                               0x02
 *  Endstop and encoder not present, homing impossible                                  0x03
 *  Endstop not present, homing by endstop impossible                                   0x04
 *  Encoder not present                                                                 0x05
 *
 *  Movement not possible, joint is not homed                                           0x06
 *  Homing not possible, joint motor is disabled                                        0x07
 *  Setting smart encoder homing not possible, encoder not available                    0x08
 *  Disabling smart encoder homing not possible, smart encoder homing hasn't been set   0x09
 *  Movement not possible, goal position is not within movement range                   0x0a
 *  Safeguard stop triggered                                                            0x0b
 ** */

using namespace drivers;

class Joint{
public:
    Joint(uint8_t number, std::unique_ptr<Driver>& driver, uint16_t gear_teeth, DIRECTION homing_direction, std::shared_ptr<Endstop> sensor = nullptr, std::shared_ptr<MagneticEncoder> encoder = nullptr);

    // Setters
    void setHomingSteps(uint16_t homing_steps);
    void setHomingVelocity(float homing_velocity);
    void setHomingAcceleration(float homing_acceleration);
    void setMaxVelocity(float max_velocity);
    void setMaxAcceleration(float max_acceleration);
    void setMinPosition(float min_position);
    void setMaxPosition(float max_position);
    void setOffset(float offset);

    // Movement functions
    float getMinDelay(float max_speed);
    float getStartDelay(float acceleration) const;
    float motorVel(float phase_time);
    float phaseTime(float joint_ang_vel);
    float jointVelFromMotorVel(float motor_ang_vel);
    float motorVelFromJointVel(float joint_ang_vel);
    float getGearSpeedRatio() const;

    operation_status disableMotor();
    operation_status enableMotor();
    bool isMotorEnabled();
    bool isMoving();

    operation_status move2Pos(float position, bool blocking);
    operation_status homeJoint();

    bool encoderAvailable();
    operation_status setEncoderHoming();
    operation_status setEndstopHoming();
    operation_status setSmartEncoderHoming();
    operation_status disableSmartEncoderHoming();
    operation_status updateEncoder();
    operation_status getEncoderData(MagneticEncoderData* data);
    operation_status getJointPosition(float* position);
    operation_status isMovementPossible(float position) const;

    operation_status checkJointStatus();
    operation_status getJointStatus();
    
    operation_status stopJoint();
    operation_status startJoint();

private:
    unsigned int degrees2Steps(float degrees);
    void accelerateJoint(DIRECTION direction, float velocity, float acceleration);
    void moveJoint(DIRECTION direction, float velocity);
    void moveJointBySteps(unsigned int steps, DIRECTION direction, float max_velocity, float max_acceleration, bool blocking);
    std::unique_ptr<Driver> driver;
    std::shared_ptr<Endstop> endstop;
    std::shared_ptr<MagneticEncoder> encoder;

    // Variables
    float min_pos = 0;
    float max_pos = 360;
    float joint_position = 0;
    float offset = 0;

    uint8_t joint_number;

    float motor_step;
    uint16_t driver_microstep;
    float one_pulse_step;

    uint16_t motor_shaft_gear_teeth;
    uint16_t joint_gear_teeth;

    float speed_gear_ratio;
    float torque_gear_ratio;

    // Homing Variables
    DIRECTION homing_direction;
    float homing_velocity = 0.25;
    float homing_acceleration = 0.5;
    unsigned int homing_steps = 200;

    // Movement variables
    float max_velocity = 0.7;
    float max_acceleration = 0.75;

    uint16_t gear_teeth;
    bool homed = false;
    bool endstop_homing = true;
    bool encoder_homing = false;
    bool smart_encoder_homing = false;
    bool safeguard_stop = false;

    operation_status joint_status;
};

#endif // RAN2_MCU_CPP_DRIVER_HPP