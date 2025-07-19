/*!
 * @file    test_vector.c
 * @brief   Test file for the the vector component
 *
 * @author  Abdullah Almosalami @memphis242
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
#define MAX_INIT_ATTEMPTS 100 //! Used in the VECTOR_NEW_KEEP_TRYIN macro to limit VectorNew(, &DEFAULT_ALLOCATOR) attempst
#define ARR_LEN(arr) ( sizeof(arr) / sizeof(arr[0]) )

#define VECTOR_NEW_KEEP_TRYIN(ptr, elsz, icap, mcap, init_len, mem_mgr)        \
{                                                                              \
   size_t iter = 0;                                                            \
   do                                                                          \
   {                                                                           \
      VectorFree(ptr);                                                         \
      ptr = VectorNew( elsz, icap, mcap, init_len, mem_mgr);                   \
      iter++;                                                                  \
   } while (                                                                   \
      (NULL == ptr) &&                                                         \
      (VectorCapacity(ptr) == 0) &&                                            \
      (iter < MAX_INIT_ATTEMPTS)                                               \
   );                                                                          \
}

/* Datatypes */

/* Local Variables */

/* Forward Function Declarations */

void setUp(void);
void tearDown(void);

void test_VectorNew_Invalid_ZeroElementSz(void);
void test_VectorNew_Invalid_MaxCapLessThanInitCap(void);
void test_VectorNew_Invalid_ZeroMaxCap(void);
void test_VectorNew_Invalid_InitialLen(void);
void test_VectorNew_ValidInputCombo_3DPoints(void);
void test_VectorNew_CapacityLimit(void);
void test_VectorNew_ElementSzLimit(void);
void test_VectorNew_InitialLenLessThanInitialCap(void);
void test_VectorNew_InitialLenSameAsInitialCap(void);

void test_VectorOpsOnNullVectors(void);

void test_VectorFree(void);
void test_VectorLength(void);
void test_VectorCapacity(void);
void test_VectorMaxCapacity(void);
void test_VectorElementSize(void);
void test_VectorIsEmpty(void);
void test_VectorIsFull(void);

void test_VectorPush_SimplePush(void);
void test_VectorPush_UntilCapacity(void);
void test_VectorPush_PastInitialCapacity(void);
void test_VectorPush_PastMaxCapacity(void);
void test_VectorPush_IntoVecWithZeroMaxCap(void);

void test_VectorInsertion_AtZeroWithVectorLessThanCapacity(void);
void test_VectorInsertion_AtZeroWithVectorAtCapacity(void);
void test_VectorInsertion_AtZeroWithVectorAtMaxCapacity(void);
void test_VectorInsertion_AtEndEqualsVecPush(void);
void test_VectorInsertion_AtMiddle(void);
void test_VectorInsertion_AtMiddleOfEmptyVec(void);

void test_VectorGetElement_ValidIdx(void);
void test_VectorGetElement_IdxPastLen(void);
void test_VectorGetElement_IdxPastCap(void);
void test_VectorLastElement(void);

void test_VectorCpyElement_ValidIdx(void);
void test_VectorCpyElement_NullBufferPassedIn(void);
void test_VectorCpyElement_IdxPastLen(void);
void test_VectorCpyElement_IdxPastCap(void);
void test_VectorCpyLastElement(void);
void test_VectorRoundTrip_CpyElementToSetElement(void);

void test_VectorSetElement_AfterPushes(void);
void test_VectorSetElement_AfterInitLen(void);
void test_VectorSetElement_PastLen(void);
void test_VectorSetElement_PastCap(void);

void test_VectorRemoveElement_AtZeroWithVectorPartiallyFull(void);
void test_VectorRemoveElement_AtZeroWithSinglePresentElement(void);
void test_VectorRemoveElement_AtZeroWithEmptyVector(void);
void test_VectorRemoveElement_AtMiddle(void);
void test_VectorRemoveElement_AtLen(void);
void test_VectorRemoveElement_LastElement(void);
void test_VectorRemoveElement_PastLen(void);
void test_VectorRemoveElement_AtZeroWithVectorPartiallyFull_WithBuf(void);
void test_VectorRemoveElement_AtZeroWithSinglePresentElement_WithBuf(void);
void test_VectorRemoveElement_AtZeroWithEmptyVector_WithBuf(void);
void test_VectorRemoveElement_AtMiddle_WithBuf(void);
void test_VectorRemoveElement_AtLen_WithBuf(void);
void test_VectorRemoveElement_LastElement_WithBuf(void);
void test_VectorRemoveElement_PastLen_WithBuf(void);

void test_VectorClearElementAt_Normal(void);
void test_VectorClearElementAt_InvalidIdx(void);

void test_VectorReset_OORIdx(void);
void test_VectorReset(void);

void test_VectorHardReset(void);

void test_VectorDuplicate_SmallVector(void);
void test_VectorDuplicate_ReallyLargeVector(void);
void test_VectorDuplicate_NullVector(void);

void test_VectorsAreEqual_SameVectors(void);
void test_VectorsAreEqual_DifferentElementSz(void);
void test_VectorsAreEqual_DifferentLength(void);
void test_VectorsAreEqual_DifferentCapacity(void);
void test_VectorsAreEqual_DifferentMaxCapacity(void);
void test_VectorsAreEqual_DifferentElementValues(void);

void test_VectorSplitAt_ValidIdx(void);
void test_VectorSplitAt_IdxZero(void);
void test_VectorSplitAt_IdxPastLen(void);
void test_VectorSplitAt_EmptyVector(void);
void test_VectorSplitAt_NullVector(void);
void test_VectorSplitAt_ValidIdx_StructData(void);

void test_VectorSlice_ValidIndices_IntData(void);
void test_VectorSlice_ValidIndices_StructData(void);
void test_VectorSlice_IdxStartEqualsIdxEnd(void);
void test_VectorSlice_IdxStartZero(void);
void test_VectorSlice_FullVector(void);
void test_VectorSlice_IdxEndAtLastElement(void);
void test_VectorSlice_EmptyVector(void);
void test_VectorSlice_NullVector(void);
void test_VectorSlice_IdxStartGreaterThanIdxEnd(void);
void test_VectorSlice_IdxEndCommonMistake(void);
void test_VectorSlice_IdxEndOutOfRange(void);

void test_VectorConcatenate_BasicUse(void);
void test_VectorConcatenate_VecsNotMutated(void);
void test_VectorConcatenate_OneVecIsEmpty(void);
void test_VectorConcatenate_BothVecsEmpty(void);
void test_VectorConcatenate_FullVectors(void);
void test_VectorConcatenate_NullArguments(void);
void test_VectorConcatenate_DifferentElementSizes(void);
void test_VectorConcatenate_ConcatenateSplitRoundTrip(void);

void test_VectorSubRange_PushElements_ValidInts(void);
void test_VectorSubRange_PushElements_ValidStructs(void);
void test_VectorSubRange_PushElements_ExpandCapacity(void);
void test_VectorSubRange_PushElements_ZeroLen(void);
void test_VectorSubRange_PushElements_NullVec(void);
void test_VectorSubRange_PushElements_NullData(void);
void test_VectorSubRange_PushElements_ExceedsMaxCapacity(void);
void test_VectorSubRange_PushElements_ExactlyMaxCapacity(void);

void test_VectorSubRange_InsertElementsAt_ValidInts(void);
void test_VectorSubRange_InsertElementsAt_ValidStructs(void);
void test_VectorSubRange_InsertElementsAt_ExpandCapacity(void);
void test_VectorSubRange_InsertElementsAt_ZeroLen(void);
void test_VectorSubRange_InsertElementsAt_NullVec(void);
void test_VectorSubRange_InsertElementsAt_NullData(void);
void test_VectorSubRange_InsertElementsAt_ExceedsMaxCapacity(void);
void test_VectorSubRange_InsertElementsAt_ExactlyMaxCapacity(void);
void test_VectorSubRange_InsertElementsAt_Push_Equivalence(void);
void test_VectorSubRange_InsertElementsAt_InvalidIdx(void);

void test_VectorSubRange_GetElementsFromIdx_ValidIdx_IntData(void);
void test_VectorSubRange_GetElementsFromIdx_ValidIdx_StructData(void);
void test_VectorSubRange_GetElementsFromIdx_EmptyVec(void);
void test_VectorSubRange_GetElementsFromIdx_InvalidIdx(void);
void test_VectorSubRange_GetElementsFromIdx_InvalidVec(void);

void test_VectorSubRange_CpyElementsInRange_ValidIdices_IntData(void);
void test_VectorSubRange_CpyElementsInRange_DoesNotMutate(void);
void test_VectorSubRange_CpyElementsInRange_ValidIndices_StructData(void);
void test_VectorSubRange_CpyElementsInRange_FullVector(void);
void test_VectorSubRange_CpyElementsInRange_FullVector_IncorrectEndIdx(void);
void test_VectorSubRange_CpyElementsInRange_EmptyVec(void);
void test_VectorSubRange_CpyElementsInRange_InvalidIdx(void);
void test_VectorSubRange_CpyElementsInRange_InvalidVec(void);
void test_VectorSubRange_CpyElementsInRange_SameIdices(void);

