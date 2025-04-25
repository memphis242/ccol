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
#include <float.h>
#include <time.h>
#include "unity.h"
#include "vector.h"

/* Local Macro Definitions */
#define MAX_INIT_ATTEMPTS 100

#define KEEP_TRYING_INIT(ptr, iter_counter, elsz, icap, mcap)       \
   do                                                               \
   {                                                                \
      ptr = VectorInit( elsz, icap, mcap );                         \
      iter_counter++;                                               \
   } while ( (NULL == ptr) && (iter_counter < MAX_INIT_ATTEMPTS) );

/* Datatypes */

/* Local Variables */

/* Forward Function Declarations */
void setUp(void);
void tearDown(void);
void Test_VectorInitialization(void);
void Test_VectorOpsOnNullVectors(void);
void Test_VectorPush(void);
void Test_VectorInsertAt(void);
void Test_VectorGetElementAt(void);
void Test_VectorSetElementAt(void);
void Test_VectorRemoveElementAt(void);
void Test_VectorLastElement(void);
void Test_VectorClear(void);
void Test_VectorIsEmpty(void);

/* Meat of the Program */

int main(void)
{
   UNITY_BEGIN();
   
   RUN_TEST(Test_VectorInitialization);
   RUN_TEST(Test_VectorOpsOnNullVectors);
   //RUN_TEST(Test_VectorPush);
   //RUN_TEST(Test_VectorInsertAt);
   //RUN_TEST(Test_VectorGetElementAt);
   //RUN_TEST(Test_VectorSetElementAt);
   //RUN_TEST(Test_VectorRemoveElementAt);
   //RUN_TEST(Test_VectorLastElement);
   //RUN_TEST(Test_VectorClear);
   //RUN_TEST(Test_VectorIsEmpty);

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
   // Call any API that takes in a pointer, and ensure appropriate behavior
   VectorFree(NULL);
   (void)VectorLength(NULL);
   (void)VectorCapacity(NULL);
   (void)VectorMaxCapacity(NULL);
   (void)VectorElementSize(NULL);
   TEST_ASSERT_FALSE( VectorPush(NULL, NULL) );
   TEST_ASSERT_FALSE( VectorInsertAt(NULL, 0, NULL) );
   TEST_ASSERT_FALSE( VectorInsertAt(NULL, UINT32_MAX, NULL) );
   TEST_ASSERT_NULL( VectorGetElementAt(NULL, 0) );
   TEST_ASSERT_NULL( VectorGetElementAt(NULL, UINT32_MAX) );
   TEST_ASSERT_FALSE( VectorSetElementAt(NULL, 0, NULL) );
   TEST_ASSERT_FALSE( VectorSetElementAt(NULL, UINT32_MAX, NULL) );
   //TEST_ASSERT_FALSE( VectorRemoveElementAt(NULL, 0) );
   //TEST_ASSERT_FALSE( VectorRemoveElementAt(NULL, UINT32_MAX) );
   TEST_ASSERT_NULL( VectorLastElement(NULL) );
   TEST_ASSERT_FALSE( VectorClear(NULL) );
   TEST_ASSERT_TRUE( VectorIsEmpty(NULL) );
}

void Test_VectorFree(void)
{
   size_t i = 0;
   struct Vector_S * vec;

   KEEP_TRYING_INIT(vec, i, sizeof(int), 10, 100);

   // No assertion I can declare here
   VectorFree(vec);
   // But run the free method anyways and make sure there
   // are no crashed.
}

void Test_VectorLength(void) {
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
    int value = 42;
    VectorPush(vec, &value);
    TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));
    VectorFree(vec);
}


void Test_VectorCapacity(void)
{
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_EQUAL_UINT32(10, VectorCapacity(vec));
    VectorFree(vec);
}

void Test_VectorPush(void)
{
   struct Vector_S * vec;
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   const uint32_t MAX_CAP = 1000000;
   const uint32_t INIT_CAP = MAX_CAP / 1000;

   unsigned int iterations_counter = 0;
   KEEP_TRYING_INIT( vec, iterations_counter, sizeof(struct MyData_S), 100, MAX_CAP );

   // Now push until you've reached the initial capacity, and confirm along the
   // way that the element was truly pushed in...
   struct MyData_S test_element = { .x = 0.0f, .y = FLT_MAX, .z = -FLT_MIN };
   struct MyData_S * last_element;
   uint32_t vec_len = 0;
   srand( (unsigned int) time(NULL) );
   while ( VectorLength(vec) < INIT_CAP )
   {
      // Random values each time so that we don't falsely believe we've added
      // an element because every lastElement() is the same.
      test_element.x = ((float)rand() / (float)RAND_MAX) * FLT_MAX;
      test_element.y = ((float)rand() / (float)RAND_MAX) * FLT_MAX;
      test_element.z = ((float)rand() / (float)RAND_MAX) * FLT_MAX;
      bool push_successfull = VectorPush( vec, &test_element );
      if ( push_successfull )
      {
         vec_len++;
         last_element = VectorLastElement(vec);
         TEST_ASSERT_EQUAL_FLOAT( test_element.x, last_element->x );
         TEST_ASSERT_EQUAL_FLOAT( test_element.y, last_element->y );
         TEST_ASSERT_EQUAL_FLOAT( test_element.z, last_element->z );
         TEST_ASSERT_EQUAL_UINT32( vec_len, VectorLength(vec) );
      }
   }
   // We should have hit the initial capacity but not any more than that
   TEST_ASSERT_EQUAL_UINT32( INIT_CAP, VectorLength(vec) );
   TEST_ASSERT_EQUAL_UINT32( INIT_CAP, VectorCapacity(vec) );
   // If every single push failed, something's off...
   TEST_ASSERT_FALSE( VectorIsEmpty(vec) );

   // Now try pushing past the capacity
   while ( VectorLength(vec) <= INIT_CAP )
   {
      // Random values each time so that we don't falsely believe we've added
      // an element because every lastElement() is the same.
      test_element.x = ((float)rand() / (float)RAND_MAX) * FLT_MAX;
      test_element.y = ((float)rand() / (float)RAND_MAX) * FLT_MAX;
      test_element.z = ((float)rand() / (float)RAND_MAX) * FLT_MAX;
      bool push_successfull = VectorPush( vec, &test_element );
      if ( push_successfull )
      {
         vec_len++;
         last_element = VectorLastElement(vec);
         TEST_ASSERT_EQUAL_FLOAT( test_element.x, last_element->x );
         TEST_ASSERT_EQUAL_FLOAT( test_element.y, last_element->y );
         TEST_ASSERT_EQUAL_FLOAT( test_element.z, last_element->z );
         TEST_ASSERT_EQUAL_UINT32( vec_len, VectorLength(vec) );
      }
   }

   VectorFree(vec);

   // Now try pushing into a vector that has failed to initialize
   TEST_ASSERT_FALSE( VectorPush(NULL, &test_element) );

   // Now try pushing into a vector that was initialized with 0 max capacity
   vec = VectorInit( sizeof(struct MyData_S), 0, 0 );
   TEST_ASSERT_FALSE( VectorPush( vec, &test_element ) );
   TEST_ASSERT_TRUE( VectorIsEmpty(vec) );
}

void Test_VectorInsertAt(void)
{

}

void Test_VectorGetElementAt(void)
{

}

void Test_VectorSetElementAt(void)
{

}

void Test_VectorRemoveElementAt(void)
{

}

void Test_VectorLastElement(void)
{

}

void Test_VectorClear(void)
{

}

void Test_VectorIsEmpty(void)
{

}
