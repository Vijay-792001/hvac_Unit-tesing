#include "unity.h"
#include "status_indicator.h"
#include "mock_stm32f4xx_hal.h"

GPIO_TypeDef GPIOC_inst;

void setUp(void) {}
void tearDown(void) {}

void test_status_indicator_SI_01(void)
{
    GPIO_InitTypeDef *init_ptr = NULL;

    HAL_GPIO_Init_Expect(GPIOC, init_ptr);

    StatusIndicator_Init();

    TEST_ASSERT_TRUE(1);
}