void test_VectorSubRange_CpyElementsFromStartToIdx_ValidIdices_IntData(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_DoesNotMutate(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_ValidIndices_StructData(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_FullVector(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_FullVector_IncorrectEndIdx(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_EmptyVec(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_InvalidIdx(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_InvalidVec(void);
void test_VectorSubRange_CpyElementsFromStartToIdx_StartIdx(void);

void test_VectorSubRange_CpyElementsFromIdxToEnd_ValidIdices_IntData(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_DoesNotMutate(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_ValidIndices_StructData(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_FullVector(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_EmptyVec(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_InvalidIdx(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_InvalidVec(void);
void test_VectorSubRange_CpyElementsFromIdxToEnd_EndIdx(void);

void test_VectorSubRange_SetElementsInRange_ValidIdices_IntData(void);
void test_VectorSubRange_SetElementsInRange_DoesNotMutate(void);
void test_VectorSubRange_SetElementsInRange_ValidIndices_StructData(void);
void test_VectorSubRange_SetElementsInRange_FullVector(void);
void test_VectorSubRange_SetElementsInRange_FullVector_IncorrectEndIdx(void);
void test_VectorSubRange_SetElementsInRange_EmptyVec(void);
void test_VectorSubRange_SetElementsInRange_InvalidIdx(void);
void test_VectorSubRange_SetElementsInRange_InvalidVec(void);
void test_VectorSubRange_SetElementsInRange_SameIdices(void);
void test_VectorSubRange_RoundTrip_CpyElementsToSetElementsInRng(void);

void test_VectorSubRange_SetElementsFromStartToIdx_ValidIdx_IntData(void);
void test_VectorSubRange_SetElementsFromStartToIdx_DoesNotMutate(void);
void test_VectorSubRange_SetElementsFromStartToIdx_ValidIdx_StructData(void);
void test_VectorSubRange_SetElementsFromStartToIdx_FullVector(void);
void test_VectorSubRange_SetElementsFromStartToIdx_FullVector_IncorrectEndIdx(void);
void test_VectorSubRange_SetElementsFromStartToIdx_EmptyVec(void);
void test_VectorSubRange_SetElementsFromStartToIdx_InvalidIdx(void);
void test_VectorSubRange_SetElementsFromStartToIdx_InvalidVec(void);
void test_VectorSubRange_SetElementsFromStartToIdx_StartIdx(void);
void test_VectorSubRange_RoundTrip_CpyElementsToSetElementsStartToIdx(void);

void test_VectorSubRange_SetElementsFromIdxToEnd_ValidIdx_IntData(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_DoesNotMutate(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_ValidIdx_StructData(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_FullVector(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_EmptyVec(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_InvalidIdx(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_InvalidVec(void);
void test_VectorSubRange_SetElementsFromIdxToEnd_EndIdx(void);
void test_VectorSubRange_RoundTrip_CpyElementsToSetElementsIdxToEnd(void);

void test_VectorSubRange_RemoveElementsInRng_Normal(void);
void test_VectorSubRange_RemoveElementsInRng_NormalWithBuf(void);
void test_VectorSubRange_RemoveElementsInRng_AllElements(void);
void test_VectorSubRange_RemoveElementsInRng_EmptyVec(void);
void test_VectorSubRange_RemoveElementsInRng_AtBeginning(void);
void test_VectorSubRange_RemoveElementsInRng_AtEnd(void);
void test_VectorSubRange_RemoveElementsInRng_InvalidIndices(void);
void test_VectorSubRange_RemoveElementsInRng_InvalidVec(void);
void test_VectorSubRange_RoundTrip_InsertAndRemove(void);

void test_VectorSubRange_RemoveElementsFromStartToIdx_Normal(void);
void test_VectorSubRange_RemoveElementsFromStartToIdx_NormalWithBuf(void);
void test_VectorSubRange_RemoveElementsFromStartToIdx_AllElements(void);
void test_VectorSubRange_RemoveElementsFromStartToIdx_EmptyVec(void);
void test_VectorSubRange_RemoveElementsFromStartToIdx_InvalidIndices(void);
void test_VectorSubRange_RemoveElementsFromStartToIdx_InvalidVec(void);

void test_VectorSubRange_RemoveElementsFromIdxToEnd_Normal(void);
void test_VectorSubRange_RemoveElementsFromIdxToEnd_NormalWithBuf(void);
void test_VectorSubRange_RemoveElementsFromIdxToEnd_AllElements(void);
void test_VectorSubRange_RemoveElementsFromIdxToEnd_EmptyVec(void);
void test_VectorSubRange_RemoveElementsFromIdxToEnd_InvalidIndices(void);
void test_VectorSubRange_RemoveElementsFromIdxToEnd_InvalidVec(void);
void test_VectorSubRange_RoundTrip_PushElementsRemoveElementsFromIdxToEnd(void);

void test_VectorSubRange_ClearElementsInRng_Normal(void);
void test_VectorSubRange_ClearElementsInRng_AllElements(void);
void test_VectorSubRange_ClearElementsInRng_EmptyVec(void);
void test_VectorSubRange_ClearElementsInRng_AtBeginning(void);
void test_VectorSubRange_ClearElementsInRng_AtEnd(void);
void test_VectorSubRange_ClearElementsInRng_InvalidIndices(void);
void test_VectorSubRange_ClearElementsInRng_InvalidVec(void);

void test_VectorSubRange_ClearElementsFromStartToIdx_Normal(void);
void test_VectorSubRange_ClearElementsFromStartToIdx_AllElements(void);
void test_VectorSubRange_ClearElementsFromStartToIdx_EmptyVec(void);
void test_VectorSubRange_ClearElementsFromStartToIdx_InvalidIndices(void);
void test_VectorSubRange_ClearElementsFromStartToIdx_InvalidVec(void);

void test_VectorSubRange_ClearElementsFromIdxToEnd_Normal(void);
void test_VectorSubRange_ClearElementsFromIdxToEnd_AllElements(void);
void test_VectorSubRange_ClearElementsFromIdxToEnd_EmptyVec(void);
void test_VectorSubRange_ClearElementsFromIdxToEnd_InvalidIndices(void);
void test_VectorSubRange_ClearElementsFromIdxToEnd_InvalidVec(void);

void test_VectorClearAllElements_Normal(void);
void test_VectorClearAllElements_EmptyVec(void);
void test_VectorClearAllElements_InvalidVec(void);

/* Meat of the Program */

int main(void)
{
   UNITY_BEGIN();

   RUN_TEST(test_VectorNew_Invalid_ZeroElementSz);
   RUN_TEST(test_VectorNew_Invalid_MaxCapLessThanInitCap);
   RUN_TEST(test_VectorNew_Invalid_ZeroMaxCap);
   RUN_TEST(test_VectorNew_Invalid_InitialLen);
   RUN_TEST(test_VectorNew_ValidInputCombo_3DPoints);
   RUN_TEST(test_VectorNew_CapacityLimit);
   RUN_TEST(test_VectorNew_ElementSzLimit);
   RUN_TEST(test_VectorNew_InitialLenLessThanInitialCap);
   RUN_TEST(test_VectorNew_InitialLenSameAsInitialCap);

   RUN_TEST(test_VectorOpsOnNullVectors);

   RUN_TEST(test_VectorFree);
   RUN_TEST(test_VectorLength);
   RUN_TEST(test_VectorCapacity);
   RUN_TEST(test_VectorMaxCapacity);
   RUN_TEST(test_VectorElementSize);
   RUN_TEST(test_VectorIsEmpty);
   RUN_TEST(test_VectorIsFull);

   RUN_TEST(test_VectorPush_SimplePush);
   RUN_TEST(test_VectorPush_UntilCapacity);
   RUN_TEST(test_VectorPush_PastInitialCapacity);
   RUN_TEST(test_VectorPush_PastMaxCapacity);
   RUN_TEST(test_VectorPush_IntoVecWithZeroMaxCap);

   RUN_TEST(test_VectorInsertion_AtZeroWithVectorLessThanCapacity);
   RUN_TEST(test_VectorInsertion_AtZeroWithVectorAtCapacity);
   RUN_TEST(test_VectorInsertion_AtZeroWithVectorAtMaxCapacity);
   RUN_TEST(test_VectorInsertion_AtEndEqualsVecPush);
   RUN_TEST(test_VectorInsertion_AtMiddle);
   RUN_TEST(test_VectorInsertion_AtMiddleOfEmptyVec);

   RUN_TEST(test_VectorGetElement_ValidIdx);
   RUN_TEST(test_VectorGetElement_IdxPastLen);
   RUN_TEST(test_VectorGetElement_IdxPastCap);
   RUN_TEST(test_VectorLastElement);

   RUN_TEST(test_VectorCpyElement_ValidIdx);
   RUN_TEST(test_VectorCpyElement_NullBufferPassedIn);
   RUN_TEST(test_VectorCpyElement_IdxPastLen);
   RUN_TEST(test_VectorCpyElement_IdxPastCap);
   RUN_TEST(test_VectorCpyLastElement);
   RUN_TEST(test_VectorRoundTrip_CpyElementToSetElement);

   RUN_TEST(test_VectorSetElement_AfterPushes);
   RUN_TEST(test_VectorSetElement_AfterInitLen);
   RUN_TEST(test_VectorSetElement_PastLen);
   RUN_TEST(test_VectorSetElement_PastCap);

   RUN_TEST(test_VectorRemoveElement_AtZeroWithVectorPartiallyFull);
   RUN_TEST(test_VectorRemoveElement_AtZeroWithSinglePresentElement);
   RUN_TEST(test_VectorRemoveElement_AtZeroWithEmptyVector);
   RUN_TEST(test_VectorRemoveElement_AtMiddle);
   RUN_TEST(test_VectorRemoveElement_AtLen);
   RUN_TEST(test_VectorRemoveElement_LastElement);
   RUN_TEST(test_VectorRemoveElement_PastLen);
   RUN_TEST(test_VectorRemoveElement_AtZeroWithVectorPartiallyFull_WithBuf);
   RUN_TEST(test_VectorRemoveElement_AtZeroWithSinglePresentElement_WithBuf);
   RUN_TEST(test_VectorRemoveElement_AtZeroWithEmptyVector_WithBuf);
   RUN_TEST(test_VectorRemoveElement_AtMiddle_WithBuf);
   RUN_TEST(test_VectorRemoveElement_AtLen_WithBuf);
   RUN_TEST(test_VectorRemoveElement_LastElement_WithBuf);
   RUN_TEST(test_VectorRemoveElement_PastLen_WithBuf);

   RUN_TEST(test_VectorClearElementAt_Normal);
   RUN_TEST(test_VectorClearElementAt_InvalidIdx);

   RUN_TEST(test_VectorReset_OORIdx);
   RUN_TEST(test_VectorReset);

   RUN_TEST(test_VectorHardReset);

   RUN_TEST(test_VectorDuplicate_SmallVector);
   RUN_TEST(test_VectorDuplicate_ReallyLargeVector);
   RUN_TEST(test_VectorDuplicate_NullVector);

   RUN_TEST(test_VectorsAreEqual_SameVectors);
   RUN_TEST(test_VectorsAreEqual_DifferentElementSz);
   RUN_TEST(test_VectorsAreEqual_DifferentLength);
   RUN_TEST(test_VectorsAreEqual_DifferentCapacity);
   RUN_TEST(test_VectorsAreEqual_DifferentMaxCapacity);
   RUN_TEST(test_VectorsAreEqual_DifferentElementValues);

   RUN_TEST(test_VectorSplitAt_ValidIdx);
   RUN_TEST(test_VectorSplitAt_IdxZero);
   RUN_TEST(test_VectorSplitAt_IdxPastLen);
   RUN_TEST(test_VectorSplitAt_EmptyVector);
   RUN_TEST(test_VectorSplitAt_NullVector);
   RUN_TEST(test_VectorSplitAt_ValidIdx_StructData);

   RUN_TEST(test_VectorSlice_ValidIndices_IntData);
   RUN_TEST(test_VectorSlice_ValidIndices_StructData);
   RUN_TEST(test_VectorSlice_IdxStartEqualsIdxEnd);
   RUN_TEST(test_VectorSlice_IdxStartZero);
   RUN_TEST(test_VectorSlice_FullVector);
   RUN_TEST(test_VectorSlice_IdxEndAtLastElement);
   RUN_TEST(test_VectorSlice_EmptyVector);
   RUN_TEST(test_VectorSlice_NullVector);
   RUN_TEST(test_VectorSlice_IdxStartGreaterThanIdxEnd);
   RUN_TEST(test_VectorSlice_IdxEndCommonMistake);
   RUN_TEST(test_VectorSlice_IdxEndOutOfRange);

   RUN_TEST(test_VectorConcatenate_BasicUse);
   RUN_TEST(test_VectorConcatenate_VecsNotMutated);
   RUN_TEST(test_VectorConcatenate_OneVecIsEmpty);
   RUN_TEST(test_VectorConcatenate_BothVecsEmpty);
   RUN_TEST(test_VectorConcatenate_FullVectors);
   RUN_TEST(test_VectorConcatenate_NullArguments);
   RUN_TEST(test_VectorConcatenate_DifferentElementSizes);
   RUN_TEST(test_VectorConcatenate_ConcatenateSplitRoundTrip);

   RUN_TEST(test_VectorSubRange_PushElements_ValidInts);
   RUN_TEST(test_VectorSubRange_PushElements_ValidStructs);
   RUN_TEST(test_VectorSubRange_PushElements_ExpandCapacity);
   RUN_TEST(test_VectorSubRange_PushElements_ZeroLen);
   RUN_TEST(test_VectorSubRange_PushElements_NullVec);
   RUN_TEST(test_VectorSubRange_PushElements_NullData);
   RUN_TEST(test_VectorSubRange_PushElements_ExceedsMaxCapacity);
   RUN_TEST(test_VectorSubRange_PushElements_ExactlyMaxCapacity);

   RUN_TEST(test_VectorSubRange_InsertElementsAt_ValidInts);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_ValidStructs);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_ExpandCapacity);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_ZeroLen);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_NullVec);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_NullData);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_ExceedsMaxCapacity);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_ExactlyMaxCapacity);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_Push_Equivalence);
   RUN_TEST(test_VectorSubRange_InsertElementsAt_InvalidIdx);

   RUN_TEST(test_VectorSubRange_GetElementsFromIdx_ValidIdx_IntData);
   RUN_TEST(test_VectorSubRange_GetElementsFromIdx_ValidIdx_StructData);
   RUN_TEST(test_VectorSubRange_GetElementsFromIdx_EmptyVec);
   RUN_TEST(test_VectorSubRange_GetElementsFromIdx_InvalidIdx);
   RUN_TEST(test_VectorSubRange_GetElementsFromIdx_InvalidVec);

   RUN_TEST(test_VectorSubRange_CpyElementsInRange_ValidIdices_IntData);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_DoesNotMutate);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_ValidIndices_StructData);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_FullVector);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_FullVector_IncorrectEndIdx);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_EmptyVec);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_InvalidIdx);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_InvalidVec);
   RUN_TEST(test_VectorSubRange_CpyElementsInRange_SameIdices);

   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_ValidIdices_IntData);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_DoesNotMutate);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_ValidIndices_StructData);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_FullVector);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_FullVector_IncorrectEndIdx);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_EmptyVec);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_InvalidIdx);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_InvalidVec);
   RUN_TEST(test_VectorSubRange_CpyElementsFromStartToIdx_StartIdx);

   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_ValidIdices_IntData);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_DoesNotMutate);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_ValidIndices_StructData);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_FullVector);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_EmptyVec);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_InvalidIdx);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_InvalidVec);
   RUN_TEST(test_VectorSubRange_CpyElementsFromIdxToEnd_EndIdx);

   RUN_TEST(test_VectorSubRange_SetElementsInRange_ValidIdices_IntData);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_DoesNotMutate);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_ValidIndices_StructData);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_FullVector);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_FullVector_IncorrectEndIdx);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_EmptyVec);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_InvalidIdx);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_InvalidVec);
   RUN_TEST(test_VectorSubRange_SetElementsInRange_SameIdices);
   RUN_TEST(test_VectorSubRange_RoundTrip_CpyElementsToSetElementsInRng);

   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_ValidIdx_IntData);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_DoesNotMutate);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_ValidIdx_StructData);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_FullVector);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_FullVector_IncorrectEndIdx);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_EmptyVec);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_InvalidIdx);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_InvalidVec);
   RUN_TEST(test_VectorSubRange_SetElementsFromStartToIdx_StartIdx);
   RUN_TEST(test_VectorSubRange_RoundTrip_CpyElementsToSetElementsStartToIdx);

   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_ValidIdx_IntData);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_DoesNotMutate);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_ValidIdx_StructData);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_FullVector);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_EmptyVec);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_InvalidIdx);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_InvalidVec);
   RUN_TEST(test_VectorSubRange_SetElementsFromIdxToEnd_EndIdx);
   RUN_TEST(test_VectorSubRange_RoundTrip_CpyElementsToSetElementsIdxToEnd);

   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_Normal);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_NormalWithBuf);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_AllElements);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_AtBeginning);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_AtEnd);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_EmptyVec);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_InvalidIndices);
   RUN_TEST(test_VectorSubRange_RemoveElementsInRng_InvalidVec);
   RUN_TEST(test_VectorSubRange_RoundTrip_InsertAndRemove);

   RUN_TEST(test_VectorSubRange_RemoveElementsFromStartToIdx_Normal);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromStartToIdx_NormalWithBuf);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromStartToIdx_AllElements);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromStartToIdx_EmptyVec);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromStartToIdx_InvalidIndices);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromStartToIdx_InvalidVec);

   RUN_TEST(test_VectorSubRange_RemoveElementsFromIdxToEnd_Normal);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromIdxToEnd_NormalWithBuf);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromIdxToEnd_AllElements);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromIdxToEnd_EmptyVec);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromIdxToEnd_InvalidIndices);
   RUN_TEST(test_VectorSubRange_RemoveElementsFromIdxToEnd_InvalidVec);
   RUN_TEST(test_VectorSubRange_RoundTrip_PushElementsRemoveElementsFromIdxToEnd);

   RUN_TEST(test_VectorSubRange_ClearElementsInRng_Normal);
   RUN_TEST(test_VectorSubRange_ClearElementsInRng_AllElements);
   RUN_TEST(test_VectorSubRange_ClearElementsInRng_EmptyVec);
   RUN_TEST(test_VectorSubRange_ClearElementsInRng_AtBeginning);
   RUN_TEST(test_VectorSubRange_ClearElementsInRng_AtEnd);
   RUN_TEST(test_VectorSubRange_ClearElementsInRng_InvalidIndices);
   RUN_TEST(test_VectorSubRange_ClearElementsInRng_InvalidVec);

   RUN_TEST(test_VectorSubRange_ClearElementsFromStartToIdx_Normal);
   RUN_TEST(test_VectorSubRange_ClearElementsFromStartToIdx_AllElements);
   RUN_TEST(test_VectorSubRange_ClearElementsFromStartToIdx_EmptyVec);
   RUN_TEST(test_VectorSubRange_ClearElementsFromStartToIdx_InvalidIndices);
   RUN_TEST(test_VectorSubRange_ClearElementsFromStartToIdx_InvalidVec);

   RUN_TEST(test_VectorSubRange_ClearElementsFromIdxToEnd_Normal);
   RUN_TEST(test_VectorSubRange_ClearElementsFromIdxToEnd_AllElements);
   RUN_TEST(test_VectorSubRange_ClearElementsFromIdxToEnd_EmptyVec);
   RUN_TEST(test_VectorSubRange_ClearElementsFromIdxToEnd_InvalidIndices);
   RUN_TEST(test_VectorSubRange_ClearElementsFromIdxToEnd_InvalidVec);

   RUN_TEST(test_VectorClearAllElements_Normal);
   RUN_TEST(test_VectorClearAllElements_EmptyVec);
   RUN_TEST(test_VectorClearAllElements_InvalidVec);

   return UNITY_END();
}

/********************************* Test Setup *********************************/

void setUp(void)
{
   // Do nothing
}

void tearDown(void)
{
   // Do nothing
}

