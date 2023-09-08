#include "../include/gripper.hpp"

Gripper::Gripper(GPIO_PIN servo_pwm_pin, GPIO_PIN feedback_pin, ADC_HandleTypeDef *adc, TIM_HandleTypeDef *tim) {
    this->servo_pwm_pin = servo_pwm_pin;
    this->feedback_pin = feedback_pin;

    this->tim = tim;

    // Enable timer
    HAL_TIM_Base_Start_IT(tim);
}
operation_status Gripper::initGripper() {
    enableGripper();
    rawSetPosition(0);
    position = 0;
    set_position = 0;
    read_position = readCurrentPosition();
    return operation_status_init(gripper, success, 0x00);
}

operation_status Gripper::enableGripper() {
    HAL_TIM_PWM_Start(tim, TIM_CHANNEL_1);
    return operation_status_init(gripper, success, 0x00);
}

operation_status Gripper::disableGripper() {
    HAL_TIM_PWM_Stop(tim, TIM_CHANNEL_1);
    return operation_status_init(gripper, success, 0x00);
}

void Gripper::rawSetPosition(float desired_position) {
    position_ms = lroundf((1.71f*desired_position/100.f + 0.69f)*1000);
    __HAL_TIM_SET_COMPARE(tim, TIM_CHANNEL_1, position_ms);
}

operation_status Gripper::setPosition(float new_position) {
    rawSetPosition(new_position);
    return operation_status_init(gripper, success, 0x00);
}

operation_status Gripper::getGripperStatus() {

}

float Gripper::getCurrentPosition() {
    return this->position;
}

float Gripper::readCurrentPosition() {
    // 0.5 V - fully open
    // 2.2 V - fully closed
    // From Pololu documentaion, but to be checked
    // y - percentage of gripper closed
    // x - measured voltage
    // y = (x - 0.69)/(2.33 0.69) * 100
    float sum = 0;

    for(uint8_t i = 0; i < 100; i++){
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        uint32_t value = HAL_ADC_GetValue(&hadc1);
        float measured_voltage = 3.3f * (float)value / 4096.0f;

        sum += (measured_voltage - 0.69f)/(2.33f - 0.69f) * 100;
    }
    read_position = sum / 100.f;
    return read_position;
}