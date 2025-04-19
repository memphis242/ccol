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
void Test_VectorInitialization(void);
void Test_VectorOpsOnNullVectors(void);

/* Meat of the Program */

int main(void)
{
   UNITY_BEGIN();
   
   RUN_TEST(Test_VectorInitialization);

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

void Test_VectorInitialization(void)
{
   struct Vector_S * vec;

   /******************* Invalid Inputs ********************/
   // Zero element size
   // 0 element size
   vec = VectorInit(0, 20, 50);
   TEST_ASSERT_NULL( vec );
   VectorFree(vec); 

   // Max capacity is less than initial capacity
   vec = VectorInit(0, 50, 20);
   TEST_ASSERT_NULL( vec );
   // TODO: Should actually throw an exception to inform the user...
   VectorFree(vec); 

   // Max capacity is zero. This is invalid because max capacity will not be
   // changeable, and a max capacity of 0 is useless. We won't allow it.
   vec = VectorInit(10, 0, 0);
   TEST_ASSERT_NULL( vec );
   // TODO: Should actually throw an exception to inform the user...
   VectorFree(vec);

   /******************** Input Combos *********************/
   const uint32_t INITIAL_CAP_MAX = (uint32_t)(1.0e5f);
   const uint32_t MAX_CAP_MAX = 10 * INITIAL_CAP_MAX;
   // Create a vector of this made up struct datatype of 3D points
   struct MyData_S { int x; int y; int z; };
   bool at_least_one_successful_initialization = false;
   for ( uint32_t initial_cap = 0;
         initial_cap < INITIAL_CAP_MAX;
         initial_cap =
            (
               ( (initial_cap + 1000) <= INITIAL_CAP_MAX ) ?
                     (initial_cap + 1000) : INITIAL_CAP_MAX
            )
      )
   {
      for ( uint32_t max_cap = initial_cap;
            max_cap < MAX_CAP_MAX;
            max_cap =
               (
                  ( (max_cap + 1000) <= MAX_CAP_MAX ) ?
                        (max_cap + 1000) : MAX_CAP_MAX
               )
         )
      {
         vec = VectorInit(sizeof(struct MyData_S), initial_cap, max_cap);

         if ( vec != NULL )
         {
            // Confirm correct initialization
            TEST_ASSERT_EQUAL_size_t( sizeof(struct MyData_S), VectorElementSize(vec) );
            TEST_ASSERT_EQUAL_UINT32( initial_cap, VectorCapacity(vec) );
            TEST_ASSERT_EQUAL_UINT32( max_cap, VectorMaxCapacity(vec) );
            if ( !at_least_one_successful_initialization )
            {
               at_least_one_successful_initialization = true;
            }
         }

         VectorFree(vec);
      }
   }
   // Even though malloc may fail a few times, there should be at least one
   // successful allocation among all these tries.
   TEST_ASSERT_TRUE(at_least_one_successful_initialization);

   // Repeat again but try a vector of strings, just to confirm another common
   // use-case.
   at_least_one_successful_initialization = false;
   for ( uint32_t initial_cap = 0;
         initial_cap < INITIAL_CAP_MAX;
         initial_cap =
            (
               ( (initial_cap + 1000) <= INITIAL_CAP_MAX ) ?
                     (initial_cap + 1000) : INITIAL_CAP_MAX
            )
      )
   {
      for ( uint32_t max_cap = initial_cap;
            max_cap < MAX_CAP_MAX;
            max_cap =
               (
                  ( (max_cap + 1000) <= MAX_CAP_MAX ) ?
                        (max_cap + 1000) : MAX_CAP_MAX
               )
         )
      {
         vec = VectorInit(sizeof(char *), initial_cap, max_cap);

         if ( vec != NULL )
         {
            // Confirm correct initialization
            TEST_ASSERT_EQUAL_size_t( sizeof(char *), VectorElementSize(vec) );
            TEST_ASSERT_EQUAL_UINT32( initial_cap, VectorCapacity(vec) );
            TEST_ASSERT_EQUAL_UINT32( max_cap, VectorMaxCapacity(vec) );
            if ( !at_least_one_successful_initialization )
            {
               at_least_one_successful_initialization = true;
            }
         }

         VectorFree(vec);
      }
   }
   // Even though malloc may fail a few times, there should be at least one
   // successful allocation among all these tries.
   TEST_ASSERT_TRUE(at_least_one_successful_initialization);


   /***************** Size/Length Limits ******************/
   // Now push things to the limit and try to create a vector at the system's limit.
   uint16_t iteration_counter = 0;
   const uint16_t MAX_ITERATIONS = UINT16_MAX;
   vec = NULL;
   // Might need to keep trying until I get a successful allocation...
   while ( ( (NULL == vec) || (VectorCapacity(vec) == 0) ) &&
           (iteration_counter < MAX_ITERATIONS)
         )
   {
      VectorFree(vec);
      vec = VectorInit( 1, UINT32_MAX, UINT32_MAX );
      iteration_counter++;
   }
   TEST_ASSERT_EQUAL_size_t( 1, VectorElementSize(vec) );
   TEST_ASSERT_EQUAL_UINT32( UINT32_MAX, VectorCapacity(vec) );
   TEST_ASSERT_EQUAL_UINT32( UINT32_MAX, VectorMaxCapacity(vec) );
   VectorFree(vec);

   iteration_counter = 0;
   vec = NULL;
   // Might need to keep trying until I get a successful allocation...
   while ( ( (NULL == vec) || (VectorCapacity(vec) == 0) ) &&
           (iteration_counter < MAX_ITERATIONS)
         )
   {
      VectorFree(vec);
      vec = VectorInit( UINT32_MAX, 1, 1 );
      iteration_counter++;
   }
   TEST_ASSERT_EQUAL_size_t( UINT32_MAX, VectorElementSize(vec) );
   TEST_ASSERT_EQUAL_UINT32( 1, VectorCapacity(vec) );
   TEST_ASSERT_EQUAL_UINT32( 1, VectorMaxCapacity(vec) );
   VectorFree(vec);

}

void Test_VectorOpsOnNullVectors(void)
{
   // TODO
}