/************************ Vector Initialization Tests *************************/
void test_VectorNew_Invalid_ZeroElementSz(void)
{
   struct Vector * vec;
   vec = VectorNew(0, 20, 50, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_NULL( vec );
   VectorFree(vec);
}

void test_VectorNew_Invalid_MaxCapLessThanInitCap(void)
{
   struct Vector * vec;
   vec = VectorNew(0, 50, 20, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_NULL( vec );
   VectorFree(vec);
}

void test_VectorNew_Invalid_ZeroMaxCap(void)
{
   // This is invalid because max capacity will not be mutable,
   // and a max capacity of 0 is useless. We won't allow it.
   struct Vector * vec;
   vec = VectorNew(10, 0, 0, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_NULL( vec );
   // TODO: Should actually throw an exception to inform the user...
   VectorFree(vec);
}

void test_VectorNew_Invalid_InitialLen(void)
{
   struct Vector * vec;
   vec = VectorNew( sizeof(int), 10, 100, 11 , &DEFAULT_ALLOCATOR);
   TEST_ASSERT_NULL( vec );
   // TODO: Should actually throw an exception to inform the user...
   VectorFree(vec);
}

void test_VectorNew_ValidInputCombo_3DPoints(void)
{
   const uint32_t InitialCaps[] = { 0, 2, 1000 };
   const uint32_t MaxCap = 10000;
   struct MyData_S { int x; int y; int z; };
   struct Vector * vec;
   // FIXME: Have to assume that malloc will succeed at least once for this test to mean anything...
   for ( size_t i = 0; i < ARR_LEN(InitialCaps); i++ )
   {
      vec = VectorNew(sizeof(struct MyData_S), InitialCaps[i], MaxCap, 0, &DEFAULT_ALLOCATOR);

      if ( vec != NULL )
      {
         // Confirm correct initialization
         TEST_ASSERT_EQUAL_size_t( sizeof(struct MyData_S), VectorElementSize(vec) );
         TEST_ASSERT_EQUAL_UINT32( InitialCaps[i], VectorCapacity(vec) );
         TEST_ASSERT_EQUAL_UINT32( MaxCap, VectorMaxCapacity(vec) );
      }

      VectorFree(vec);
   }
}

void test_VectorNew_CapacityLimit(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, 1, UINT32_MAX, UINT32_MAX, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_EQUAL_size_t( 1, VectorElementSize(vec) );
   TEST_ASSERT_EQUAL_UINT32( UINT32_MAX, VectorCapacity(vec) );
   TEST_ASSERT_EQUAL_UINT32( UINT32_MAX, VectorMaxCapacity(vec) );
   VectorFree(vec);
}

void test_VectorNew_ElementSzLimit(void)
{
   // Now push things to the limit and try to create a vector at the system's limit.
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, UINT32_MAX, 1, 1, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_EQUAL_size_t( UINT32_MAX, VectorElementSize(vec) );
   TEST_ASSERT_EQUAL_UINT32( 1, VectorCapacity(vec) );
   TEST_ASSERT_EQUAL_UINT32( 1, VectorMaxCapacity(vec) );
   VectorFree(vec);
}

void test_VectorNew_InitialLenLessThanInitialCap(void)
{
   struct Vector * vec = NULL;
   const size_t INIT_LEN = 5;
   VECTOR_NEW_KEEP_TRYIN(vec, 50, INIT_LEN * 2, INIT_LEN * 10, INIT_LEN, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_EQUAL_UINT32( INIT_LEN,     VectorLength(vec) );
   TEST_ASSERT_EQUAL_UINT32( INIT_LEN * 2, VectorCapacity(vec) );
   // Verify initial elements have been zero'd out
   for ( size_t i = 0; i < INIT_LEN; i++ )
   {
      unsigned char * elm = (unsigned char *)VectorGetElementAt(vec, i);
      for ( size_t j = 0; j < 50; j++ )
      {
         TEST_ASSERT_EQUAL_UINT8( 0, elm[j] );
      }
   }
   VectorFree(vec);
}

void test_VectorNew_InitialLenSameAsInitialCap(void)
{
   struct Vector * vec = NULL;
   const size_t INIT_LEN = 10;
   VECTOR_NEW_KEEP_TRYIN(vec, 50, INIT_LEN, INIT_LEN * 10, INIT_LEN, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_EQUAL_UINT32( INIT_LEN, VectorLength(vec) );
   TEST_ASSERT_EQUAL_UINT32( INIT_LEN, VectorCapacity(vec) );
   // Verify initial elements have been zero'd out
   for ( size_t i = 0; i < INIT_LEN; i++ )
   {
      unsigned char * elm = (unsigned char *)VectorGetElementAt(vec, i);
      for ( size_t j = 0; j < 50; j++ )
      {
         TEST_ASSERT_EQUAL_UINT8( 0, elm[j] );
      }
   }
   VectorFree(vec);
}

/**************************** Vector Ops On Nulls *****************************/
void test_VectorOpsOnNullVectors(void)
{
   // Call any API that takes in a pointer, and ensure appropriate behavior, or
   // that the application does not crash.
   VectorFree(NULL);
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(NULL));
   TEST_ASSERT_EQUAL_UINT32(0, VectorCapacity(NULL));
   TEST_ASSERT_EQUAL_UINT32(0, VectorMaxCapacity(NULL));
   TEST_ASSERT_EQUAL_size_t(0, VectorElementSize(NULL));
   TEST_ASSERT_TRUE(VectorIsEmpty(NULL));
   TEST_ASSERT_FALSE(VectorIsFull(NULL));
   TEST_ASSERT_FALSE(VectorPush(NULL, NULL));
   TEST_ASSERT_FALSE(VectorInsertAt(NULL, 0, NULL));
   TEST_ASSERT_FALSE(VectorInsertAt(NULL, UINT32_MAX, NULL));
   TEST_ASSERT_NULL(VectorGetElementAt(NULL, 0));
   TEST_ASSERT_NULL(VectorGetElementAt(NULL, UINT32_MAX));
   TEST_ASSERT_FALSE(VectorCpyElementAt(NULL, 0, NULL));
   TEST_ASSERT_FALSE(VectorCpyElementAt(NULL, UINT32_MAX, NULL));
   TEST_ASSERT_FALSE(VectorSetElementAt(NULL, 0, NULL));
   TEST_ASSERT_FALSE(VectorSetElementAt(NULL, UINT32_MAX, NULL));
   TEST_ASSERT_FALSE(VectorRemoveElementAt(NULL, 0, NULL));
   TEST_ASSERT_FALSE(VectorRemoveElementAt(NULL, UINT32_MAX, NULL));
   TEST_ASSERT_NULL(VectorLastElement(NULL));
   TEST_ASSERT_FALSE(VectorCpyLastElement(NULL, NULL));
   TEST_ASSERT_FALSE(VectorClearElementAt(NULL, 0));
   TEST_ASSERT_FALSE(VectorReset(NULL));
   TEST_ASSERT_FALSE(VectorHardReset(NULL));
   TEST_ASSERT_NULL(VectorDuplicate(NULL));
   TEST_ASSERT_FALSE(VectorsAreEqual(NULL, NULL));
   TEST_ASSERT_FALSE(VectorsAreEqual(NULL, (struct Vector *)1));
   TEST_ASSERT_FALSE(VectorsAreEqual((struct Vector *)1, NULL));
}

/***************************** Simple Vector Ops ******************************/
void test_VectorFree(void)
{
   struct Vector * vec = NULL;

   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // FIXME: Need to assert post-conditions of a vector free operation...
   VectorFree(vec);
}

void test_VectorLength(void) {
    struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
    TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
    VectorPush(vec, &(int){42});
    TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));
    VectorFree(vec);
}

void test_VectorCapacity(void)
{
    struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
    TEST_ASSERT_EQUAL_UINT32(10, VectorCapacity(vec));
    VectorFree(vec);
}

void test_VectorMaxCapacity(void) {
    struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
    TEST_ASSERT_EQUAL_UINT32(100, VectorMaxCapacity(vec));
    VectorFree(vec);
}

void test_VectorElementSize(void) {
    struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
    TEST_ASSERT_EQUAL_size_t(sizeof(int), VectorElementSize(vec));
    VectorFree(vec);
}

void test_VectorIsEmpty(void)
{
    struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
    TEST_ASSERT_TRUE(VectorIsEmpty(vec));
    VectorPush(vec, &(int){42});
    TEST_ASSERT_FALSE(VectorIsEmpty(vec));
    VectorFree(vec);
}

void test_VectorIsFull(void)
{
   // Initialize a vector with a small capacity
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 3, 3, 0, &DEFAULT_ALLOCATOR);

   // Verify the vector is not full initially
   TEST_ASSERT_FALSE(VectorIsFull(vec));

   // Add elements to the vector
   int value = 42;
   VectorPush(vec, &value);
   TEST_ASSERT_FALSE(VectorIsFull(vec)); // Still not full after one element

   VectorPush(vec, &value);
   TEST_ASSERT_FALSE(VectorIsFull(vec)); // Still not full after two elements

   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorIsFull(vec)); // Should be full after three elements

   // Attempt to push another element (should fail if capacity is enforced)
   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorIsFull(vec)); // Still full after failed push

   VectorFree(vec);
}

/******************************** Vector Push *********************************/
void test_VectorPush_SimplePush(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int value1 = 42;
   int value2 = 84;

   // Now try pushing...
   // There should be no issues pushing because the initial capacity
   // is greater than 2.
   // Push the first value and verify
   TEST_ASSERT_TRUE(VectorPush(vec, &value1));
   TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(value1, *(int *)VectorGetElementAt(vec, 0));

   // Push the second value and verify
   TEST_ASSERT_TRUE(VectorPush(vec, &value2));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(value2, *(int *)VectorGetElementAt(vec, 1));

   // Ensure the vector is not empty
   TEST_ASSERT_FALSE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorPush_UntilCapacity(void)
{
   struct Vector * vec = NULL;
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   const uint32_t MAX_CAP = (uint32_t)(1.0e6);
   const uint32_t INIT_CAP = MAX_CAP / 1000;

   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(struct MyData_S), INIT_CAP, MAX_CAP, 0, &DEFAULT_ALLOCATOR);

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

   // We should have gotten past the initial capacity and grown
   TEST_ASSERT_GREATER_THAN_UINT32( INIT_CAP, VectorLength(vec) );
   TEST_ASSERT_GREATER_THAN_UINT32( INIT_CAP, VectorCapacity(vec) );

   VectorFree(vec);
}

void test_VectorPush_PastInitialCapacity(void)
{
   struct Vector * vec = NULL;
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   const uint32_t MAX_CAP = (uint32_t)(1.0e6);
   const uint32_t INIT_CAP = MAX_CAP / 1000;

   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(struct MyData_S), 100, MAX_CAP, 0, &DEFAULT_ALLOCATOR );

   // Fill to initial capacity
   struct MyData_S test_element = { .x = 0.0f, .y = FLT_MAX, .z = -FLT_MIN };
   struct MyData_S * last_element;
   uint32_t vec_len = 0;
   while ( VectorLength(vec) < INIT_CAP )
   {
      bool push_successfull = VectorPush( vec, &test_element );
      vec_len = push_successfull ? (vec_len + 1) : vec_len;
   }

   // Now try pushing past the capacity
   // Change the test element to distinguish it from the previously added elmnts
   test_element.x = 10.5f;
   test_element.y = -10.5;
   test_element.z = 105.0f;
   while ( VectorLength(vec) <= INIT_CAP )
   {
      bool push_successfull = VectorPush( vec, &test_element );
      vec_len = push_successfull ? (vec_len + 1) : vec_len;
   }
   last_element = VectorLastElement(vec);
   TEST_ASSERT_EQUAL_FLOAT( test_element.x, last_element->x );
   TEST_ASSERT_EQUAL_FLOAT( test_element.y, last_element->y );
   TEST_ASSERT_EQUAL_FLOAT( test_element.z, last_element->z );

   VectorFree(vec);
}

void test_VectorPush_PastMaxCapacity(void)
{
   struct Vector * vec = NULL;
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   const uint32_t MAX_CAP = (uint32_t)(10.0e3);
   const uint32_t INIT_CAP = MAX_CAP / 1000;

   VECTOR_NEW_KEEP_TRYIN( vec,  sizeof(struct MyData_S), INIT_CAP, MAX_CAP, 0, &DEFAULT_ALLOCATOR );

   // Fill to initial capacity
   struct MyData_S test_element = { .x = 0.0f, .y = FLT_MAX, .z = -FLT_MIN };
   struct MyData_S * last_element;
   uint32_t vec_len = 0;
   while ( VectorLength(vec) < MAX_CAP )
   {
      bool push_successfull = VectorPush( vec, &test_element );
      vec_len = push_successfull ? (vec_len + 1) : vec_len;
   }

   // Now try pushing past the max capacity many times
   // Change the test element to distinguish it from the previously added elmnts
   struct MyData_S new_test_element = { .x = 10.5f, .y = -10.5f, .z = 105.0f };
   for ( uint16_t i = 0; i < 1000; i++ )
   {
      bool push_successfull = VectorPush( vec, &new_test_element );
      last_element = VectorLastElement(vec);
      TEST_ASSERT_FALSE(push_successfull);
      // Last element is still the original data
      TEST_ASSERT_EQUAL_FLOAT( test_element.x, last_element->x );
      TEST_ASSERT_EQUAL_FLOAT( test_element.y, last_element->y );
      TEST_ASSERT_EQUAL_FLOAT( test_element.z, last_element->z );
   }

   VectorFree(vec);
}

void test_VectorPush_IntoVecWithZeroMaxCap(void)
{
   struct Vector * vec;
   struct MyData_S
   {
      char str[20];
      unsigned long long int id;
   } test_element =
   {
      .str = "Test Element",
      .id  = 0
   };
   vec = VectorNew( sizeof(struct MyData_S), 0, 0, 0 , &DEFAULT_ALLOCATOR);
   TEST_ASSERT_FALSE( VectorPush( vec, &test_element ) );
   TEST_ASSERT_TRUE( VectorIsEmpty(vec) );
}

/****************************** Vector Insertion ******************************/
void test_VectorInsertion_AtZeroWithVectorLessThanCapacity(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );

   int val = 20;
   while ( VectorLength(vec) < 5 )
   {
      (void)VectorPush(vec, &val);
      val++;
   }

   // Do an insertion at the beginning. I expect everything downstream
   // to have been shifted over as well.
   int test_val = 100;
   int * result;
   while( !VectorInsertAt(vec, 0, &test_val) ); // Ensure insertion success
   result = (int *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_EQUAL_INT(test_val, *result);
   TEST_ASSERT_EQUAL_size_t(6, VectorLength(vec));
   for ( size_t i = 1; i < 6; i++ )
   {
      result = VectorGetElementAt(vec, i);
      TEST_ASSERT_NOT_NULL(result);
      TEST_ASSERT_EQUAL_INT( 20 + (i-1), *result );
   }

   // Cleanup
   VectorFree(vec);
}

void test_VectorInsertion_AtZeroWithVectorAtCapacity(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );

   int val = 20;
   while ( VectorLength(vec) < 10 )
   {
      (void)VectorPush(vec, &val);
      val++;
   }

   // Do an insertion at the beginning. I expect everything downstream
   // to have been shifted over as well.
   int test_val = 100;
   int * result;
   while( !VectorInsertAt(vec, 0, &test_val) ); // Ensure insertion success
   result = (int *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_EQUAL_INT(test_val, *result);
   TEST_ASSERT_EQUAL_size_t(11, VectorLength(vec));
   for ( size_t i = 0; i < 10; i++ )
   {
      TEST_ASSERT_EQUAL_INT( 20 + i, *( (int *)VectorGetElementAt(vec, i + 1) ) );
   }

   // Cleanup
   VectorFree(vec);
}

