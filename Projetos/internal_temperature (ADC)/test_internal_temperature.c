#include "unity.h"
#include <math.h>

float adc_to_celsius(uint16_t adc_val);

void setUp(void)
{
    
}

void tearDown(void)
{

}

void test_adc_to_celsius_known_value(void) {
    uint16_t adc_val = 876;
    float temp = adc_to_celsius(adc_val);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 27.0f, temp);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adc_to_celsius_known_value);
    return UNITY_END();
}