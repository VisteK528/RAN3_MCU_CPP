#ifndef RAN2_MCU_CPP_MAGNETIC_ENCODER_HPP
#define RAN2_MCU_CPP_MAGNETIC_ENCODER_HPP

#include "as5600.hpp"
#include "errors.hpp"

/*  Operation_status information
 *
 *  Module codes used in this module:
 *  - Encoders (Numbers from 1 to 6)    0x0f - 0x14
 *
 *  Operation result codes:
 *  Result                                  Code
 *  Operation ended successfully            0x00
 *  Operation continue                      0x01
 *
 *  Connection cannot be established        0x02
 *  Magnet too weak                         0x03
 *  Magnet too strong                       0x04
 *
 * */

class MagneticEncoder{
public:
    MagneticEncoder(uint8_t encoderNumber, uint8_t encoderAddress, uint8_t channelNumber, I2C_HandleTypeDef* i2c, float homingPosition, float degPerRotation);

    operation_status checkEncoder();

    /** @brief Checks if the raw position of the encoder is at homing position with given tolerance
     *  @attention Be mindful that the minimum tolerance (in degrees) must be at least equal to the value of one step
     *  eg. If the step value of the motor is 1.8 deg then the minimum tolerance is 1.8 deg.
     *  @returns Returns status of the homing procedure
     * */
    bool homeEncoder();

    /** @brief Returns true if the encoder is homed, otherwise returns false.
     * */
    bool isHomed();

    /** @brief Updates position values (current position, totalAngle, rawAngle).
     *  @returns None
     * */
    void updatePosition();

    /** @brief Returns current position of the encoder, always between 0 and 360 degrees.
     * */
    float getPosition();

    /** @brief Returns total angle of movement of the encoder, may be negative.
     * */
    float getTotalAngle();

    // TODO To be done along with the getAcceleration function
    /** @brief Returns current velocity measured by the encoder, units not selected yet.
     * */
    float getVelocity();
    /** @brief Returns current acceleration measured by the encoder, units not selected yet.
     * */
    float getAcceleration();

    /** @brief Returns current homing position angle related to the raw angle.
     * */
    float getHomingPosition();

    /** @brief Sets new homing position related to the raw angle.
     *  @param homingPosition New homing position related to the raw angle, given in degrees.
     *  @returns None
     * */
    void setHomingPosition(float homingPosition);

    /** @brief If the encoder is placed on the joint with gear ratio other than 1, then one rotation of the joint is not
     *  equal to one rotation measured by the encoder on the motor. Thus the degPerRotation variable was created,
     *  which means how many rotations measured by the encoder are equal to one rotation of the joint.
     *  This function returns this value.
     * */
    float getDegPerRotation();


private:
    // Checks in which quadrant the encoder is currently. Then updates the total angle value.
    void checkQuadrant();

    float totalAngle;
    float rawAngle;
    float currentPosition;
    float oldPosition;
    float degPerRotation;

    int turns;
    float offset;
    bool homed;

    unsigned char quadrant;
    unsigned char previousQuadrant;

    float velocity;
    float acceleration;

    float homingPosition;

    uint8_t encoderNumber;
    uint8_t channelNumber;
    uint8_t encoderAddress;
    I2C_HandleTypeDef* i2c;
};

#endif //RAN2_MCU_CPP_MAGNETIC_ENCODER_HPP