void test_VectorInsertion_AtZeroWithVectorAtMaxCapacity(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );

   int val = 20;
   while ( VectorLength(vec) < 100 )
   {
      (void)VectorPush(vec, &val);
      val++;
   }

   // Try to do an insertion many times. This should fail.
   // Also, elements in vector should basically remain intact.
   int test_val = 100;
   for ( size_t i = 0; i < 100; i++ )
   {
      TEST_ASSERT_FALSE( VectorInsertAt(vec, 0, &test_val) );
   }
   for ( size_t i = 0; i < 100; i++ )
   {
      TEST_ASSERT_EQUAL_INT( 20 + i, *( (int *)VectorGetElementAt(vec, i) ) );
   }

   VectorFree(vec);
}

void test_VectorInsertion_AtEndEqualsVecPush(void)
{
   struct Vector * vec1 = NULL;
   struct Vector * vec2 = NULL;
   VECTOR_NEW_KEEP_TRYIN( vec1, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );
   VECTOR_NEW_KEEP_TRYIN( vec2, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );

   int val = 20;
   size_t iter = 0;
   while ( (VectorLength(vec1) < 100) && (iter < 1000) )
   {
      bool result = VectorPush(vec1, &val);
      if ( result) val++;
      iter++;
   }
   val = 20;
   size_t idx = 0;
   iter = 0;
   while ( (VectorLength(vec2) < 100) && (iter < 1000) )
   {
      bool result = VectorInsertAt(vec2, idx, &val);
      if ( result )
      {
         val++;
         idx++;
      }
      iter++;
   }
   // It is possible that the 1000 iteration attempts were not enough to insert
   // the 100 elements depending on how much reallocations failed. Feels unlikely
   // but Murphy, ya know.

   TEST_ASSERT_TRUE( VectorsAreEqual(vec1, vec2) );

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorInsertion_AtMiddle(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );

   int val = 20;
   const size_t IDX_OF_INSERTION = 4;
   while ( VectorLength(vec) < (IDX_OF_INSERTION *2) )
   {
      (void)VectorPush(vec, &val);
   }

   // Do an insertion at the beginning. I expect everything downstream
   // to have been shifted over as well.
   int test_val = 100;
   int * result;
   while( !VectorInsertAt(vec, IDX_OF_INSERTION, &test_val) ); // Ensure insertion success
   result = (int *)VectorGetElementAt(vec, IDX_OF_INSERTION);
   TEST_ASSERT_EQUAL_INT(test_val, *result);
   TEST_ASSERT_EQUAL_size_t(2*IDX_OF_INSERTION + 1, VectorLength(vec));
   for ( size_t i = 1; i < VectorLength(vec); i++ )
   {
      if ( i == IDX_OF_INSERTION )  continue;

      result = VectorGetElementAt(vec, i);
      TEST_ASSERT_NOT_NULL(result);
      TEST_ASSERT_EQUAL_INT( val, *result );
   }

   // Cleanup
   VectorFree(vec);
}

void test_VectorInsertion_AtMiddleOfEmptyVec(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN( vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR );

   int val = 20;
   const size_t IDX_OF_INSERTION = 4;
   while ( VectorLength(vec) < (IDX_OF_INSERTION *2) )
   {
      (void)VectorPush(vec, &val);
   }

   // Do an insertion at the beginning. I expect everything downstream
   // to have been shifted over as well.
   int test_val = 100;
   int * result;
   while( !VectorInsertAt(vec, IDX_OF_INSERTION, &test_val) ); // Ensure insertion success
   result = (int *)VectorGetElementAt(vec, IDX_OF_INSERTION);
   TEST_ASSERT_EQUAL_INT(test_val, *result);
   TEST_ASSERT_EQUAL_size_t(2*IDX_OF_INSERTION + 1, VectorLength(vec));
   for ( size_t i = 1; i < VectorLength(vec); i++ )
   {
      if ( i == IDX_OF_INSERTION )  continue;

      result = VectorGetElementAt(vec, i);
      TEST_ASSERT_NOT_NULL(result);
      TEST_ASSERT_EQUAL_INT( val, *result );
   }

   // Cleanup
   VectorFree(vec);
}

/******************************** Vector Gets *********************************/
void test_VectorGetElement_ValidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   VectorPush(vec, &(int){42});
   int * retrieved = (int *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_NOT_NULL(retrieved);
   TEST_ASSERT_EQUAL_INT(42, *retrieved);
   VectorFree(vec);
}

void test_VectorGetElement_IdxPastLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   VectorPush(vec, &value);
   int * retrieved = (int *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_NULL(retrieved);
   VectorFree(vec);
}

void test_VectorGetElement_IdxPastCap(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   VectorPush(vec, &value);
   int * retrieved = (int *)VectorGetElementAt(vec, 10000000);
   TEST_ASSERT_NULL(retrieved);
   VectorFree(vec);
}

void test_VectorLastElement(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   VectorPush(vec, &value);
   int * last = (int *)VectorLastElement(vec);
   TEST_ASSERT_NOT_NULL(last);
   TEST_ASSERT_EQUAL_INT(42, *last);
   VectorFree(vec);
}

/**************************** Vector Copy Element *****************************/
void test_VectorCpyElement_ValidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   int buffer;
   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorCpyElementAt(vec, 0, &buffer));
   TEST_ASSERT_EQUAL_INT(value, buffer);
   VectorFree(vec);
}

void test_VectorCpyElement_NullBufferPassedIn(void)
{
   // Easy mistake to make for the user.
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   int * buffer = NULL;
   VectorPush(vec, &value);
   TEST_ASSERT_FALSE(VectorCpyElementAt(vec, 0, buffer));
   VectorFree(vec);
}

void test_VectorCpyElement_IdxPastLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   int buffer = 0;
   VectorPush(vec, &value);
   TEST_ASSERT_FALSE(VectorCpyElementAt(vec, 1, &buffer));
   TEST_ASSERT_EQUAL_INT(0, buffer);   // Confirm buffer unchanged
   VectorFree(vec);
}

void test_VectorCpyElement_IdxPastCap(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   int buffer = 0;
   VectorPush(vec, &value);
   TEST_ASSERT_FALSE(VectorCpyElementAt(vec, 1000000000, &buffer));
   TEST_ASSERT_EQUAL_INT(0, buffer);   // Confirm buffer unchanged
   VectorFree(vec);
}

void test_VectorCpyLastElement(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42, buffer;
   VectorPush(vec, &value);
   TEST_ASSERT_TRUE(VectorCpyLastElement(vec, &buffer));
   TEST_ASSERT_NOT_EQUAL_size_t( &buffer, VectorGetElementAt(vec, 0) );
   TEST_ASSERT_EQUAL_INT(42, buffer);
   VectorFree(vec);
}

void test_VectorRoundTrip_CpyElementToSetElement(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   int buffer;
   VectorPush(vec, &value);
   VectorCpyElementAt(vec, 0, &buffer);
   buffer++;
   TEST_ASSERT_TRUE(VectorSetElementAt(vec, 0, &buffer));
   buffer = *( (int *)VectorGetElementAt(vec, 0) );
   TEST_ASSERT_EQUAL_INT(value + 1, buffer);
   VectorFree(vec);
}

/******************************** Vector Sets *********************************/
void test_VectorSetElement_AfterPushes(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value1 = 42, value2 = 84;
   VectorPush(vec, &value1);
   TEST_ASSERT_TRUE(VectorSetElementAt(vec, 0, &value2));
   TEST_ASSERT_EQUAL_INT(value2, *(int *)VectorGetElementAt(vec, 0));
   VectorFree(vec);
}

void test_VectorSetElement_AfterInitLen(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 5, &DEFAULT_ALLOCATOR);
   int val = 5;
   TEST_ASSERT_TRUE(VectorSetElementAt(vec, 0, &val));
   TEST_ASSERT_TRUE(VectorSetElementAt(vec, 3, &val));
   TEST_ASSERT_EQUAL_INT(val, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(val, *(int *)VectorGetElementAt(vec, 3));
   VectorFree(vec);
}
void test_VectorSetElement_PastLen(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value1 = 42, value2 = 84;
   while ( VectorLength(vec) < 10 )
   {
      VectorPush(vec, &value1);
   }
   TEST_ASSERT_FALSE(VectorSetElementAt(vec, 10, &value2));
   // Vector unaffected
   for ( size_t i = 0; i < 10; i++ )
   {
      TEST_ASSERT_EQUAL_INT(value1, *(int *)VectorGetElementAt(vec, i));
   }
   VectorFree(vec);
}

void test_VectorSetElement_PastCap(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 10, 0, &DEFAULT_ALLOCATOR);
   int value1 = 42, value2 = 84;
   while ( VectorLength(vec) < 10 )
   {
      VectorPush(vec, &value1);
   }
   TEST_ASSERT_FALSE(VectorSetElementAt(vec, 10, &value2));
   // Vector unaffected
   for ( size_t i = 0; i < 10; i++ )
   {
      TEST_ASSERT_EQUAL_INT(value1, *(int *)VectorGetElementAt(vec, i));
   }
   VectorFree(vec);
}

/****************************** Vector Removals *******************************/
void test_VectorRemoveElement_AtZeroWithVectorPartiallyFull(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 0, NULL));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(values[1], *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(values[2], *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtZeroWithSinglePresentElement(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   VectorPush(vec, &value);

   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 0, NULL));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtZeroWithEmptyVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   TEST_ASSERT_FALSE(VectorRemoveElementAt(vec, 0, NULL));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtMiddle(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 1, NULL));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(values[0], *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(values[2], *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   TEST_ASSERT_FALSE(VectorRemoveElementAt(vec, VectorLength(vec), NULL));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_LastElement(void)
{
   struct Vector * vec1 = NULL;
   struct Vector * vec2 = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec1, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   VECTOR_NEW_KEEP_TRYIN(vec2, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++)
   {
      VectorPush(vec1, &values[i]);
      VectorPush(vec2, &values[i]);
   }

   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec1, 2, NULL));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec1));
   TEST_ASSERT_EQUAL_INT(values[0], *(int *)VectorGetElementAt(vec1, 0));
   TEST_ASSERT_EQUAL_INT(values[1], *(int *)VectorGetElementAt(vec1, 1));

   // Should be the same result as VectorRemoveLastElement
   TEST_ASSERT_TRUE(VectorRemoveLastElement(vec2, NULL));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec2));
   TEST_ASSERT_EQUAL_INT(values[0], *(int *)VectorGetElementAt(vec2, 0));
   TEST_ASSERT_EQUAL_INT(values[1], *(int *)VectorGetElementAt(vec2, 1));

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorRemoveElement_PastLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   TEST_ASSERT_FALSE(VectorRemoveElementAt(vec, 5, NULL));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtZeroWithVectorPartiallyFull_WithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer;
   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 0, &buffer));
   TEST_ASSERT_EQUAL_INT(42, buffer);
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(values[1], *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(values[2], *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtZeroWithSinglePresentElement_WithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int value = 42;
   VectorPush(vec, &value);

   int buffer;
   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 0, &buffer));
   TEST_ASSERT_EQUAL_INT(42, buffer);
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtZeroWithEmptyVector_WithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int buffer = 52;
   TEST_ASSERT_FALSE(VectorRemoveElementAt(vec, 0, &buffer));
   TEST_ASSERT_EQUAL_INT(52, buffer); // Confirm buffer is unchanged
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtMiddle_WithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer;
   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec, 1, &buffer));
   TEST_ASSERT_EQUAL_INT(84, buffer);
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(values[0], *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(values[2], *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorRemoveElement_AtLen_WithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer = 52;
   TEST_ASSERT_FALSE(VectorRemoveElementAt(vec, 3, &buffer));
   TEST_ASSERT_EQUAL_INT(52, buffer); // Confirm buffer is unchanged
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorRemoveElement_LastElement_WithBuf(void)
{
   struct Vector * vec1 = NULL;
   struct Vector * vec2 = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec1, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   VECTOR_NEW_KEEP_TRYIN(vec2, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++)
   {
      VectorPush(vec1, &values[i]);
      VectorPush(vec2, &values[i]);
   }

   int buffer;
   TEST_ASSERT_TRUE(VectorRemoveElementAt(vec1, 2, &buffer));
   TEST_ASSERT_EQUAL_INT(values[2], buffer);
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec1));
   TEST_ASSERT_EQUAL_INT(values[0], *(int *)VectorGetElementAt(vec1, 0));
   TEST_ASSERT_EQUAL_INT(values[1], *(int *)VectorGetElementAt(vec1, 1));

   // Should be the same result as VectorRemoveLastElement
   TEST_ASSERT_TRUE(VectorRemoveLastElement(vec2, &buffer));
   TEST_ASSERT_EQUAL_INT(values[2], buffer);
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec2));
   TEST_ASSERT_EQUAL_INT(values[0], *(int *)VectorGetElementAt(vec2, 0));
   TEST_ASSERT_EQUAL_INT(values[1], *(int *)VectorGetElementAt(vec2, 1));

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorRemoveElement_PastLen_WithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer;
   TEST_ASSERT_FALSE(VectorRemoveElementAt(vec, 5, &buffer));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

/******************************** Vector Clears *******************************/
void test_VectorClearElementAt_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   // Clear the second element
   TEST_ASSERT_TRUE(VectorClearElementAt(vec, 1));

   // Verify the cleared element is zeroed out
   int * cleared = (int *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_NOT_NULL(cleared);
   TEST_ASSERT_EQUAL_INT(0, *cleared);

   // Verify other elements remain unchanged
   int * first = (int *)VectorGetElementAt(vec, 0);
   int * third = (int *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_NOT_NULL(first);
   TEST_ASSERT_NOT_NULL(third);
   TEST_ASSERT_EQUAL_INT(42, *first);
   TEST_ASSERT_EQUAL_INT(126, *third);

   VectorFree(vec);
}

void test_VectorClearElementAt_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   // Attempt to clear an invalid index (greater than length)
   TEST_ASSERT_FALSE(VectorClearElementAt(vec, 5));
   TEST_ASSERT_FALSE(VectorClearElementAt(vec, UINT32_MAX));

   // Verify that the vector remains unchanged
   for (size_t i = 0; i < 3; i++) {
      int * element = (int *)VectorGetElementAt(vec, i);
      TEST_ASSERT_NOT_NULL(element);
      TEST_ASSERT_EQUAL_INT(values[i], *element);
   }

   VectorFree(vec);
}

/******************************** Vector Resets *******************************/

