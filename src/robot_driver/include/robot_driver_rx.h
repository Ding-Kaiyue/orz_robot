#ifndef __ROBOT_DRIVER_RX_H__
#define __ROBOT_DRIVER_RX_H__

#include <rclcpp/rclcpp.hpp>
#include <vector>
#include "can_msgs/msg/frame.hpp"
#include "ros2_socketcan/socket_can_id.hpp"
#include "ros2_socketcan/socket_can_receiver.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "robot_interfaces/msg/motor_fdb.hpp"

using namespace drivers::socketcan;

typedef struct
{
    uint8_t enable_flag = 0;
    uint8_t control_mode = 0;
    uint8_t limit_flag = 0;
    uint16_t temperature = 0.0f;
    float position = 0.0f;
    float velocity = 0.0f;
    float current = 0.0f;
    float vbus = 0.0f;
    uint32_t error_code = 0;
} Motor_Status_t;

class rx_package_t {
    private:
        uint8_t Motor_ID = 0;
        uint8_t rx_data[8] = {0};
        std::shared_ptr<Motor_Status_t> motor_status;

        enum DataFdbClass_e {
            DATA1 = 0x01,
            DATA2,
            DATA3,
            DATA4
        };
        union FloatUintConverter_u {
            float f;
            uint32_t u;
        };
        // 反馈接口类型
        typedef enum  {
            MOTOR_CTRL_RX = 0x00000100,
            FDB_REQ_RX = 0x00000300,
            FUN_CTRL_RX = 0x00000500,
            PAM_RW_RX = 0x00000700,
        } CommandID_Rx_e;

        typedef enum {
            MOTOR1_RET_ID = 0x301,
            MOTOR2_RET_ID ,
            MOTOR3_RET_ID ,
            MOTOR4_RET_ID ,
            MOTOR5_RET_ID ,
            MOTOR6_RET_ID ,
        } motor_ret_id_e;
    public:
        rx_package_t(CanId can_id, uint8_t *data, std::shared_ptr<Motor_Status_t> motor_status_) {
            this->motor_status = motor_status_;
            
            if (sizeof(data) == 8) {
                std::copy(data, data + 8, rx_data);
                Motor_ID = can_id.identifier() & 0x000000FF;

                switch (can_id.identifier() & 0xFFFFFF00) {
                    // 常用控制接口
                    case MOTOR_CTRL_RX: {
                        break;
                    }
                    // 请求反馈接口
                    case FDB_REQ_RX: {
                        if (rx_data[0] == DATA1) {              // data1
                            motor_status->enable_flag = rx_data[1];
                            motor_status->control_mode = rx_data[2];
                            uint32_t position_uint = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                            FloatUintConverter_u converter;
                            converter.u = position_uint;
                            motor_status->position = converter.f;
                        } else if (rx_data[0] == DATA2) {       // data2
                            motor_status->limit_flag = rx_data[1];
                            motor_status->temperature = (rx_data[2] << 8) | (rx_data[3] & 0xFF);
                            uint32_t velocity_uint = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                            FloatUintConverter_u converter;
                            converter.u = velocity_uint;
                            motor_status->velocity = converter.f;
                        } else if (rx_data[0] == DATA3) {       // data3
                            motor_status->vbus = (rx_data[2] << 8) | (rx_data[3] & 0xFF);
                            uint32_t current_uint = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                            FloatUintConverter_u converter;
                            converter.u = current_uint;
                            motor_status->current = converter.f;
                        } else if (rx_data[0] == DATA4) {       // data4
                            motor_status->error_code = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] & 0xFF);
                        }
                        break;
                    }
                    // 函数操作接口
                    case FUN_CTRL_RX: {
                        if (rx_data[1] == 1) {
                            std::cout << unsigned(rx_data[0]) << "%d operation done successfully!";
                        }
                        break;
                    }
                    // 参数读写接口
                    case PAM_RW_RX: {
                        if (rx_data[0] == 0x01) {       // Parameter read
                            if (rx_data[3] == 0x01) {   // The parameter is int
                                std::cout << "Read " << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %d" 
                                          << ((rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF));
                            } else if (rx_data[3] == 0x02) {   // The parameter is float
                                FloatUintConverter_u converter;
                                converter.u = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF);
                                std::cout << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %f" << converter.f;
                            }
                        } else if (rx_data[0] == 0x02) {        // Parameter write
                            if (rx_data[3] == 0x01) {   // The parameter is int
                                std::cout << "Write " << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %d" 
                                          << ((rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF));
                            } else if (rx_data[3] == 0x02) {   // The parameter is float
                                FloatUintConverter_u converter;
                                converter.u = (rx_data[4] << 24) | (rx_data[5] << 16) | (rx_data[6] << 8) | (rx_data[7] && 0xFF);
                                std::cout << unsigned((rx_data[1] << 8) | rx_data[2] & 0xFF) << "%d parameter is %f" << converter.f;
                            }
                        }
                        break;
                    }
                    default: 
                        break;
                }
            }
        }
};
#endif