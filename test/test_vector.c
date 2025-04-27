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

#define TRY_INIT(ptr, iter_counter, elsz, icap, mcap)       \
   do                                                               \
   {                                                                \
      VectorFree(ptr);                                              \
      ptr = VectorInit( elsz, icap, mcap );                         \
      iter_counter++;                                               \
   } while ( (NULL == ptr) && (VectorCapacity(ptr) == 0) && (iter_counter < MAX_INIT_ATTEMPTS) );

/* Datatypes */

/* Local Variables */

/* Forward Function Declarations */
void setUp(void);
void tearDown(void);
void test_VectorInit_Invalid_ZeroElementSz(void);
void test_VectorInit_Invalid_MaxCapLessThanInitCap(void);
void test_VectorInit_Invalid_ZeroMaxCap(void);
void test_VectorInit_ValidInputCombo_3DPoints(void);
void test_VectorInit_ValidInputCombo_PtrData(void);
void test_VectorInit_CapacityLimit(void);
void test_VectorInit_ElementSzLimit(void);
void test_VectorOpsOnNullVectors(void);
void test_VectorFree(void);
void test_VectorLength(void);
void test_VectorCapacity(void);
void test_VectorMaxCapacity(void);
void test_VectorElementSize(void);
void test_VectorIsEmpty(void);
void test_VectorPush(void);
void test_VectorInsertAt(void);
void test_VectorGetElementAt(void);
void test_VectorCpyElementAt(void);
void test_VectorSetElementAt(void);
void test_VectorRemoveElementAt(void);
void test_VectorLastElement(void);
void test_VectorCpyLastElement(void);
void test_VectorClear(void);

/* Meat of the Program */

int main(void)
{
   UNITY_BEGIN();
   
   RUN_TEST(test_VectorInit_Invalid_ZeroElementSz);

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


/************************ Vector Initialization Tests *************************/
void test_VectorInit_Invalid_ZeroElementSz(void)
{
   struct Vector_S * vec;
   vec = VectorInit(0, 20, 50);
   TEST_ASSERT_NULL( vec );
   VectorFree(vec); 
}

void test_VectorInit_Invalid_MaxCapLessThanInitCap(void)
{
   struct Vector_S * vec;
   vec = VectorInit(0, 50, 20);
   TEST_ASSERT_NULL( vec );
   VectorFree(vec); 
}

void test_VectorInit_Invalid_ZeroMaxCap(void)
{
   // This is invalid because max capacity will not be mutable,
   // and a max capacity of 0 is useless. We won't allow it.
   struct Vector_S * vec;
   vec = VectorInit(10, 0, 0);
   TEST_ASSERT_NULL( vec );
   // TODO: Should actually throw an exception to inform the user...
   VectorFree(vec);
}

void test_VectorInit_ValidInputCombo_3DPoints(void)
{
   struct Vector_S * vec;
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
}

void test_VectorInit_ValidInputCombo_PtrData(void)
{
   struct Vector_S * vec;
   const uint32_t INITIAL_CAP_MAX = (uint32_t)(1.0e5f);
   const uint32_t MAX_CAP_MAX = 10 * INITIAL_CAP_MAX;
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
}

void test_VectorInit_CapacityLimit(void)
{
   // Now push things to the limit and try to create a vector at the system's limit.
   uint16_t iteration_counter = 0;
   const uint16_t MAX_ITERATIONS = UINT16_MAX;
   struct Vector_S * vec = NULL;
   TRY_INIT(vec, iteration_counter, 1, UINT32_MAX, UINT32_MAX);
   TEST_ASSERT_EQUAL_size_t( 1, VectorElementSize(vec) );
   TEST_ASSERT_EQUAL_UINT32( UINT32_MAX, VectorCapacity(vec) );
   TEST_ASSERT_EQUAL_UINT32( UINT32_MAX, VectorMaxCapacity(vec) );
   VectorFree(vec);
}

void test_VectorInit_ElementSzLimit(void)
{
   // Now push things to the limit and try to create a vector at the system's limit.
   uint16_t iteration_counter = 0;
   const uint16_t MAX_ITERATIONS = UINT16_MAX;
   struct Vector_S * vec = NULL;
   TRY_INIT(vec, iteration_counter, UINT32_MAX, 1, 1);
   TEST_ASSERT_EQUAL_size_t( UINT32_MAX, VectorElementSize(vec) );
   TEST_ASSERT_EQUAL_UINT32( 1, VectorCapacity(vec) );
   TEST_ASSERT_EQUAL_UINT32( 1, VectorMaxCapacity(vec) );
   VectorFree(vec);
}

void test_VectorOpsOnNullVectors(void)
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

void test_VectorFree(void)
{
   size_t i = 0;
   struct Vector_S * vec;

   TRY_INIT(vec, i, sizeof(int), 10, 100);

   // No assertion I can declare here
   VectorFree(vec);
   // But run the free method anyways and make sure there
   // are no crashed.
}

void test_VectorLength(void) {
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
    int value = 42;
    VectorPush(vec, &value);
    TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));
    VectorFree(vec);
}

