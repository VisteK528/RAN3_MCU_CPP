#include "alt_main.hpp"
#include "include/robot_build.hpp"
#include <cstring>
#include <string>
#include <iomanip>
#include "spi.h"

#define LINE_MAX_LENGTH	80

static char line_buffer[LINE_MAX_LENGTH + 1];
static uint32_t line_length;
bool start = false;

operation_status robot_operation_status;
operation_status background_robot_operation_status;

Robot my_robot;
coordinates coords[6] = {};
std::string coords_header;
std::string coords_data;
uint16_t counter = 0;

int __io_putchar(int ch)
{
    if (ch == '\n') {
        uint8_t ch2 = '\r';
        HAL_UART_Transmit(&huart2, &ch2, 1, HAL_MAX_DELAY);
    }

    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return 1;
}

uint8_t line_append(uint8_t value)
{
    if (value == '\r' || value == '\n') {
        if (line_length > 0) {
            line_buffer[line_length] = '\0';
            line_length = 0;
            return 0;
        }
    }
    else {
        if (line_length >= LINE_MAX_LENGTH) {
            line_length = 0;
        }
        if(((uint8_t)value == 8  || (uint8_t)value == 127 ) && line_length > 0){
            line_buffer[--line_length] = 0;
        }
        else{
            line_buffer[line_length++] = value;
        }
    }
    return 1;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi2)
    {

    }
}

int alt_main() {
    HAL_TIM_Base_Start_IT(&htim1);

    my_robot = buildRobot();
    robot_operation_status = my_robot.systemsCheck(true);
    start = true;

    SDP_Message message;
    HAL_StatusTypeDef status;
    std::string response;

    while (1) {
        status = SDP_Receive(&message, 1000);
        if(status == HAL_OK){

            std::vector<std::string> commands = {};
            commands = splitCommandFile(&message);
            for(auto& command: commands){

                // Get ready status
                robot_operation_status.result = in_progress;


                // Execute command
                response.clear();
                robot_operation_status = executeGCODE(my_robot, response, command.c_str());

                // Add command result
                if(robot_operation_status.result == success) {
                    response.insert(0, "0\t");
                } else {
                    response.insert(0, "1\t");
                }
                response += "\r\n";

                // Send response
                SDP_Transmit((uint8_t*)response.c_str(), response.length());


                // Getting coordinates of End Effector from robot and then printing them on display, to be optimised
                my_robot.getRobotArmCoordinates(coords);

                HAL_Delay(1000);

            }
            fflush(stdin);
            fflush(stdout);

        }

    }

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim != &htim1){
        DriverHandleCallback(htim);
    }
    else if(start && !my_robot.getMovement()){
        if(my_robot.systemsCheck(false).result == success){
            background_robot_operation_status = my_robot.updateEncoders();
            if(background_robot_operation_status.result == failure){
            }
        }
    }

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // GPIO_PIN = 4 / PC_2 GPIO EXT2
    if(GPIO_Pin == 4){
        my_robot.safeguardTrigger();
        counter++;
    }
}