void test_VectorReset_OORIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   // Attempt to clear an out-of-range index
   TEST_ASSERT_FALSE(VectorClearElementAt(vec, 5)); // Index greater than length
   TEST_ASSERT_FALSE(VectorClearElementAt(vec, UINT32_MAX)); // Extreme out-of-range index

   // Verify that the vector remains unchanged
   for (size_t i = 0; i < 3; i++) {
      int * element = (int *)VectorGetElementAt(vec, i);
      TEST_ASSERT_NOT_NULL(element);
      TEST_ASSERT_EQUAL_INT(values[i], *element);
   }

   VectorFree(vec);
}

void test_VectorReset(void)
{
   struct Vector * vec = NULL;

   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR)
   TEST_ASSERT_NOT_NULL(vec);

   int value = 42;
   while ( VectorLength(vec) < 10 )
   {
      VectorPush(vec, &value);
   }
   TEST_ASSERT_FALSE(VectorIsEmpty(vec));

   TEST_ASSERT_TRUE(VectorReset(vec));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

/******************************** Vector Reset *******************************/
void test_VectorHardReset(void)
{
   struct Vector * vec = NULL;

   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR)
   TEST_ASSERT_NOT_NULL(vec);

   // Add some elements to the vector
   int value1 = 42, value2 = 84, value3 = 126;
   VectorPush(vec, &value1);
   VectorPush(vec, &value2);
   VectorPush(vec, &value3);

   // TODO: Figure out how we can confirm the data was zero'd out and the memory
   //       allocated for it was free'd...
   // Get pointers to these values to check later
   // int * ptr_val1;
   // int * ptr_val2;
   // int * ptr_val3;
   // ptr_val1 = VectorGetElementAt(vec, 0);
   // ptr_val2 = VectorGetElementAt(vec, 1);
   // ptr_val3 = VectorGetElementAt(vec, 2);

   // Perform a hard reset
   TEST_ASSERT_TRUE(VectorHardReset(vec));

   // Verify the vector is empty
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

/******************************* Vector Duplicate *****************************/
void test_VectorDuplicate_SmallVector(void)
{
   struct Vector * original = NULL;
   VECTOR_NEW_KEEP_TRYIN(original, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int values[] = {42, 84, 126};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(original, &values[i]);
   }

   struct Vector * duplicate = VectorDuplicate(original);
   TEST_ASSERT_NOT_NULL(duplicate);

   // Verify the duplicate matches the original
   TEST_ASSERT_EQUAL_UINT32(VectorLength(original), VectorLength(duplicate));
   TEST_ASSERT_EQUAL_UINT32(VectorCapacity(original), VectorCapacity(duplicate));
   TEST_ASSERT_EQUAL_UINT32(VectorMaxCapacity(original), VectorMaxCapacity(duplicate));
   TEST_ASSERT_EQUAL_size_t(VectorElementSize(original), VectorElementSize(duplicate));

   for (size_t i = 0; i < VectorLength(original); i++) {
      int * original_element = (int *)VectorGetElementAt(original, i);
      int * duplicate_element = (int *)VectorGetElementAt(duplicate, i);
      TEST_ASSERT_NOT_NULL(original_element);
      TEST_ASSERT_NOT_NULL(duplicate_element);
      TEST_ASSERT_EQUAL_INT(* original_element, *duplicate_element);
   }

   VectorFree(original);
   VectorFree(duplicate);
}

void test_VectorDuplicate_ReallyLargeVector(void)
{
   struct Vector * original = NULL;
   const size_t OriginalVecLen = 10000000;
   VECTOR_NEW_KEEP_TRYIN(original, sizeof(uint8_t), OriginalVecLen, OriginalVecLen, 0, &DEFAULT_ALLOCATOR);

   for (size_t i = 0; i < OriginalVecLen; i++) {
      const uint8_t val = 5;
      VectorPush(original, &val);
   }

   struct Vector * duplicate = VectorDuplicate(original);
   TEST_ASSERT_NOT_NULL(duplicate);

   // Verify the duplicate matches the original
   TEST_ASSERT_EQUAL_UINT32(VectorLength(original), VectorLength(duplicate));
   TEST_ASSERT_EQUAL_UINT32(VectorCapacity(original), VectorCapacity(duplicate));
   TEST_ASSERT_EQUAL_UINT32(VectorMaxCapacity(original), VectorMaxCapacity(duplicate));
   TEST_ASSERT_EQUAL_size_t(VectorElementSize(original), VectorElementSize(duplicate));

   for (size_t i = 0; i < VectorLength(original); i++) {
      uint8_t * original_element  = (uint8_t *)VectorGetElementAt(original, i);
      uint8_t * duplicate_element = (uint8_t *)VectorGetElementAt(duplicate, i);
      TEST_ASSERT_NOT_NULL(original_element);
      TEST_ASSERT_NOT_NULL(duplicate_element);
      TEST_ASSERT_EQUAL_UINT8(* original_element, *duplicate_element);
   }

   VectorFree(original);
   VectorFree(duplicate);
}

void test_VectorDuplicate_NullVector(void)
{
   struct Vector * original = NULL;

   struct Vector * duplicate = VectorDuplicate(original);
   TEST_ASSERT_NULL(duplicate);
}

/****************************** Vectors Are Equal *****************************/
void test_VectorsAreEqual_SameVectors(void)
{
   struct Vector * vec = NULL;
   VECTOR_NEW_KEEP_TRYIN(vec, sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   size_t iter = 0;
   int val = 10;
   while ( (VectorLength(vec) < 5) && (iter < 100) )
   {
      bool result = VectorPush(vec, &val);
      if ( result ) val += 5;
   }

   // Vector should obviously be equal to itself
   TEST_ASSERT_TRUE( VectorsAreEqual(vec, vec) );

   VectorFree(vec);
}

void test_VectorsAreEqual_DifferentElementSz(void)
{
   struct Vector * vec1 = VectorNew(sizeof(uint8_t), 10, 100, 0, &DEFAULT_ALLOCATOR);
   struct Vector * vec2 = VectorNew(sizeof(float), 10, 100, 0, &DEFAULT_ALLOCATOR);

   TEST_ASSERT_FALSE(VectorsAreEqual(vec1, vec2));

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorsAreEqual_DifferentLength(void)
{
   struct Vector * vec1 = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   struct Vector * vec2 = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int value = 42;
   VectorPush(vec1, &value);

   TEST_ASSERT_FALSE(VectorsAreEqual(vec1, vec2));

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorsAreEqual_DifferentCapacity(void)
{
   struct Vector * vec1 = VectorNew(sizeof(int), 5, 100, 0, &DEFAULT_ALLOCATOR);
   struct Vector * vec2 = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   TEST_ASSERT_FALSE(VectorsAreEqual(vec1, vec2));

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorsAreEqual_DifferentMaxCapacity(void)
{
   struct Vector * vec1 = VectorNew(sizeof(int), 10, 50, 0, &DEFAULT_ALLOCATOR);
   struct Vector * vec2 = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   TEST_ASSERT_FALSE(VectorsAreEqual(vec1, vec2));

   VectorFree(vec1);
   VectorFree(vec2);
}

void test_VectorsAreEqual_DifferentElementValues(void)
{
   struct Vector * vec1 = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   struct Vector * vec2 = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int value1 = 42, value2 = 84;
   VectorPush(vec1, &value1);
   VectorPush(vec2, &value2);

   TEST_ASSERT_FALSE(VectorsAreEqual(vec1, vec2));

   VectorFree(vec1);
   VectorFree(vec2);
}

/************************ Vector Subrange: Banana Split ***********************/

void test_VectorSplitAt_ValidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * split_vec = VectorSplitAt(vec, 2);
   TEST_ASSERT_NOT_NULL(split_vec);

   // Verify original vector
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));

   // Verify split vector
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(split_vec));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(split_vec, 0));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(split_vec, 1));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(split_vec, 2));

   VectorFree(vec);
   VectorFree(split_vec);
}

void test_VectorSplitAt_IdxZero(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * split_vec = VectorSplitAt(vec, 0);
   TEST_ASSERT_NULL(split_vec);

   // Verify original vector remains unchanged
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   for (size_t i = 0; i < 5; i++) {
      TEST_ASSERT_EQUAL_INT(values[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSplitAt_IdxPastLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * split_vec = VectorSplitAt(vec, 10);
   TEST_ASSERT_NULL(split_vec);

   // Verify original vector remains unchanged
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   for (size_t i = 0; i < 5; i++) {
      TEST_ASSERT_EQUAL_INT(values[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSplitAt_EmptyVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct Vector * split_vec = VectorSplitAt(vec, 0);
   TEST_ASSERT_NULL(split_vec);

   // Verify original vector remains unchanged
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorSplitAt_NullVector(void)
{
   struct Vector * split_vec = VectorSplitAt(NULL, 0);
   TEST_ASSERT_NULL(split_vec);
}

void test_VectorSplitAt_ValidIdx_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * split_vec = VectorSplitAt(vec, 2);
   TEST_ASSERT_NOT_NULL(split_vec);

   // Verify original vector
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_FLOAT(1.0f, ((struct MyData_S *)VectorGetElementAt(vec, 0))->x);
   TEST_ASSERT_EQUAL_FLOAT(4.0f, ((struct MyData_S *)VectorGetElementAt(vec, 1))->x);

   // Verify split vector
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(split_vec));
   TEST_ASSERT_EQUAL_FLOAT(7.0f, ((struct MyData_S *)VectorGetElementAt(split_vec, 0))->x);
   TEST_ASSERT_EQUAL_FLOAT(10.0f, ((struct MyData_S *)VectorGetElementAt(split_vec, 1))->x);
   TEST_ASSERT_EQUAL_FLOAT(13.0f, ((struct MyData_S *)VectorGetElementAt(split_vec, 2))->x);

   VectorFree(vec);
   VectorFree(split_vec);
}

/*************************** Vector Subrange: Slice ***************************/

void test_VectorSlice_ValidIndices_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   // Slice from index 1 to 3 (inclusive)
   struct Vector * slice = VectorSlice(vec, 1, 3);
   TEST_ASSERT_NOT_NULL(slice);
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(slice));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(slice, 0));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(slice, 1));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(slice, 2));

   VectorFree(vec);
   VectorFree(slice);
}

void test_VectorSlice_ValidIndices_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   // Slice from index 2 to 4 (inclusive)
   struct Vector * slice = VectorSlice(vec, 2, 4);
   TEST_ASSERT_NOT_NULL(slice);
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(slice));
   struct MyData_S * elm = (struct MyData_S *)VectorGetElementAt(slice, 0);
   TEST_ASSERT_EQUAL_FLOAT(7.0f, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(8.0f, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(9.0f, elm->z);
   elm = (struct MyData_S *)VectorGetElementAt(slice, 1);
   TEST_ASSERT_EQUAL_FLOAT(10.0f, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(11.0f, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(12.0f, elm->z);
   elm = (struct MyData_S *)VectorGetElementAt(slice, 2);
   TEST_ASSERT_EQUAL_FLOAT(13.0f, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(14.0f, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(15.0f, elm->z);

   VectorFree(vec);
   VectorFree(slice);
}

void test_VectorSlice_IdxStartEqualsIdxEnd(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   // Slice a single element
   struct Vector * slice = VectorSlice(vec, 3, 3);
   TEST_ASSERT_NOT_NULL(slice);
   TEST_ASSERT_EQUAL_UINT32(1, VectorLength(slice));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(slice, 0));

   VectorFree(vec);
   VectorFree(slice);
}

void test_VectorSlice_IdxStartZero(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   // Slice from start to index 2
   struct Vector * slice = VectorSlice(vec, 0, 2);
   TEST_ASSERT_NOT_NULL(slice);
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(slice));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(slice, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(slice, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(slice, 2));

   VectorFree(vec);
   VectorFree(slice);
}

void test_VectorSlice_FullVector(void)
{
   // Basically just like duplication
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   // Slice that is the full vector
   struct Vector * slice = VectorSlice(vec, 0, 4);
   // Duplicate, which should be equivalent
   struct Vector * dup = VectorDuplicate(vec);

   TEST_ASSERT_NOT_NULL(slice);
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(slice));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(slice, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(slice, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(slice, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(slice, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(slice, 4));

   TEST_ASSERT_NOT_NULL(dup);
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(dup));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(dup, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(dup, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(dup, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(dup, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(dup, 4));

   VectorFree(vec);
   VectorFree(slice);
   VectorFree(dup);
}

void test_VectorSlice_IdxEndAtLastElement(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   // Slice from index 2 to last element
   struct Vector * slice = VectorSlice(vec, 2, 4);
   TEST_ASSERT_NOT_NULL(slice);
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(slice));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(slice, 0));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(slice, 1));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(slice, 2));

   VectorFree(vec);
   VectorFree(slice);
}

void test_VectorSlice_EmptyVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct Vector * slice = VectorSlice(vec, 0, 0);
   TEST_ASSERT_NULL(slice);

   VectorFree(vec);
}

void test_VectorSlice_NullVector(void)
{
   struct Vector * slice = VectorSlice(NULL, 0, 0);
   TEST_ASSERT_NULL(slice);
}

void test_VectorSlice_IdxStartGreaterThanIdxEnd(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * slice = VectorSlice(vec, 3, 2);
   TEST_ASSERT_NULL(slice);

   VectorFree(vec);
}

void test_VectorSlice_IdxEndCommonMistake(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * slice = VectorSlice(vec, 1, VectorLength(vec));
   TEST_ASSERT_NULL(slice);

   VectorFree(vec);
}

void test_VectorSlice_IdxEndOutOfRange(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   struct Vector * slice = VectorSlice(vec, 1, 5);
   TEST_ASSERT_NULL(slice);

   VectorFree(vec);
}

/*********************** Vector Subrange: Concatenation ***********************/

void test_VectorConcatenate_BasicUse(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   int vals1[] = {1, 2, 3};
   int vals2[] = {4, 5};

   for (size_t i = 0; i < 3; i++) VectorPush(v1, &vals1[i]);
   for (size_t i = 0; i < 2; i++) VectorPush(v2, &vals2[i]);

   struct Vector * cat = VectorConcatenate(v1, v2);

   TEST_ASSERT_NOT_NULL(cat);
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(cat));
   TEST_ASSERT_EQUAL_UINT32(10, VectorCapacity(cat));
   TEST_ASSERT_EQUAL_UINT32(20, VectorMaxCapacity(cat));
   for (size_t i = 0; i < 3; i++)
   {
      TEST_ASSERT_EQUAL_INT(vals1[i], *(int*)VectorGetElementAt(cat, i));
   }
   for (size_t i = 0; i < 2; i++)
   {
      TEST_ASSERT_EQUAL_INT(vals2[i], *(int*)VectorGetElementAt(cat, i+3));
   }
   
   VectorFree(v1);
   VectorFree(v2);
   VectorFree(cat);
}

void test_VectorConcatenate_VecsNotMutated(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   int vals1[] = {1, 2, 3};
   int vals2[] = {4, 5};

   for (size_t i = 0; i < 3; i++) VectorPush(v1, &vals1[i]);
   for (size_t i = 0; i < 2; i++) VectorPush(v2, &vals2[i]);
   struct Vector * v1_dup = VectorDuplicate(v1);
   struct Vector * v2_dup = VectorDuplicate(v2);

   struct Vector * cat = VectorConcatenate(v1, v2);

   TEST_ASSERT_EQUAL_INT( 1, vals1[0] );
   TEST_ASSERT_EQUAL_INT( 2, vals1[1] );
   TEST_ASSERT_EQUAL_INT( 3, vals1[2] );
   TEST_ASSERT_EQUAL_INT( 3, VectorLength(v1) );
   TEST_ASSERT_EQUAL_INT( 5, VectorCapacity(v1) );
   TEST_ASSERT_EQUAL_INT( 10, VectorMaxCapacity(v1) );
   TEST_ASSERT_TRUE( VectorsAreEqual(v1, v1_dup) );

   TEST_ASSERT_EQUAL_INT( 4, vals2[0] );
   TEST_ASSERT_EQUAL_INT( 5, vals2[1] );
   TEST_ASSERT_EQUAL_INT( 2, VectorLength(v2) );
   TEST_ASSERT_EQUAL_INT( 5, VectorCapacity(v2) );
   TEST_ASSERT_EQUAL_INT( 10, VectorMaxCapacity(v2) );
   TEST_ASSERT_TRUE( VectorsAreEqual(v2, v2_dup) );
   
   VectorFree(v1); VectorFree(v1_dup);
   VectorFree(v2); VectorFree(v2_dup);
   VectorFree(cat);
}

void test_VectorConcatenate_OneVecIsEmpty(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   int vals1[] = {1, 2, 3};
   for (size_t i = 0; i < 3; i++) VectorPush(v1, &vals1[i]);
   TEST_ASSERT_TRUE(VectorIsEmpty(v2));

   struct Vector * cat = VectorConcatenate(v1, v2);

   TEST_ASSERT_NOT_NULL(cat);
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(cat));
   for (size_t i = 0; i < 3; i++)
   {
      TEST_ASSERT_EQUAL_INT(vals1[i], *(int*)VectorGetElementAt(cat, i));
   }

   VectorFree(v1);
   VectorFree(v2);
   VectorFree(cat);
}

void test_VectorConcatenate_BothVecsEmpty(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_TRUE(VectorIsEmpty(v1));
   TEST_ASSERT_TRUE(VectorIsEmpty(v2));

   struct Vector * cat = VectorConcatenate(v1, v2);

   TEST_ASSERT_NOT_NULL(cat);
   TEST_ASSERT_TRUE(VectorIsEmpty(cat));

   VectorFree(v1);
   VectorFree(v2);
   VectorFree(cat);
}

void test_VectorConcatenate_FullVectors(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 3, 3, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(int), 2, 2, 0, &DEFAULT_ALLOCATOR);
   int vals1[] = {1, 2, 3};
   int vals2[] = {4, 5};
   for (size_t i = 0; i < 3; i++) VectorPush(v1, &vals1[i]);
   for (size_t i = 0; i < 2; i++) VectorPush(v2, &vals2[i]);
   TEST_ASSERT_TRUE(VectorIsFull(v1));
   TEST_ASSERT_TRUE(VectorIsFull(v2));

   struct Vector * cat = VectorConcatenate(v1, v2);

   TEST_ASSERT_NOT_NULL(cat);
   TEST_ASSERT_EQUAL_INT(5, VectorLength(cat));
   TEST_ASSERT_TRUE(VectorIsFull(cat));

   VectorFree(v1);
   VectorFree(v2);
   VectorFree(cat);
}

