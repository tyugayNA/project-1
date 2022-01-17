/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "led.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_OF_MSG_FROM_UART	5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern led_parameters xLedRed;
extern led_parameters xLedGreen;
extern led_parameters xLedBlue;
extern led_parameters xLedOrange;
xSemaphoreHandle xCountingButtonSemaphore;
xSemaphoreHandle xCountingUARTReceiveMsgSemaphore;
extern xQueueHandle xQueueUARTMsg;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void blinkLedHandle(void *argument);
void ledTask(void *argument);
void buttonHandle(void *arg);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	xCountingButtonSemaphore = xSemaphoreCreateCounting(4,0);
	xCountingUARTReceiveMsgSemaphore = xSemaphoreCreateCounting(5, 0);
	xQueueUARTMsg = xQueueCreate(NUM_OF_MSG_FROM_UART, sizeof(xUART_buf));
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  //xTaskCreate(blinkLedHandle, (char*)"blinkRed", 128, (void*)&xLedRed, osPriorityNormal, NULL);
  //xTaskCreate(blinkLedHandle, (char*)"blinkGreen", 128, (void*)&xLedGreen, osPriorityNormal, NULL);
  //xTaskCreate(blinkLedHandle, (char*)"blinkBlue", 128, (void*)&xLedBlue, osPriorityNormal, NULL);

  xTaskCreate(ledTask, (char*)"blinkRed", 128, (void*)&xLedRed, osPriorityNormal, NULL);
  xTaskCreate(ledTask, (char*)"blinkGreen", 128, (void*)&xLedGreen, osPriorityNormal, NULL);
  xTaskCreate(ledTask, (char*)"blinkBlue", 128, (void*)&xLedBlue, osPriorityNormal, NULL);
  xTaskCreate(ledTask, (char*)"blinkBlue", 128, (void*)&xLedOrange, osPriorityNormal, NULL);
  xTaskCreate(buttonHandle, (char*)"buttonHandle", 128, NULL, osPriorityNormal, NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void blinkLedHandle(void *pvParameters) {
	volatile led_parameters *xLedParameters;
	xLedParameters = (led_parameters*)pvParameters;
	if (xLedParameters->count == 0) {
		while(1) {
			HAL_GPIO_TogglePin(xLedParameters->port, xLedParameters->pin);
			vTaskDelay((TickType_t)(xLedParameters->period));
		}
	}
	while(1) {
		if ((xLedParameters->count) != 0) {
			(xLedParameters->count)--;
			HAL_GPIO_WritePin(xLedParameters->port, xLedParameters->pin, GPIO_PIN_SET);
			vTaskDelay((TickType_t)(xLedParameters->period));
			HAL_GPIO_WritePin(xLedParameters->port, xLedParameters->pin, GPIO_PIN_RESET);
			vTaskDelay((TickType_t)(xLedParameters->period));
		}
	}
	vTaskDelete(NULL);
}

void ledTask(void *pvParameters) {
	volatile led_parameters *xLedParameters;
	xLedParameters = (led_parameters*)pvParameters;
	while(1) {
		xSemaphoreTake(xCountingButtonSemaphore, portMAX_DELAY);
		HAL_GPIO_WritePin(xLedParameters->port, xLedParameters->pin, GPIO_PIN_SET);
		vTaskDelay((TickType_t)(xLedParameters->period));
		HAL_GPIO_WritePin(xLedParameters->port, xLedParameters->pin, GPIO_PIN_RESET);
	}
	vTaskDelete(NULL);
}

void buttonHandle(void *param) {
	while(1) {
		if (HAL_GPIO_ReadPin(button_GPIO_Port, button_Pin) == GPIO_PIN_SET) {
			vTaskDelay(75);
			if (HAL_GPIO_ReadPin(button_GPIO_Port, button_Pin) == GPIO_PIN_RESET) {
				xSemaphoreGive(xCountingButtonSemaphore);
			}
		}
	}
}
/* USER CODE END Application */
