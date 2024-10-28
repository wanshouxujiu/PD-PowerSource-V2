/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_GeneralTim.h"
#include "task.h"
#include "bsp_SIQ02FVS3.h"
#include "bsp_key.h"
#include "KeyInputFsm.h"


uint8_t TaskTimeCount;


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/**
  * @brief  ͨ�ö�ʱ��3��TIM4�����жϣ����ڶ���״̬�ļ����Լ��������ļ���
  * @param  None
  * @retval None
  */
void GENERAL_TIM_IRQHandler_3(void)
{
  if(TIM_GetITStatus(GENERAL_TIM_3, TIM_IT_Update) != RESET)    //ȷ��ʹ�ܲ��Ҳ������ж�
	{
    if(ShowTimeCount < 255)
    {
      ShowTimeCount++;
    }

    if(TaskTimeCount < 255)
    {
      TaskTimeCount++;
    }

    if(NormalCount < 255)
    {
      NormalCount++;
    }

    if(KeyLongCount < 255)
    {
      KeyLongCount++;
    }

    if(SlowTaskCount < 65535)
    {
      SlowTaskCount++;
    }

    if(ExtinguishTimeCountFlag == 1)
    {
      if(ExtinguishTimeCount < 4294967295)
      {
        ExtinguishTimeCount++;
      }
    }

    if(AttonityTimeCountFlag == 1)
    {
      if(AttonityTimeCount < 65535)
      {
        AttonityTimeCount++;
      }
    }

    TIM_ClearITPendingBit(GENERAL_TIM_3, TIM_FLAG_Update);      //����жϱ�־λ�������� TIM_ClearFlag ����һ��
	}
}


void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) //��ֹ����
	{
		if(GPIO_ReadInputDataBit(SIQ02FVS3_ECB_GPIO_PORT, SIQ02FVS3_ECB_GPIO_PIN))
    {
      KeyInputFSM_SetEvent(EVENT_TURN_LEFT);
    }
    else
    {
      KeyInputFSM_SetEvent(EVENT_TURN_RIGHT);
    }
	}
	EXTI_ClearITPendingBit(EXTI_Line4);       //�������
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
