#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG1 = "Task 1 -> ";
static const char *TAG2 = "Task 2 -> ";

QueueHandle_t message_queue;

// Task_1 Increase counter
void Task_1(void *pvParam)
{
    int counter = 0;
    while (1)
    {
        counter++;
	//	ESP_LOGI(TAG1, "Counter: %d", counter);

        // Send counter to the queue
        if (xQueueSend(message_queue, &counter, 0) != pdTRUE)
        {
            ESP_LOGE(TAG1, "Failed to send message to queue");
        }

        // 5 sec delay
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Task_2 Log message from queue
void Task_2(void *pvParam)
{
	int message = 0;
	while(1)
	{
		xQueueReceive(message_queue, &message, 3050); // portMAX_DELAY
		ESP_LOGI(TAG2, "Message: %d", message);
	}
}

void app_main(void)
{
    message_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(&Task_1, "Task_1", 2048, message_queue, 5, NULL);
    xTaskCreate(&Task_2, "Task_2", 2048, message_queue, 5, NULL);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
