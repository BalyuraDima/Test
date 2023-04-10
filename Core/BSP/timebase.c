/** 
 * @file timebase.c
 * @brief <h1>Модуль обеспечения работы EventRecorder</h1>
 * @author <a href="mailto://balura_pe99a@rambler.ru">Балюра Д.Е.</a>
 * @date 09 апреля 2023 г.
 * ************************************************************************** */
#include "stm32f0xx_hal.h"

#include "EventRecorder.h"

#define SYSTICK_PERIOD		SysTick->LOAD

static volatile uint32_t SysTickCount; 		///< Хранение количества "тиков" источника частоты для SysTick
static volatile uint8_t  SysTickUpdated;	///< Признак обновления переменной SysTickCount

/** @brief обработчик прерывания таймера SysTick */
void HAL_IncTick(void)
{
  SysTickCount  += SYSTICK_PERIOD;
  SysTickUpdated = 1U;
  uwTick += uwTickFreq;
}

/** @brief Получение частоты таймера обеспечения работы EventRecorder 
@return Частота источника сигнала таймера обеспечения работы EventRecorder в Гц 
*/
uint32_t EventRecorderTimerGetFreq (void)
{
	return SYSTICK_PERIOD * 1000 / uwTickFreq;
}

/** @brief получение текущей величины прошедших "тиков" с момента запуска таймера
@return Значение счетчика таймера
*/
uint32_t EventRecorderTimerGetCount (void)
{
  uint32_t val;
 
  do {
    SysTickUpdated = 0U;
    val  = SysTickCount;
    val += (SYSTICK_PERIOD - 1U) - SysTick->VAL;
  } while (SysTickUpdated != 0U);
 
  return (val);
}

/** @brief инициализация таймера обеспечения работы EventRecorder. 
Не используется. Возвращает 1
*/
uint32_t EventRecorderTimerSetup (void)
{
	return 1U;
}
