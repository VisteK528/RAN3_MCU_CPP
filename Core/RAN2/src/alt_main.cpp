#include "../include/alt_main.hpp"
#include "../include/robot.hpp"
#include "../include/read_gcode.h"
#include <string>
#include <cstring>

#include "../include/algorithm6dof.hpp"

#define LINE_MAX_LENGTH	80


static char line_buffer[LINE_MAX_LENGTH + 1];
static uint32_t line_length;

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
        // odebraliśmy znak końca linii
        if (line_length > 0) {
            // jeśli bufor nie jest pusty to dodajemy 0 na końcu linii
            line_buffer[line_length] = '\0';
            // przetwarzamy dane
            // zaczynamy zbieranie danych od nowa
            line_length = 0;
            return 0;
        }
    }
    else {
        if (line_length >= LINE_MAX_LENGTH) {
            // za dużo danych, usuwamy wszystko co odebraliśmy dotychczas
            line_length = 0;
        }
        // dopisujemy wartość do bufora
        if((uint8_t)value == 8 && line_length > 0){
            line_buffer[--line_length] = 0;
        }
        else{
            line_buffer[line_length++] = value;
        }
    }
    return 1;
}

int alt_main(){
    printf("RAN2 Software MCU©\n");
    printf("Starting...\n");

    Robot my_robot = buildRobot();
    my_robot.home();
    //my_robot.move2Default();

    LINK_MAP map = {
            {BASE_HEIGHT, 3.f},
            {SHOULDER_HEIGHT, 12.f},
            {SHOULDER_LENGTH, 20.76355f},
            {ELBOW_LENGTH, 16.50985f},
            {EE_LENGTH, 5},
    };

    float offsets[6] = {0, 0, 0, 0, 0, 0};
    float angles[6];

    Algorithm6Dof algorithm(map, offsets);

    float rot_mat_d[9] = {
            0, 1, 0,
            1, 0, 0,
            0, 0, 1
    };

    matrix_f32 rot_mat;
    matrix_init_f32(&rot_mat, 3, 3, rot_mat_d);

    while (1)
    {
        uint8_t uart_value;
        if (HAL_UART_Receive(&huart2, &uart_value, 1, 0) == HAL_OK){
            if(line_append(uart_value) == 0){
                // Check which command to choose
                char letter;
                float value;
                uint16_t counter = 0;
                uint8_t result;
                while((result = parseMessage(&letter, &value, line_buffer, &counter)) == 1) {
                    if(letter == 'G'){
                        if((int)value == 28){
                            if(result == 1){
                                while(parseMessage(&letter, &value, line_buffer, &counter) == 1){
                                    if(letter == 'J' && value >= 1 && value < 7){
                                        my_robot.homeJoint((int)value - 1);
                                    }
                                }
                            }
                            else{
                                my_robot.home();
                            }

                        }
                        else if((int)value == 1){
                            float values[3];
                            int i = 0;
                            while(parseMessage(&letter, &value, line_buffer, &counter) == 1){
                                values[i] = value;
                                i++;
                            }

                            algorithm.inverseKinematics(values[0], values[1], values[2], &rot_mat, angles);
                            for(int i = 0; i < 6; i++){
                                printf("Theta%d: %f\n", i, rad2Deg(angles[i]));
                            }

                            coordinates points[6];

                            algorithm.forwardKinematics(angles, points);

                            for(int i = 0; i < 6; i++){
                                printf("Point: %d\tX: %f\tY: %f\tZ:%f:\n", i, points[i].x, points[i].y, points[i].z);
                            }
                        }
                        else if((int)value == 2){
                            for(int i = 0; i < 1000; i++){
                                algorithm.inverseKinematics(15, 10.f+(float)i/100.f, 20, &rot_mat, angles);

                                angles[1] = 180.f - angles[1];
                                angles[2] = angles[2] - 50.3;
                                angles[3] = 180.f - angles[3];
                                angles[4] = angles[4] - 45.f;

                                for(int j = 0; j < 6; j++){
                                    my_robot.moveJoint(j, angles[j]);
                                }
                            }
                        }
                    }
                    else if(letter == 'N'){

                    }
                    else if(letter == 'M'){
                        if(int(value) == 18){
                            if(result == 1){
                                while(parseMessage(&letter, &value, line_buffer, &counter) == 1){
                                    if(letter == 'J' && value >= 1 && value < 7){
                                        my_robot.disableJoint((int)value - 1);
                                    }
                                }
                            }
                            else{
                                my_robot.disableJoints();
                            }
                        }
                    }
                    else if(letter == 'J'){
                        uint8_t joint_number = (uint8_t)value;
                        parseMessage(&letter, &value, line_buffer, &counter);
                        if(letter == 'P'){
                            my_robot.moveJoint(joint_number-1, value);
                        }
                    }
                }
                memset(line_buffer, 0, 80);
            }

            if(uart_value != '\0' && line_length >= 0){
                printf("Command: %s\n", line_buffer);
            }

            fflush(stdin);
            fflush(stdout);
        }
    }
}