// #include <stdio.h>
// #include<string.h>
// #include<stdlib.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/uart.h"
// #include "driver/gpio.h"
// #include "esp_timer.h"
// #include "esp_log.h"

// #define ECHO_TEST_TXD 17
// #define ECHO_TEST_RXD 16
// #define ECHO_TEST_RTS UART_PIN_NO_CHANGE
// #define ECHO_TEST_CTS UART_PIN_NO_CHANGE

// #define ECHO_UART_PORT_NUM 2
// #define ECHO_UART_BAUD_RATE 9600
// #define ECHO_TASK_STACK_SIZE 30

// #define BUF_SIZE (1024)

// //Lọc dữ liệu
// void lon_lat(char *str){
//     //Tách lấy chuỗi $GPRMC
//     char *token;
//     token=strtok(str,"\n");
//     while (token!=NULL)
//     {
//         if(token[0]=='$'&&token[1]=='G'&&token[2]=='P'&&token[3]=='R'&&token[4]=='M'&&token[5]=='C'){
//             break;
//         }
//         token=strtok(NULL,"\n");
//     }
//     //Tách lấy phần longitude,latitude
//    char *tok;
//     char latitude[20];
//     char longitude[20];
//     tok = strtok(token, ",");
//     int field = 0;
//     while (tok != NULL) {
//         field++;
//         if (field == 4) {
//             strcpy(latitude, tok);
//         }
//         if (field == 6) {
//             strcpy(longitude, tok);
//         }
//         tok = strtok(NULL, ",");
//     }
//     //Chuẩn hóa về độ
//     double lon,lat;
//     char x[10],y[10],x1[10],y1[10];
//     lon = atof(strncpy(x, longitude, strlen(longitude) - 8)) + atof(strcpy(y, longitude + strlen(longitude) - 8))/60;
//     lat = atof(strncpy(x1, latitude, strlen(latitude) - 8)) + atof(strcpy(y1, latitude + strlen(latitude) - 8))/60;
//     ESP_LOGI("Longitude: ","%lf",lon);
//     ESP_LOGI("Latitude: ","%lf",lat);
// }

//  void init_uart2(void)
// {
//     // Config uart2
//     uart_config_t uart_config0= {
//         .baud_rate = ECHO_UART_BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .source_clk = UART_SCLK_DEFAULT,
//     };
//     uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
//     uart_param_config(ECHO_UART_PORT_NUM, &uart_config0);
//     uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
// }
// void echo_task(void *arg){
//     init_uart2();
//     char *data = (char *)malloc(BUF_SIZE);
//     while (true)
//     {
//         int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 1000 / portTICK_PERIOD_MS);
//         if (len > 0)
//         {
//             lon_lat(data);
//         }
//         vTaskDelay(1000/portTICK_PERIOD_MS);
//     }
// }
// void app_main(void)
// {
//     xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE*1024, NULL, 10, NULL);
// }