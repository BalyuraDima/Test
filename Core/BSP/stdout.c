/* ************************************************************************** */
/** 
 * @file stdout.c 
 * @brief <h1>Модуль перенаправления вывода в стандартный файл stdout</h1>
 * @author <a href="mailto://balura_pe99a@rambler.ru">Балюра Д.Е.</a>
 * @date 09 апреля 2023 г.
 * @details
 * Перенаправление стандартного вывода заключается в замене стандартной функции 
 * fputc. Чтобы это было возможно необходимо глобально определить __use_no_semihosting
 * Это делается по-разному, в зависимости от версии компилятора. Для версии от 6.0 и
 * необходима строчка:
 * @code
 * __asm (".global __use_no_semihosting\n");
 * @endcode
 * Для компилятора с версией меньше 6.0 определение выглядит следующим образом:
 * @code
 * #pragma import(__use_no_semihosting)
 * @endcode
 * Так же для версии от 6.0 необходимо переопределение функции _sys_open.
 * Вставляем в код пустые функции _sys_exit и _ttywrch. Они в нашем варианте
 * не используются, но переопределение их необходимо, чтобы компилятор не 
 * вставил на их место функции по умолчанию
 * ************************************************************************** */

#include <stdint.h>
#include <stdio.h>

#include "EventRecorder.h"

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION > 6000000)
#include <rt_sys.h>

__asm (".global __use_no_semihosting\n");

extern void $Super$$_sys_open(void);

FILEHANDLE $Sub$$_sys_open(const char *name, int openmode)
{
 return 1;
}

#else
#ifdef __CC_ARM
#pragma import(__use_no_semihosting)
struct __FILE
{
  int handle;
};
FILE __stdout;
#endif
#endif

void _sys_exit(int x)
{
  (void) x;
}

void _ttywrch(int ch)
{
  (void) ch;
}

/** @brief Перенаправление вывода в стандартный файл stdout через EventRecorder
 * @param [in] ch выводимый символ
 * @param [in] f файл, в коорый выводится символ (игнорируется)
 * @return выведенный символ
 */
int fputc(int ch, FILE *f)
{
	(void)f;
  static uint32_t index = 0U;
  static uint8_t  buffer[8];
 
  if (index >= 8U) 
	{
    return (-1);
  }
	if ((ch == '\n') || (ch == '\r'))
	{
		while (index < 7)
			buffer[index++] = 0x20;
		ch = '\n';
	}
  buffer[index++] = (uint8_t)ch;
  if (index == 8) 
	{
    EventRecordData(EventID(EventLevelOp, 0xFE, 0x00), buffer, index);
    index = 0U;
  }
  return (ch);
}
