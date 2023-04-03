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

static QueueHandle_t message_queue = NULL;

// Task_1 Increase counter
void Task1_SendCounter(void *arg)
{
    int counter = 0;
  	int previous_time = 0;
    int time_between_call_task = 0;	

    while (1)
    {
        counter++;
	//	ESP_LOGI(TAG1, "Counter: %d", counter);
        time_between_call_task = xTaskGetTickCount() - previous_time;
        ESP_LOGI(TAG1, "Counter: %d Time since last call task: %d ms", counter, time_between_call_task);
        // Send counter to the queue
        if (xQueueSend(message_queue, &counter, 0) != pdTRUE)
        {
            ESP_LOGE(TAG1, "Failed to send message to queue");
        }

        previous_time = xTaskGetTickCount();
        // 5 sec delay
        //vTaskDelay(pdMS_TO_TICKS(5000));
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// Task_2 Receive message from queue and log into Terminal
void Task2_ReceiveCounter(void *arg)
{
	int message = 0;
   	int previous_message = 0;
    int time_between_messages = 0;	

	while(1)
	{
		xQueueReceive(message_queue, &message, portMAX_DELAY);
		time_between_messages = xTaskGetTickCount() - previous_message;
        ESP_LOGI(TAG2, "Receive: %d Time since previous message: %d ms", message, time_between_messages);
//        ESP_LOGI(TAG2, "Message: %d", message);
        previous_message = xTaskGetTickCount();
	}
}

void app_main(void)
{
    gpio_set_direction(LED_PIN ,GPIO_MODE_OUTPUT); // Use build in Led 
    
    message_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(&Task1_SendCounter, "Task1_SendCounter", 2048, message_queue, 5, NULL);
    xTaskCreate(&Task2_ReceiveCounter, "Task2_ReceiveCounter", 2048, message_queue, 3, NULL);

    while (1)
    {
        led_status = !led_status;
        gpio_set_level(LED_PIN, led_status);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