void test_VectorConcatenate_NullArguments(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 2, 2, 0, &DEFAULT_ALLOCATOR);
   struct Vector * cat = VectorConcatenate(NULL, v1);
   TEST_ASSERT_NULL(cat);
   cat = VectorConcatenate(v1, NULL);
   TEST_ASSERT_NULL(cat);
   cat = VectorConcatenate(NULL, NULL);
   TEST_ASSERT_NULL(cat);
   VectorFree(v1);
}

void test_VectorConcatenate_DifferentElementSizes(void)
{
   struct Vector * v1 = VectorNew(sizeof(int), 2, 2, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(double), 2, 2, 0, &DEFAULT_ALLOCATOR);

   struct Vector * cat = VectorConcatenate(v1, v2);
   TEST_ASSERT_NULL(cat);

   VectorFree(v1);
   VectorFree(v2);
}

void test_VectorConcatenate_ConcatenateSplitRoundTrip(void)
{
   // Concatenate and split at may be considered somewhat inverse operations.
   // Check that they behave that way. Note that capacity and max capacity are
   // not inverted through these operations.
   struct Vector * v1 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   struct Vector * v2 = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   int vals1[] = {1, 2, 3};
   int vals2[] = {4, 5};

   for (size_t i = 0; i < 3; i++) VectorPush(v1, &vals1[i]);
   for (size_t i = 0; i < 2; i++) VectorPush(v2, &vals2[i]);

   struct Vector * cat = VectorConcatenate(v1, v2);
   struct Vector * v2_from_split = VectorSplitAt(cat, 3);

   TEST_ASSERT_EQUAL_INT( VectorLength(v1), VectorLength(cat) );
   for (size_t i = 0; i < 3; i++)
   {
      TEST_ASSERT_EQUAL_INT(
         *(int*)VectorGetElementAt(v1,  i),
         *(int*)VectorGetElementAt(cat, i) );
   }
   TEST_ASSERT_EQUAL_INT( VectorLength(v2), VectorLength(v2_from_split) );
   for (size_t i = 0; i < 2; i++)
   {
      TEST_ASSERT_EQUAL_INT(
         *(int*)VectorGetElementAt(v2,            i),
         *(int*)VectorGetElementAt(v2_from_split, i));
   }
   
   VectorFree(v1);
   VectorFree(v2);
   VectorFree(v2_from_split);
   VectorFree(cat);
}

/*********************** Vector Subrange: Push Elements ***********************/

