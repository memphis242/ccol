/*!
 * @file    test_vector.c
 * @brief   Test file for the the vector component
 * 
 * @author  Abdullah Almosalami @c85hup8
 * @date    Tues Apr 19, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "unity.h"
#include "vector.h"

/* Local Macro Definitions */

/* Datatypes */

/* Local Variables */

/* Forward Function Declarations */
void setUp(void);
void tearDown(void);
void Test_VectorInit(void);

/* Meat of the Program */

int main(void)
{
   UNITY_BEGIN();
   
   RUN_TEST(Test_VectorInit);

   return UNITY_END();
}

void setUp(void)
{
   // Do nothing
}
void tearDown(void)
{
   // Do nothing
}

/* Computation */

void Test_VectorInit(void)
{
   // Test /w invalid inputs 
   
   // Zero element size
   TEST_MESSAGE("Checking invalid initialization...");
   struct Vector_S * vec = VectorInit(0, 20, 50);
   TEST_ASSERT( NULL == vec );
   VectorFree(vec); 


}