void test_VectorCapacity(void)
{
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_EQUAL_UINT32(10, VectorCapacity(vec));
    VectorFree(vec);
}

void test_VectorMaxCapacity(void) {
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_EQUAL_UINT32(100, VectorMaxCapacity(vec));
    VectorFree(vec);
}

void test_VectorElementSize(void) {
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_EQUAL_size_t(sizeof(int), VectorElementSize(vec));
    VectorFree(vec);
}

void test_VectorIsEmpty(void) {
    struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
    TEST_ASSERT_TRUE(VectorIsEmpty(vec));
    int value = 42;
    VectorPush(vec, &value);
    TEST_ASSERT_FALSE(VectorIsEmpty(vec));
    VectorFree(vec);
}

void test_VectorPush(void)
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
   TRY_INIT( vec, iterations_counter, sizeof(struct MyData_S), 100, MAX_CAP );

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

void test_VectorInsertAt(void)
{
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value1 = 42, value2 = 84;
   VectorPush(vec, &value1);
   TEST_ASSERT_TRUE(VectorInsertAt(vec, 0, &value2));
   TEST_ASSERT_EQUAL_INT(84, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(42, *(int *)VectorGetElementAt(vec, 1));
   VectorFree(vec);
}

void test_VectorGetElementAt(void)
{
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value = 42;
   VectorPush(vec, &value);
   int *retrieved = (int *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_NOT_NULL(retrieved);
   TEST_ASSERT_EQUAL_INT(42, *retrieved);
   VectorFree(vec);
}

void test_VectorCpyElementAt(void) {
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value = 42, buffer;
   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorCpyElementAt(vec, 0, &buffer));
   TEST_ASSERT_EQUAL_INT(42, buffer);
   VectorFree(vec);
}

void test_VectorSetElementAt(void) {
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value1 = 42, value2 = 84;
   VectorPush(vec, &value1);
   TEST_ASSERT_TRUE(VectorSetElementAt(vec, 0, &value2));
   TEST_ASSERT_EQUAL_INT(84, *(int *)VectorGetElementAt(vec, 0));
   VectorFree(vec);
}

void test_VectorRemoveElementAt(void) {
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value = 42, buffer;
   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 0, &buffer));
   TEST_ASSERT_EQUAL_INT(42, buffer);
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));
   VectorFree(vec);
}

void test_VectorLastElement(void) {
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value = 42;
   VectorPush(vec, &value);
   int *last = (int *)VectorLastElement(vec);
   TEST_ASSERT_NOT_NULL(last);
   TEST_ASSERT_EQUAL_INT(42, *last);
   VectorFree(vec);
}

void test_VectorCpyLastElement(void) {
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value = 42, buffer;
   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorCpyLastElement(vec, &buffer));
   TEST_ASSERT_EQUAL_INT(42, buffer);
   VectorFree(vec);
}

void test_VectorClear(void) {
   struct Vector_S *vec = VectorInit(sizeof(int), 10, 100);
   int value = 42;
   VectorPush(vec, &value);
   TEST_ASSERT_FALSE(VectorIsEmpty(vec));
   TEST_ASSERT_TRUE(VectorClear(vec));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));
   VectorFree(vec);
}