void test_VectorSubRange_PushElements_ValidInts(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3};

   TEST_ASSERT_TRUE(VectorSubRange_PushElements(vec, data, 3));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));
   for (size_t i = 0; i < 3; i++)
   {
      TEST_ASSERT_EQUAL_INT(data[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSubRange_PushElements_ValidStructs(void)
{
   struct MyData_S {
      float x, y, z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 2, 10, 0, &DEFAULT_ALLOCATOR);
   struct MyData_S data[2] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f},
      { .x = 4.0f, .y = 5.0f, .z = 6.0f}
   };

   TEST_ASSERT_TRUE(VectorSubRange_PushElements(vec, data, 2));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   struct MyData_S * elm = (struct MyData_S *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_EQUAL_FLOAT(1.0f, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(2.0f, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(3.0f, elm->z);
   elm = (struct MyData_S *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_EQUAL_FLOAT(4.0f, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(5.0f, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(6.0f, elm->z);

   VectorFree(vec);
}

void test_VectorSubRange_PushElements_ExpandCapacity(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 10, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3, 4};

   TEST_ASSERT_TRUE(VectorSubRange_PushElements(vec, data, 4));
   TEST_ASSERT_EQUAL_UINT32(4, VectorLength(vec));
   for (size_t i = 0; i < 4; i++)
   {
      TEST_ASSERT_EQUAL_INT(data[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSubRange_PushElements_ZeroLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 10, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3};

   TEST_ASSERT_FALSE(VectorSubRange_PushElements(vec, data, 0));
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_PushElements_NullVec(void)
{
   int data[] = {1, 2, 3};
   TEST_ASSERT_FALSE(VectorSubRange_PushElements(NULL, data, 3));
}

void test_VectorSubRange_PushElements_NullData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 10, 0, &DEFAULT_ALLOCATOR);
   TEST_ASSERT_FALSE(VectorSubRange_PushElements(vec, NULL, 3));
   VectorFree(vec);
}

void test_VectorSubRange_PushElements_ExceedsMaxCapacity(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 4, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3, 4, 5};

   TEST_ASSERT_FALSE(VectorSubRange_PushElements(vec, data, 5));
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_PushElements_ExactlyMaxCapacity(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 4, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3, 4};

   TEST_ASSERT_TRUE(VectorSubRange_PushElements(vec, data, 4));
   TEST_ASSERT_EQUAL_UINT32(4, VectorLength(vec));
   TEST_ASSERT_EQUAL_UINT32(4, VectorCapacity(vec));
   for (size_t i = 0; i < 4; i++)
   {
      TEST_ASSERT_EQUAL_INT(data[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

/********************** Vector Subrange: Insert Elements **********************/

void test_VectorSubRange_InsertElementsAt_ValidInts(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);

   int initial[] = {1, 2, 5};
   for (size_t i = 0; i < 3; i++) 
   {
      VectorPush(vec, &initial[i]);
   }

   int insert[] = {3, 4};
   // Insert at index 2 (before 5)
   TEST_ASSERT_TRUE(VectorSubRange_InsertElementsAt(vec, 2, insert, 2));
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(1, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(2, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(3, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(4, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(5, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_ValidStructs(void)
{
   struct MyData_S { int x, y, z; };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 4, 10, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S initial[2] = { {1,2,3}, {4,5,6} };
   for (size_t i = 0; i < 2; i++)  VectorPush(vec, &initial[i]);

   struct MyData_S insert[2] = { {7,8,9}, {10,11,12} };
   // Insert at index 1
   TEST_ASSERT_TRUE(VectorSubRange_InsertElementsAt(vec, 1, insert, 2));

   TEST_ASSERT_EQUAL_UINT32(4, VectorLength(vec));

   struct MyData_S * elm = (struct MyData_S *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_EQUAL_FLOAT(7, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(8, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(9, elm->z);
   elm = (struct MyData_S *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_FLOAT(10, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(11, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(12, elm->z);
   elm = (struct MyData_S *)VectorGetElementAt(vec, 3);
   TEST_ASSERT_EQUAL_FLOAT(4, elm->x);
   TEST_ASSERT_EQUAL_FLOAT(5, elm->y);
   TEST_ASSERT_EQUAL_FLOAT(6, elm->z);

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_ExpandCapacity(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 10, 0, &DEFAULT_ALLOCATOR);

   int initial[] = {1, 2};
   for (size_t i = 0; i < 2; i++) VectorPush(vec, &initial[i]);

   int insert[] = {3, 4, 5};
   // Insert at index 1, should expand capacity
   TEST_ASSERT_TRUE(VectorSubRange_InsertElementsAt(vec, 1, insert, 3));

   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));

   TEST_ASSERT_EQUAL_INT(1, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(3, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(4, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(5, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(2, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_ZeroLen(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 10, 0, &DEFAULT_ALLOCATOR);

   int data[] = {1, 2, 3};
   VectorPush(vec, &data[0]);

   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(vec, 0, data, 0));
   TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_NullVec(void)
{
   int data[] = {1, 2, 3};
   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(NULL, 0, data, 2));
}

void test_VectorSubRange_InsertElementsAt_NullData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 10, 0, &DEFAULT_ALLOCATOR);
   VectorPush(vec, &(int){1});
   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(vec, 0, NULL, 1));

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_ExceedsMaxCapacity(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 4, 0, &DEFAULT_ALLOCATOR);

   int data[] = {1, 2, 3, 4, 5};
   VectorPush(vec, &data[0]);

   // Would exceed max capacity (1+5 > 4)
   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(vec, 0, data, 5));
   TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_ExactlyMaxCapacity(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 5, 0, &DEFAULT_ALLOCATOR);

   int data[] = {1, 2, 3};
   int val = 0;
   VectorPush(vec, &val);
   val = 4;
   VectorPush(vec, &val);

   TEST_ASSERT_TRUE(VectorSubRange_InsertElementsAt(vec, 1, data, 3));
   
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   TEST_ASSERT_EQUAL_UINT32(5, VectorCapacity(vec));
   TEST_ASSERT_TRUE(VectorIsFull(vec));

   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(1, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(2, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(3, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(4, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_InsertElementsAt_Push_Equivalence(void)
{
   struct Vector * vecpush = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);
   struct Vector * vecinsert = VectorNew(sizeof(int), 5, 10, 0, &DEFAULT_ALLOCATOR);

   int data[] = {1, 2, 3};
   int val = 0;
   VectorPush(vecpush, &val);
   VectorPush(vecinsert, &val);
   VectorSubRange_PushElements(vecpush, data, 3);
   VectorSubRange_InsertElementsAt(vecinsert, 1, data, 3);

   for ( size_t i=0; i < 4; i++ )
   {
      TEST_ASSERT_EQUAL_INT( *(int *)VectorGetElementAt(vecpush, i),
                             *(int *)VectorGetElementAt(vecinsert, i) );
   }

   VectorFree(vecpush);
   VectorFree(vecinsert);
}

void test_VectorSubRange_InsertElementsAt_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 2, 4, 0, &DEFAULT_ALLOCATOR);

   int data[] = {1, 2, 3};
   VectorPush(vec, &data[0]);

   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(vec, 2, &data[1], 2));
   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(vec, 4, &data[1], 2));
   TEST_ASSERT_FALSE(VectorSubRange_InsertElementsAt(vec, 1000, &data[1], 2));
   
   TEST_ASSERT_EQUAL_UINT32(1, VectorLength(vec));

   VectorFree(vec);
}

/************************ Vector Subrange: Get Elements ***********************/

void test_VectorSubRange_GetElementsFromIdx_ValidIdx_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int * subrange = (int *)VectorSubRange_GetElementsFromIdx(vec, 2);
   TEST_ASSERT_NOT_NULL(subrange);
   TEST_ASSERT_EQUAL_INT(30, subrange[0]);
   TEST_ASSERT_EQUAL_INT(40, subrange[1]);
   TEST_ASSERT_EQUAL_INT(50, subrange[2]);

   subrange[0] = 100;
   TEST_ASSERT_EQUAL_INT( 100, *((int *)VectorGetElementAt(vec, 2)) );

   VectorFree(vec);
}

void test_VectorSubRange_GetElementsFromIdx_ValidIdx_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 00.0f, .y = 10.0f, .z = -10.0f },
      { .x = 01.0f, .y = 20.0f, .z = -20.0f },
      { .x = 02.0f, .y = 30.0f, .z = -30.0f },
      { .x = 03.0f, .y = 40.0f, .z = -40.0f },
      { .x = 04.0f, .y = 50.0f, .z = -50.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S * subrange =
      (struct MyData_S *)VectorSubRange_GetElementsFromIdx(vec, 2);
   TEST_ASSERT_NOT_NULL(subrange);
   TEST_ASSERT_EQUAL_FLOAT( values[2].x, subrange[0].x );
   TEST_ASSERT_EQUAL_FLOAT( values[2].y, subrange[0].y );
   TEST_ASSERT_EQUAL_FLOAT( values[2].z, subrange[0].z );
   TEST_ASSERT_EQUAL_FLOAT( values[3].x, subrange[1].x );
   TEST_ASSERT_EQUAL_FLOAT( values[3].y, subrange[1].y );
   TEST_ASSERT_EQUAL_FLOAT( values[3].z, subrange[1].z );
   TEST_ASSERT_EQUAL_FLOAT( values[4].x, subrange[2].x );
   TEST_ASSERT_EQUAL_FLOAT( values[4].y, subrange[2].y );
   TEST_ASSERT_EQUAL_FLOAT( values[4].z, subrange[2].z );

   subrange[1].z = 123.456f;
   TEST_ASSERT_EQUAL_FLOAT(
      123.456f,
      ((struct MyData_S *)VectorGetElementAt(vec, 3))->z
   );

   VectorFree(vec);
}

void test_VectorSubRange_GetElementsFromIdx_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int * subrange = (int *)VectorSubRange_GetElementsFromIdx(vec, 0);
   TEST_ASSERT_NULL(subrange);

   VectorFree(vec);
}

void test_VectorSubRange_GetElementsFromIdx_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int * subrange = (int *)VectorSubRange_GetElementsFromIdx(vec, 5);
   TEST_ASSERT_NULL(subrange);

   VectorFree(vec);
}

void test_VectorSubRange_GetElementsFromIdx_InvalidVec(void)
{
   int * subrange = (int *)VectorSubRange_GetElementsFromIdx(NULL, 0);
   TEST_ASSERT_NULL(subrange);
}

/****************** Vector Subrange: Copy Elements In Range *******************/

void test_VectorSubRange_CpyElementsInRange_ValidIdices_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsInRange(vec, 1, 3, buffer));
   TEST_ASSERT_EQUAL_INT(20, buffer[0]);
   TEST_ASSERT_EQUAL_INT(30, buffer[1]);
   TEST_ASSERT_EQUAL_INT(40, buffer[2]);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsInRange_DoesNotMutate(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsInRange(vec, 1, 3, buffer));
   buffer[0] = 1;
   buffer[1] = 1;
   buffer[2] = 1;
   int * data;
   data = (int *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_EQUAL_INT(20, *data);
   data = (int *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_INT(30, *data);
   data = (int *)VectorGetElementAt(vec, 3);
   TEST_ASSERT_EQUAL_INT(40, *data);
}

void test_VectorSubRange_CpyElementsInRange_ValidIndices_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S buffer[2];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsInRange(vec, 2, 3, buffer));
   TEST_ASSERT_EQUAL_FLOAT(7.0f, buffer[0].x);
   TEST_ASSERT_EQUAL_FLOAT(8.0f, buffer[0].y);
   TEST_ASSERT_EQUAL_FLOAT(9.0f, buffer[0].z);
   TEST_ASSERT_EQUAL_FLOAT(10.0f, buffer[1].x);
   TEST_ASSERT_EQUAL_FLOAT(11.0f, buffer[1].y);
   TEST_ASSERT_EQUAL_FLOAT(12.0f, buffer[1].z);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsInRange_FullVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[5];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsInRange(vec, 0, VectorLength(vec) - 1, buffer));
   TEST_ASSERT_EQUAL_INT(10, buffer[0]);
   TEST_ASSERT_EQUAL_INT(20, buffer[1]);
   TEST_ASSERT_EQUAL_INT(30, buffer[2]);
   TEST_ASSERT_EQUAL_INT(40, buffer[3]);
   TEST_ASSERT_EQUAL_INT(50, buffer[4]);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsInRange_FullVector_IncorrectEndIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[5];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsInRange(vec, 0, VectorLength(vec), buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsInRange_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsInRange(vec, 0, 2, buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsInRange_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsInRange(vec, 2, 5, buffer)); // End index out of range
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsInRange(vec, 5, 6, buffer)); // Start index out of range
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsInRange(vec, 2, 1, buffer)); // Start index > End index

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsInRange_InvalidVec(void)
{
   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsInRange(NULL, 0, 2, buffer));
}

void test_VectorSubRange_CpyElementsInRange_SameIdices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[1];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsInRange(vec, 2, 2, buffer)); // End index out of range

   TEST_ASSERT_EQUAL_INT(30, buffer[0]);

   VectorFree(vec);
}

/************* Vector Subrange: Copy Elements From Start To Idx ***************/

void test_VectorSubRange_CpyElementsFromStartToIdx_ValidIdices_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromStartToIdx(vec, 2, buffer));
   TEST_ASSERT_EQUAL_INT(10, buffer[0]);
   TEST_ASSERT_EQUAL_INT(20, buffer[1]);
   TEST_ASSERT_EQUAL_INT(30, buffer[2]);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_DoesNotMutate(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   VectorSubRange_CpyElementsFromStartToIdx(vec, 2, buffer);
   buffer[0] = 1;
   buffer[1] = 1;
   buffer[2] = 1;
   int * data;
   data = (int *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_EQUAL_INT(10, *data);
   data = (int *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_EQUAL_INT(20, *data);
   data = (int *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_INT(30, *data);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_ValidIndices_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S buffer[3];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromStartToIdx(vec, 2, buffer));
   TEST_ASSERT_EQUAL_FLOAT(1.0f, buffer[0].x);
   TEST_ASSERT_EQUAL_FLOAT(2.0f, buffer[0].y);
   TEST_ASSERT_EQUAL_FLOAT(3.0f, buffer[0].z);
   TEST_ASSERT_EQUAL_FLOAT(4.0f, buffer[1].x);
   TEST_ASSERT_EQUAL_FLOAT(5.0f, buffer[1].y);
   TEST_ASSERT_EQUAL_FLOAT(6.0f, buffer[1].z);
   TEST_ASSERT_EQUAL_FLOAT(7.0f, buffer[2].x);
   TEST_ASSERT_EQUAL_FLOAT(8.0f, buffer[2].y);
   TEST_ASSERT_EQUAL_FLOAT(9.0f, buffer[2].z);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_FullVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[5];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromStartToIdx(vec, VectorLength(vec) - 1, buffer));
   TEST_ASSERT_EQUAL_INT(10, buffer[0]);
   TEST_ASSERT_EQUAL_INT(20, buffer[1]);
   TEST_ASSERT_EQUAL_INT(30, buffer[2]);
   TEST_ASSERT_EQUAL_INT(40, buffer[3]);
   TEST_ASSERT_EQUAL_INT(50, buffer[4]);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_FullVector_IncorrectEndIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[5];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromStartToIdx(vec, VectorLength(vec), buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromStartToIdx(vec, 2, buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromStartToIdx(vec, 1000, buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromStartToIdx_InvalidVec(void)
{
   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromStartToIdx(NULL, 2, buffer));
}

void test_VectorSubRange_CpyElementsFromStartToIdx_StartIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[1];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromStartToIdx(vec, 0, buffer));
   TEST_ASSERT_EQUAL(10, buffer[0]);

   VectorFree(vec);
}

/************** Vector Subrange: Copy Elements From Idx To End ****************/

void test_VectorSubRange_CpyElementsFromIdxToEnd_ValidIdices_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromIdxToEnd(vec, 2, buffer));
   TEST_ASSERT_EQUAL_INT(30, buffer[0]);
   TEST_ASSERT_EQUAL_INT(40, buffer[1]);
   TEST_ASSERT_EQUAL_INT(50, buffer[2]);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_DoesNotMutate(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   VectorSubRange_CpyElementsFromIdxToEnd(vec, 2, buffer);
   buffer[0] = 1;
   buffer[1] = 1;
   buffer[2] = 1;
   int * data;
   data = (int *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_INT(30, *data);
   data = (int *)VectorGetElementAt(vec, 3);
   TEST_ASSERT_EQUAL_INT(40, *data);
   data = (int *)VectorGetElementAt(vec, 4);
   TEST_ASSERT_EQUAL_INT(50, *data);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_ValidIndices_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S buffer[3];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromIdxToEnd(vec, 2, buffer));
   TEST_ASSERT_EQUAL_FLOAT(7.0f, buffer[0].x);
   TEST_ASSERT_EQUAL_FLOAT(8.0f, buffer[0].y);
   TEST_ASSERT_EQUAL_FLOAT(9.0f, buffer[0].z);
   TEST_ASSERT_EQUAL_FLOAT(10.0f, buffer[1].x);
   TEST_ASSERT_EQUAL_FLOAT(11.0f, buffer[1].y);
   TEST_ASSERT_EQUAL_FLOAT(12.0f, buffer[1].z);
   TEST_ASSERT_EQUAL_FLOAT(13.0f, buffer[2].x);
   TEST_ASSERT_EQUAL_FLOAT(14.0f, buffer[2].y);
   TEST_ASSERT_EQUAL_FLOAT(15.0f, buffer[2].z);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_FullVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[5];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromIdxToEnd(vec, 0, buffer));
   TEST_ASSERT_EQUAL_INT(10, buffer[0]);
   TEST_ASSERT_EQUAL_INT(20, buffer[1]);
   TEST_ASSERT_EQUAL_INT(30, buffer[2]);
   TEST_ASSERT_EQUAL_INT(40, buffer[3]);
   TEST_ASSERT_EQUAL_INT(50, buffer[4]);

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromIdxToEnd(vec, 0, buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromIdxToEnd(vec, 5, buffer));

   VectorFree(vec);
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_InvalidVec(void)
{
   int buffer[3];
   TEST_ASSERT_FALSE(VectorSubRange_CpyElementsFromIdxToEnd(NULL, 0, buffer));
}

void test_VectorSubRange_CpyElementsFromIdxToEnd_EndIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[1];
   TEST_ASSERT_TRUE(VectorSubRange_CpyElementsFromIdxToEnd(vec, 4, buffer));
   TEST_ASSERT_EQUAL_INT(50, buffer[0]);

   VectorFree(vec);
}

/****************** Vector Subrange: Set Elements In Range ********************/

void test_VectorSubRange_SetElementsInRange_ValidIdices_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsInRange(vec, 1, 3, new_values));

   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_DoesNotMutate(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsInRange(vec, 1, 3, new_values));

   new_values[0] = 999;
   new_values[1] = 999;
   new_values[2] = 999;

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 3));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_ValidIndices_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S new_values[2] =
   {
      { .x = 100.0f, .y = 200.0f, .z = 300.0f },
      { .x = 400.0f, .y = 500.0f, .z = 600.0f }
   };
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsInRange(vec, 2, 3, new_values));

   struct MyData_S * element = (struct MyData_S *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_FLOAT(100.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(200.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(300.0f, element->z);

   element = (struct MyData_S *)VectorGetElementAt(vec, 3);
   TEST_ASSERT_EQUAL_FLOAT(400.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(500.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(600.0f, element->z);

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_FullVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300, 400, 500};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsInRange(vec, 0, 4, new_values));

   for (size_t i = 0; i < 5; i++) {
      TEST_ASSERT_EQUAL_INT(new_values[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_FullVector_IncorrectEndIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300, 400, 500};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsInRange(vec, 0, 5, new_values));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsInRange(vec, 0, 2, new_values));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsInRange(vec, 2, 5, new_values)); // End index out of range
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsInRange(vec, 5, 6, new_values)); // Start index out of range
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsInRange(vec, 2, 1, new_values)); // Start index > End index

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsInRange_InvalidVec(void)
{
   int new_values[] = {100, 200, 300};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsInRange(NULL, 0, 2, new_values));
}

void test_VectorSubRange_SetElementsInRange_SameIdices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_value = 100;
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsInRange(vec, 2, 2, &new_value));

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 2));

   VectorFree(vec);
}

