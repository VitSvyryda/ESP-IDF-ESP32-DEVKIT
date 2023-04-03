#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define LED_PIN  2 // Use build in Led 
bool led_status = false;

static const char *TAG1 = "Task 1";
static const char *TAG2 = "Task 2";

// Task_1 Increase counter
static void Task1_SendCounter(void *arg)
{
    QueueHandle_t message_queue = (QueueHandle_t) arg;
    int counter = 0;

    while (1)
    {
        counter++;
        // Send counter to the queue
        if (xQueueSend(message_queue, &counter, 0) != pdTRUE)
        {
            ESP_LOGE(TAG1, "Failed to send message to queue");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS); // 5 sec delay
    }
}

// Task_2 Receive message from queue and log into Terminal
static void Task2_ReceiveCounter(void *arg)
{
    QueueHandle_t message_queue = (QueueHandle_t) arg;
    int message = 0;
    int time = 0;
    int time0 = 0;
	while(1)
	{
		xQueueReceive(message_queue, &message, portMAX_DELAY);
        
        time = (xTaskGetTickCount() - time0) * portTICK_PERIOD_MS;
        ESP_LOGI(TAG2, "Message: %d | Time between messages: %d ms", message, time);
        time0 = xTaskGetTickCount();
	}
}

void app_main(void)
{
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT); // Use build in Led 
    
    QueueHandle_t message_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(&Task1_SendCounter, "Task1_SendCounter", 2048, message_queue, 5, NULL);
    xTaskCreate(&Task2_ReceiveCounter, "Task2_ReceiveCounter", 2048, message_queue, 5, NULL);

    while (1)
    {
        led_status = !led_status;
        gpio_set_level(LED_PIN, led_status);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
