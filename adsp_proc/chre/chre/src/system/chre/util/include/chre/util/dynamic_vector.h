/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CHRE_UTIL_DYNAMIC_VECTOR_H_
#define CHRE_UTIL_DYNAMIC_VECTOR_H_

#include <cstddef>

#include "chre/util/non_copyable.h"

namespace chre {

/**
 * A container for storing a sequential array of elements. This container
 * resizes dynamically using heap allocations.
 */
template<typename ElementType>
class DynamicVector : public NonCopyable {
 public:
  /**
   * Random-access iterator that points to some element in the container.
   */
  typedef ElementType* iterator;
  typedef const ElementType* const_iterator;

  typedef size_t size_type;

  /**
   * Default-constructs a dynamic vector.
   */
  DynamicVector();

  /**
   * Move-constructs a dynamic vector from another. The other dynamic vector is
   * left in an empty state.
   *
   * @param other The other vector to move from.
   */
  DynamicVector(DynamicVector<ElementType>&& other);

  /**
   * Move-constructs a dynamic vector from another. The other dynamic vector is
   * left in an empty state.
   */
  DynamicVector& operator=(DynamicVector<ElementType>&& other);

  /**
   * Destructs the objects and releases the memory owned by the vector.
   */
  ~DynamicVector();

  /**
   * Removes all elements from the vector, but does not change the capacity.
   * All iterators and references are invalidated.
   */
  void clear();

  /**
   * Returns a pointer to the underlying buffer. Note that this should not be
   * considered to be persistent as the vector will be moved and resized
   * automatically.
   *
   * @return The pointer to the underlying buffer.
   */
  ElementType *data();

  /**
   * Returns a const pointer to the underlying buffer. Note that this should not
   * be considered to be persistent as the vector will be moved and resized
   * automatically.
   *
   * @return The const pointer to the underlying buffer.
   */
  const ElementType *data() const;

  /**
   * Returns the current number of elements in the vector.
   *
   * @return The number of elements in the vector.
   */
  size_type size() const;

  /**
   * Returns the maximum number of elements that can be stored in this vector
   * without a resize operation.
   *
   * @return The capacity of the vector.
   */
  size_type capacity() const;

  /**
   * Determines whether the vector is empty or not.
   *
   * @return true if the vector is empty.
   */
  bool empty() const;

  /**
   * Erases the last element in the vector. Invalid to call on an empty vector.
   *
   * Invalidates any references to back() and end()/cend().
   */
  void pop_back();

  /**
   * Copy- or move-constructs an element onto the back of the vector. If the
   * vector requires a resize and that allocation fails this function will
   * return false. All iterators and references are invalidated if the container
   * has been resized. Otherwise, only the past-the-end iterator is invalidated.
   *
   * @param The element to push onto the vector.
   * @return true if the element was pushed successfully.
   */
  bool push_back(const ElementType& element);
  bool push_back(ElementType&& element);

  /**
   * Constructs an element onto the back of the vector. All iterators and
   * references are invalidated if the container has been resized. Otherwise,
   * only the past-the-end iterator is invalidated.
   *
   * @param The arguments to the constructor
   * @return true if the element is constructed successfully.
   */
  template<typename... Args>
  bool emplace_back(Args&&... args);

  /**
   * Obtains an element of the vector given an index. It is illegal to index
   * this vector out of bounds and the user of the API must check the size()
   * function prior to indexing this vector to ensure that they will not read
   * out of bounds.
   *
   * @param The index of the element.
   * @return The element.
   */
  ElementType& operator[](size_type index);

  /**
   * Obtains a const element of the vector given an index. It is illegal to
   * index this vector out of bounds and the user of the API must check the
   * size() function prior to indexing this vector to ensure that they will not
   * read out of bounds.
   *
   * @param The index of the element.
   * @return The element.
   */
  const ElementType& operator[](size_type index) const;

  /**
   * Compares two vectors for equality. It will compare the vector sizes and
   * return false if those are different; if not, it will compare the contents
   * of the vectors element-by-element. The operator == should be defined and
   * meaningful for the vector's element type.
   *
   * @param Right-hand side vector to compared with.
   * @return true if two vectors are equal, false otherwise.
   */
  bool operator==(const DynamicVector<ElementType>& other) const;

  /**
   * Resizes the vector to a new capacity returning true if allocation was
   * successful. If the new capacity is smaller than the current capacity,
   * the operation is a no-op and true is returned. If a memory allocation
   * fails, the contents of the vector are not modified and false is returned.
   * This is intended to be similar to the reserve function of the std::vector.
   * All iterators and references are invalidated unless the container did not
   * resize.
   *
   * @param The new capacity of the vector.
   * @return true if the resize operation was successful.
   */
  bool reserve(size_type newCapacity);

  /**
   * Resizes the vector to a new size. If the new capacity is smaller than the
   * current size, the extraneous objects at the end are destructed. If the new
   * capacity is larger than the current size, the new objects are
   * default-constructed.
   *
   * @param size The new size of the vector.
   * @return true if the resize operation was successful.
   */
  bool resize(size_type newSize);

  /**
   * Inserts an element into the vector at a given index. If a resize of the
   * vector is required and the allocation fails, false will be returned. This
   * will shift all vector elements after the given index one position backward
   * in the list. The supplied index must be <= the size of the vector. It is
   * not possible to have a sparse list of items. If the index is > the current
   * size of the vector, false will be returned. All iterators and references
   * to and after the indexed element are invalidated. Iterators and references
   * to before the indexed elements are unaffected if the container did not resize.
   *
   * @param index The index to insert an element at.
   * @param element The element to insert.
   * @return Whether or not the insert operation was successful.
   */
  bool insert(size_type index, const ElementType& element);
  bool insert(size_type index, ElementType&& element);