void test_VectorSubRange_RoundTrip_CpyElementsToSetElementsInRng(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3] = {0};
   VectorSubRange_CpyElementsInRange(vec, 1, 3, buffer);
   buffer[0] = 100;
   buffer[1] = 200;
   buffer[2] = 300;
   VectorSubRange_SetElementsInRange(vec, 1, 3, buffer);

   TEST_ASSERT_EQUAL_INT(10,  *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50,  *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

/**************** Vector Subrange: Set Elements Start → Idx *******************/

void test_VectorSubRange_SetElementsFromStartToIdx_ValidIdx_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromStartToIdx(vec, 2, new_values));

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_DoesNotMutate(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromStartToIdx(vec, 2, new_values));

   new_values[0] = 999;
   new_values[1] = 999;
   new_values[2] = 999;

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 2));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_ValidIdx_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S new_values[3] =
   {
      { .x = 100.0f, .y = 200.0f, .z = 300.0f },
      { .x = 400.0f, .y = 500.0f, .z = 600.0f },
      { .x = 700.0f, .y = 800.0f, .z = 900.0f }
   };
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromStartToIdx(vec, 2, new_values));

   struct MyData_S * element = (struct MyData_S *)VectorGetElementAt(vec, 0);
   TEST_ASSERT_EQUAL_FLOAT(100.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(200.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(300.0f, element->z);

   element = (struct MyData_S *)VectorGetElementAt(vec, 1);
   TEST_ASSERT_EQUAL_FLOAT(400.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(500.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(600.0f, element->z);

   element = (struct MyData_S *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_FLOAT(700.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(800.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(900.0f, element->z);

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_FullVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300, 400, 500};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromStartToIdx(vec, 4, new_values));

   for (size_t i = 0; i < 5; i++) {
      TEST_ASSERT_EQUAL_INT(new_values[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_FullVector_IncorrectEndIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300, 400, 500};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromStartToIdx(vec, 5, new_values));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromStartToIdx(vec, 2, new_values));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromStartToIdx(vec, 1000, new_values));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromStartToIdx_InvalidVec(void)
{
   int new_values[] = {100, 200, 300};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromStartToIdx(NULL, 2, new_values));
}

void test_VectorSubRange_SetElementsFromStartToIdx_StartIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_value = 100;
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromStartToIdx(vec, 0, &new_value));

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_RoundTrip_CpyElementsToSetElementsStartToIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3] = {0};
   VectorSubRange_CpyElementsFromStartToIdx(vec, 2, buffer);
   buffer[0] = 100;
   buffer[1] = 200;
   buffer[2] = 300;
   VectorSubRange_SetElementsFromStartToIdx(vec, 2, buffer);

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

/***************** Vector Subrange: Set Elements Idx → End ********************/

void test_VectorSubRange_SetElementsFromIdxToEnd_ValidIdx_IntData(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromIdxToEnd(vec, 2, new_values));

   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromIdxToEnd_DoesNotMutate(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromIdxToEnd(vec, 2, new_values));

   new_values[0] = 999;
   new_values[1] = 999;
   new_values[2] = 999;

   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromIdxToEnd_ValidIdx_StructData(void)
{
   struct MyData_S
   {
      float x;
      float y;
      float z;
   };
   struct Vector * vec = VectorNew(sizeof(struct MyData_S), 10, 100, 0, &DEFAULT_ALLOCATOR);

   struct MyData_S values[5] =
   {
      { .x = 1.0f, .y = 2.0f, .z = 3.0f },
      { .x = 4.0f, .y = 5.0f, .z = 6.0f },
      { .x = 7.0f, .y = 8.0f, .z = 9.0f },
      { .x = 10.0f, .y = 11.0f, .z = 12.0f },
      { .x = 13.0f, .y = 14.0f, .z = 15.0f }
   };
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   struct MyData_S new_values[3] =
   {
      { .x = 100.0f, .y = 200.0f, .z = 300.0f },
      { .x = 400.0f, .y = 500.0f, .z = 600.0f },
      { .x = 700.0f, .y = 800.0f, .z = 900.0f }
   };
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromIdxToEnd(vec, 2, new_values));

   struct MyData_S * element = (struct MyData_S *)VectorGetElementAt(vec, 2);
   TEST_ASSERT_EQUAL_FLOAT(100.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(200.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(300.0f, element->z);

   element = (struct MyData_S *)VectorGetElementAt(vec, 3);
   TEST_ASSERT_EQUAL_FLOAT(400.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(500.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(600.0f, element->z);

   element = (struct MyData_S *)VectorGetElementAt(vec, 4);
   TEST_ASSERT_EQUAL_FLOAT(700.0f, element->x);
   TEST_ASSERT_EQUAL_FLOAT(800.0f, element->y);
   TEST_ASSERT_EQUAL_FLOAT(900.0f, element->z);

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromIdxToEnd_FullVector(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200, 300, 400, 500};
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromIdxToEnd(vec, 0, new_values));

   for (size_t i = 0; i < 5; i++) {
      TEST_ASSERT_EQUAL_INT(new_values[i], *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromIdxToEnd_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   int new_values[] = {100, 200, 300};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromIdxToEnd(vec, 0, new_values));

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromIdxToEnd_InvalidIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   for (size_t i = 0; i < 3; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_values[] = {100, 200};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromIdxToEnd(vec, 5, new_values)); // Start index out of range

   VectorFree(vec);
}

void test_VectorSubRange_SetElementsFromIdxToEnd_InvalidVec(void)
{
   int new_values[] = {100, 200, 300};
   TEST_ASSERT_FALSE(VectorSubRange_SetElementsFromIdxToEnd(NULL, 0, new_values));
}

void test_VectorSubRange_SetElementsFromIdxToEnd_EndIdx(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int new_value = 100;
   TEST_ASSERT_TRUE(VectorSubRange_SetElementsFromIdxToEnd(vec, 4, &new_value));

   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_RoundTrip_CpyElementsToSetElementsIdxToEnd(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   for (size_t i = 0; i < 5; i++) {
      VectorPush(vec, &values[i]);
   }

   int buffer[3] = {0};
   VectorSubRange_CpyElementsFromIdxToEnd(vec, 2, buffer);
   buffer[0] = 100;
   buffer[1] = 200;
   buffer[2] = 300;
   VectorSubRange_SetElementsFromIdxToEnd(vec, 2, buffer);

   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(100, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(200, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(300, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

/*************** Vector Subrange: Remove Elements in Range ********************/

void test_VectorSubRange_RemoveElementsInRng_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50, 60};
   VectorSubRange_PushElements(vec, values, 6);

   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsInRange(vec, 2, 4, NULL));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(60, *(int *)VectorGetElementAt(vec, 2));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_NormalWithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50, 60};
   VectorSubRange_PushElements(vec, values, 6);

   int buf[3] = {0};
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsInRange(vec, 2, 4, buf));
   TEST_ASSERT_EQUAL_INT(30, buf[0]);
   TEST_ASSERT_EQUAL_INT(40, buf[1]);
   TEST_ASSERT_EQUAL_INT(50, buf[2]);
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(60, *(int *)VectorGetElementAt(vec, 2));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_AllElements(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);
   struct Vector * vec_dup = VectorDuplicate(vec);

   int buf[3] = {0};
   // Remove all elements
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsInRange(vec, 0, 2, buf));
   TEST_ASSERT_EQUAL_INT(10, buf[0]);
   TEST_ASSERT_EQUAL_INT(20, buf[1]);
   TEST_ASSERT_EQUAL_INT(30, buf[2]);
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));
   VectorReset(vec_dup);
   TEST_ASSERT_TRUE(VectorsAreEqual(vec, vec_dup));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // Try to remove from empty vector
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, 0, 1, NULL));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_AtBeginning(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Remove first two elements (indices 0, 1)
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsInRange(vec, 0, 1, NULL));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 2));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_AtEnd(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Remove last two elements (indices 3, 4)
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsInRange(vec, 3, 4, NULL));
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(vec, 2));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_InvalidIndices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // idx_start > idx_end
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, 2, 1, NULL));
   // idx_end >= len
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, 1, 3, NULL));
   // idx_start >= len
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, 3, 3, NULL));
   // Large indices
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, (size_t)-1, 1, NULL));
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, 1, (size_t)-1, NULL));
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(vec, (size_t)-1, (size_t)-1, NULL));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsInRng_InvalidVec(void)
{
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsInRange(NULL, 0, 1, NULL));
}

void test_VectorSubRange_RoundTrip_InsertAndRemove(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3, 4, 5};
   VectorSubRange_PushElements(vec, data, 5);
   struct Vector * vec_dup = VectorDuplicate(vec);

   // Insert some elements
   int insert[] = {10, 11, 12};
   VectorSubRange_InsertElementsAt(vec, 2, insert, 3);
   TEST_ASSERT_FALSE( VectorsAreEqual(vec_dup, vec) );

   // Now remove the inserted elements
   int buf[3] = {0};
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsInRange(vec, 2, 4, buf));
   TEST_ASSERT_EQUAL_INT(10, buf[0]);
   TEST_ASSERT_EQUAL_INT(11, buf[1]);
   TEST_ASSERT_EQUAL_INT(12, buf[2]);
   // Vector should be back to original
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   TEST_ASSERT_TRUE( VectorsAreEqual(vec_dup, vec) );

   VectorFree(vec);
   VectorFree(vec_dup);
}

/********** Vector Subrange: Remove Elements From Start To Idx ****************/

void test_VectorSubRange_RemoveElementsFromStartToIdx_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Remove first three elements (indices 0, 1, 2)
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsFromStartToIdx(vec, 2, NULL));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromStartToIdx_NormalWithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   int buf[3] = {0};
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsFromStartToIdx(vec, 2, buf));
   TEST_ASSERT_EQUAL_INT(10, buf[0]);
   TEST_ASSERT_EQUAL_INT(20, buf[1]);
   TEST_ASSERT_EQUAL_INT(30, buf[2]);
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromStartToIdx_AllElements(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);
   struct Vector * vec_dup = VectorDuplicate(vec);

   int buf[3] = {0};
   // Remove all elements
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsFromStartToIdx(vec, 2, buf));
   TEST_ASSERT_EQUAL_INT(10, buf[0]);
   TEST_ASSERT_EQUAL_INT(20, buf[1]);
   TEST_ASSERT_EQUAL_INT(30, buf[2]);
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));
   VectorReset(vec_dup);
   TEST_ASSERT_TRUE(VectorsAreEqual(vec, vec_dup));

   VectorFree(vec);
   VectorFree(vec_dup);
}

void test_VectorSubRange_RemoveElementsFromStartToIdx_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // Try to remove from empty vector
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromStartToIdx(vec, 0, NULL));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromStartToIdx_InvalidIndices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // Typical 1-based idx misuse
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromStartToIdx(vec, 3, NULL));
   // Large index
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromStartToIdx(vec, (size_t)-1, NULL));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromStartToIdx_InvalidVec(void)
{
   int buf[3] = {0};
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromStartToIdx(NULL, 0, buf));
}

/*********** Vector Subrange: Remove Elements From Idx To End *****************/

void test_VectorSubRange_RemoveElementsFromIdxToEnd_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Remove from index 2 to end (removes 30, 40, 50)
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsFromIdxToEnd(vec, 2, NULL));
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromIdxToEnd_NormalWithBuf(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   int buf[3] = {0};
   // Remove from index 2 to end (removes 30, 40, 50)
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsFromIdxToEnd(vec, 2, buf));
   TEST_ASSERT_EQUAL_INT(30, buf[0]);
   TEST_ASSERT_EQUAL_INT(40, buf[1]);
   TEST_ASSERT_EQUAL_INT(50, buf[2]);
   TEST_ASSERT_EQUAL_UINT32(2, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromIdxToEnd_AllElements(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);
   struct Vector * vec_dup = VectorDuplicate(vec);

   int buf[3] = {0};
   // Remove all elements (idx = 0)
   TEST_ASSERT_TRUE(VectorSubRange_RemoveElementsFromIdxToEnd(vec, 0, buf));
   TEST_ASSERT_EQUAL_INT(10, buf[0]);
   TEST_ASSERT_EQUAL_INT(20, buf[1]);
   TEST_ASSERT_EQUAL_INT(30, buf[2]);
   TEST_ASSERT_EQUAL_UINT32(0, VectorLength(vec));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));
   VectorReset(vec_dup);
   TEST_ASSERT_TRUE(VectorsAreEqual(vec, vec_dup));

   VectorFree(vec);
   VectorFree(vec_dup);
}

void test_VectorSubRange_RemoveElementsFromIdxToEnd_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // Try to remove from empty vector
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromIdxToEnd(vec, 0, NULL));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromIdxToEnd_InvalidIndices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // idx >= len
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromIdxToEnd(vec, 3, NULL));
   // Large index
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromIdxToEnd(vec, (size_t)-1, NULL));

   VectorFree(vec);
}

void test_VectorSubRange_RemoveElementsFromIdxToEnd_InvalidVec(void)
{
   int buf[3] = {0};
   TEST_ASSERT_FALSE(VectorSubRange_RemoveElementsFromIdxToEnd(NULL, 0, buf));
}

void test_VectorSubRange_RoundTrip_PushElementsRemoveElementsFromIdxToEnd(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int data[] = {1, 2, 3, 4, 5};
   VectorSubRange_PushElements(vec, data, 5);
   struct Vector * vec_dup = VectorDuplicate(vec);

   int datapush[] = {6, 7};
   VectorSubRange_PushElements(vec, datapush, 2);
   VectorSubRange_RemoveElementsFromIdxToEnd(vec, 5, NULL);
   TEST_ASSERT_TRUE(VectorsAreEqual(vec, vec_dup));

   VectorFree(vec);
   VectorFree(vec_dup);
}

/*************** Vector Subrange: Clear Elements in Range *********************/

void test_VectorSubRange_ClearElementsInRng_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50, 60};
   VectorSubRange_PushElements(vec, values, 6);

   // Clear elements at indices 2, 3, 4 (set to zero)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsInRange(vec, 2, 4));
   TEST_ASSERT_EQUAL_UINT32(6, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 4));
   TEST_ASSERT_EQUAL_INT(60, *(int *)VectorGetElementAt(vec, 5));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsInRng_AllElements(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // Clear all elements
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsInRange(vec, 0, 2));
   for (size_t i = 0; i < 3; i++) {
      TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, i));
   }
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsInRng_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // Try to clear on empty vector
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, 0, 1));
   TEST_ASSERT_TRUE(VectorIsEmpty(vec));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsInRng_AtBeginning(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Clear first two elements (indices 0, 1)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsInRange(vec, 0, 1));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsInRng_AtEnd(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Clear last two elements (indices 3, 4)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsInRange(vec, 3, 4));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(30, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsInRng_InvalidIndices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // idx_start > idx_end
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, 2, 1));
   // idx_end >= len
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, 1, 3));
   // idx_start >= len
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, 3, 3));
   // Large indices
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, (size_t)-1, 1));
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, 1, (size_t)-1));
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(vec, (size_t)-1, (size_t)-1));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsInRng_InvalidVec(void)
{
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsInRange(NULL, 0, 1));
}

/********** Vector Subrange: Clear Elements From Start To Idx *****************/

void test_VectorSubRange_ClearElementsFromStartToIdx_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Clear first three elements (indices 0, 1, 2)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsFromStartToIdx(vec, 2));
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(40, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(50, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromStartToIdx_AllElements(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // Clear all elements (idx = 2)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsFromStartToIdx(vec, VectorLength(vec) - 1));
   for (size_t i = 0; i < 3; i++) {
      TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, i));
   }
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromStartToIdx_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // Try to clear on empty vector
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromStartToIdx(vec, 0));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromStartToIdx_InvalidIndices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // idx >= len
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromStartToIdx(vec, 3));
   // Large index
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromStartToIdx(vec, (size_t)-1));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromStartToIdx_InvalidVec(void)
{
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromStartToIdx(NULL, 0));
}

/********** Vector Subrange: Clear Elements From Start To Idx *****************/

void test_VectorSubRange_ClearElementsFromIdxToEnd_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   // Clear from index 2 to end (indices 2, 3, 4)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsFromIdxToEnd(vec, 2));
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   TEST_ASSERT_EQUAL_INT(10, *(int *)VectorGetElementAt(vec, 0));
   TEST_ASSERT_EQUAL_INT(20, *(int *)VectorGetElementAt(vec, 1));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 2));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 3));
   TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, 4));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromIdxToEnd_AllElements(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // Clear all elements (idx = 0)
   TEST_ASSERT_TRUE(VectorSubRange_ClearElementsFromIdxToEnd(vec, 0));
   for (size_t i = 0; i < 3; i++) {
      TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, i));
   }
   TEST_ASSERT_EQUAL_UINT32(3, VectorLength(vec));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromIdxToEnd_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   // Try to clear on empty vector
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromIdxToEnd(vec, 0));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromIdxToEnd_InvalidIndices(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30};
   VectorSubRange_PushElements(vec, values, 3);

   // idx >= len
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromIdxToEnd(vec, 3));
   // Large index
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromIdxToEnd(vec, (size_t)-1));

   VectorFree(vec);
}

void test_VectorSubRange_ClearElementsFromIdxToEnd_InvalidVec(void)
{
   TEST_ASSERT_FALSE(VectorSubRange_ClearElementsFromIdxToEnd(NULL, 0));
}

/********** Vector Subrange: Clear Elements From Start To Idx *****************/

void test_VectorClearAllElements_Normal(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);
   int values[] = {10, 20, 30, 40, 50};
   VectorSubRange_PushElements(vec, values, 5);

   TEST_ASSERT_TRUE(VectorClearAll(vec));
   TEST_ASSERT_EQUAL_UINT32(5, VectorLength(vec));
   for (size_t i = 0; i < 5; i++) {
      TEST_ASSERT_EQUAL_INT(0, *(int *)VectorGetElementAt(vec, i));
   }

   VectorFree(vec);
}

void test_VectorClearAllElements_EmptyVec(void)
{
   struct Vector * vec = VectorNew(sizeof(int), 10, 100, 0, &DEFAULT_ALLOCATOR);

   TEST_ASSERT_FALSE(VectorClearAll(vec));

   VectorFree(vec);
}

void test_VectorClearAllElements_InvalidVec(void)
{
   TEST_ASSERT_FALSE(VectorClearAll(NULL));
}