  /**
   * Similar to wrap(), except makes a copy of the supplied C-style array,
   * maintaining ownership of the buffer within the DynamicVector container. The
   * vector's capacity is increased if necessary to fit the given array, though
   * note that this function will not cause the capacity to shrink. Upon
   * successful reservation of necessary capacity, any pre-existing items in the
   * vector are removed (via clear()), the supplied array is copied, and the
   * vector's size is set to elementCount. All iterators and references are
   * invalidated unless the container did not resize.
   *
   * This is essentially equivalent to calling these functions from std::vector:
   *   vector.clear();
   *   vector.insert(vector.begin(), array, &array[elementCount]);
   *
   * This function is not valid to call on a vector where owns_data() is false.
   * Use unwrap() first in that case.
   *
   * @param array Pointer to the start of an array
   * @param elementCount Number of elements in the supplied array to copy
   *
   * @return true if capacity was reserved to fit the supplied array (or the
   *         vector already had sufficient capacity), and the supplied array was
   *         copied into the vector. If false, the vector is not modified.
   */
  bool copy_array(const ElementType *array, size_type elementCount);

  /**
   * Removes an element from the vector given an index. All elements after the
   * indexed one are moved forward one position. The destructor is invoked on
   * on the invalid item left at the end of the vector. The index passed in
   * must be less than the size() of the vector. If the index is greater than or
   * equal to the size no operation is performed. All iterators and references
   * to and after the indexed element are invalidated.
   *
   * @param index The index to remove an element at.
   */
  void erase(size_type index);

  /**
   * Searches the vector for an element.
   *
   * @param element The element to comare against.
   * @return The index of the element found. If the return is equal to size()
   *         then the element was not found.
   */
  size_type find(const ElementType& element) const;

  /**
   * Swaps the location of two elements stored in the vector. The indices
   * passed in must be less than the size() of the vector. If the index is
   * greater than or equal to the size, no operation is performed. All
   * iterators and references to these two indexed elements are invalidated.
   *
   * @param index0 The index of the first element
   * @param index1 The index of the second element
   */
  void swap(size_type index0, size_type index1);

  /**
   * Wraps an existing C-style array so it can be used as a DynamicVector. A
   * reference to the supplied array is kept, as opposed to making a copy. The
   * caller retains ownership of the memory. Calling code must therefore ensure
   * that the lifetime of the supplied array is at least as long as that of this
   * vector, and that the memory is released after this vector is destructed, as
   * the vector will not attempt to free the memory itself.
   *
   * Once a vector wraps another buffer, it cannot be resized except through
   * another call to wrap(). However, elements can be erased to make room for
   * adding new elements.
   *
   * Destruction of elements within a wrapped array remains the responsibility
   * of the calling code. While the vector may invoke the element destructor as
   * a result of explicit calls to functions like erase() or clear(), it will
   * not destruct elements remaining in the array when the vector is destructed.
   * Therefore, special care must be taken when wrapping an array of elements
   * that have a non-trivial destructor.
   *
   * @param array Pointer to a pre-allocated array
   * @param elementCount Number of elements in the array (NOT the array's size
   *        in bytes); will become the vector's size() and capacity()
   */
  void wrap(ElementType *array, size_type elementCount);


  /**
   * Returns a vector that is wrapping an array to the newly-constructed state,
   * with capacity equal to 0, and owns_data() is true.
   */
  void unwrap();

  /**
   * @return false if this vector is wrapping an array passed in via wrap()
   */
  bool owns_data() const;

  /**
   * Returns a reference to the first element in the vector. It is illegal to
   * call this on an empty vector.
   *
   * @return The first element in the vector.
   */
  ElementType& front();

  /**
   * Returns a const reference to the first element in the vector. It is illegal
   * to call this on an empty vector.
   *
   * @return The first element in the vector.
   */
  const ElementType& front() const;

  /**
   * Returns a reference to the last element in the vector. It is illegal to
   * call this on an empty vector.
   *
   * @return The last element in the vector.
   */
  ElementType& back();

  /**
   * Returns a const reference to the last element in the vector. It is illegal
   * to call this on an empty vector.
   *
   * @return The last element in the vector.
   */
  const ElementType& back() const;

  /**
   * Prepares a vector to push a minimum of one element onto the back. The
   * vector may be resized if required. The capacity of the vector increases
   * with the growth policy of this vector (doubles for each resize for now).
   *
   * @return Whether or not the resize was successful.
   */
  bool prepareForPush();

  /**
   * @return A random-access iterator to the beginning.
   */
  typename DynamicVector<ElementType>::iterator begin();
  typename DynamicVector<ElementType>::const_iterator begin() const;
  typename DynamicVector<ElementType>::const_iterator cbegin() const;

  /**
   * @return A random-access iterator to the end.
   */
  typename DynamicVector<ElementType>::iterator end();
  typename DynamicVector<ElementType>::const_iterator end() const;
  typename DynamicVector<ElementType>::const_iterator cend() const;

 private:
  //! A pointer to the underlying data buffer.
  ElementType *mData = nullptr;

  //! The current size of the vector, as in the number of elements stored.
  size_t mSize = 0;

  //! The current capacity of the vector, as in the maximum number of elements
  //! that can be stored.
  size_t mCapacity = 0;

  //! Set to true when the buffer (mData) was supplied via wrap()
  bool mDataIsWrapped = false;

  /**
   * Prepares the vector for insertion - upon successful return, the memory at
   * the given index will be allocated but uninitialized
   *
   * @param index
   * @return true
   */
  bool prepareInsert(size_t index);
};

}  // namespace chre

#include "chre/util/dynamic_vector_impl.h"

#endif  // CHRE_UTIL_DYNAMIC_VECTOR_H_
