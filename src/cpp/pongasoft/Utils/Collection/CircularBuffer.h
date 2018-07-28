#ifndef __PONGASOFT_UTILS_COLLECTION_CIRCULAR_BUFFER_H__
#define __PONGASOFT_UTILS_COLLECTION_CIRCULAR_BUFFER_H__

#include <cassert>
#include <memory>

namespace pongasoft {
namespace Utils {
namespace Collection {

template<typename T>
class CircularBuffer
{
public:
  explicit CircularBuffer(int iSize) : fSize(iSize), fStart(0)
  {
    assert(fSize > 0);

    fBuf = new T[iSize];
  };

  CircularBuffer(CircularBuffer const& iOther) : fSize(iOther.fSize), fStart(iOther.fStart)
  {
    fBuf = new T[fSize];
    memcpy(fBuf, iOther.fBuf, fSize * sizeof(T));
  }

  ~CircularBuffer()
  {
    delete[] fBuf;
  }

  // handle negative offsets as well
  inline int getSize() const
  {
    return fSize;
  }

  inline T getAt(int offset) const
  {
    return fBuf[adjustIndexFromOffset(offset)];
  }

  inline void setAt(int offset, T e)
  {
    fBuf[adjustIndexFromOffset(offset)] = e;
  };

  inline void incrementHead()
  {
    fStart = adjustIndex(fStart + 1);
  }

  inline void push(T e)
  {
    setAt(0, e);
    incrementHead();
  }

  inline void init(T initValue)
  {
    for(int i = 0; i < fSize; ++i)
    {
      fBuf[i] = initValue;
    }
  }

  inline void copyToBuffer(int startOffset, T *oBuffer, int iSize)
  {
    int adjStartOffset = adjustIndexFromOffset(startOffset);

    if(adjStartOffset + iSize < fSize)
    {
      memcpy(oBuffer, &fBuf[adjStartOffset], iSize * sizeof(T));
    }
    else
    {
      int i = adjStartOffset;
      for(int k = 0; k < iSize; k++)
      {
        oBuffer[k] = fBuf[i];
        i++;
        if(i == fSize)
          i = 0;
      }
    }
  }

  /*
   * Technically speaking the BinaryPredicate is defined like this
   *  template<typename U>
   *  using BinaryPredicate = U (*)(U const&, T const&);
   *
   *  BUT it is not really defined in the following methods because of the fact that lambda with capture
   *  cannot be converted to it... where by defining it loosely this way, it just works...
   */

  /**
   * "standard" implementation of the fold algorithm starting at startOffset and ending at endOffsetNotIncluded (which
   * as the name hinted is NOT included). This implementation works fine whether startOffset is less or more than
   * endOffsetNotIncluded (in which case the direction of iteration is reversed). It also handles properly wrapping
   * around the buffer (potentially multiple times...).
   *
   * The fold algorithm is the following:
   * resultValue = initValue;
   * resultValue = op(resultValue, fBuf[<adjusted start offset>]);
   * resultValue = op(resultValue, fBuf[<adjusted start offset> + 1]);
   * ...
   * return resultValue;
   */
  template<typename U, class BinaryPredicate>
  inline U fold(int startOffset, int endOffsetNotIncluded, U initValue, BinaryPredicate &op) const
  {
    if(startOffset == endOffsetNotIncluded)
      return initValue;

    U resultValue = initValue;

    int i = adjustIndexFromOffset(startOffset);

    if(startOffset < endOffsetNotIncluded)
    {
      int size = endOffsetNotIncluded - startOffset;
      while(size > 0)
      {
        resultValue = op(resultValue, fBuf[i]);
        ++i;
        if(i == fSize)
          i = 0;
        size--;
      }
    }
    else
    {
      int size = startOffset - endOffsetNotIncluded;
      while(size > 0)
      {
        resultValue = op(resultValue, fBuf[i]);
        --i;
        if(i == -1)
          i = fSize - 1;
        size--;
      }
    }

    return resultValue;
  }

  /**
   * Shortcut with startOffset 0
   */
  template<typename U, class BinaryPredicate>
  inline U fold(int endOffsetNotIncluded, U initValue, BinaryPredicate &op) const
  {
    return fold(0, endOffsetNotIncluded, initValue, op);
  }

  /**
   * Shortcut for entire buffer (starting at startOffset 0)
   */
  template<typename U, class BinaryPredicate>
  inline U fold(U initValue, BinaryPredicate &op) const
  {
    return fold(0, fSize, initValue, op);
  }

  /**
 * Similar to fold but BinaryPredicateWithIndex is also provided the index (starting at startOffset)
 *  template<typename U>
 *  using BinaryPredicateWithIndex = U (*)(int, U const&, T const&);
 */
  template<typename U, class BinaryPredicateWithIndex>
  inline U foldWithIndex(int startOffset, int endOffsetNotIncluded, U initValue, BinaryPredicateWithIndex &op) const
  {
    if(startOffset == endOffsetNotIncluded)
      return initValue;

    U resultValue = initValue;

    int i = adjustIndexFromOffset(startOffset);
    int index = startOffset;

    if(startOffset < endOffsetNotIncluded)
    {
      int size = endOffsetNotIncluded - startOffset;
      while(size > 0)
      {
        resultValue = op(index, resultValue, fBuf[i]);
        ++i;
        if(i == fSize)
          i = 0;
        size--;
        index++;
      }
    }
    else
    {
      int size = startOffset - endOffsetNotIncluded;
      while(size > 0)
      {
        resultValue = op(index, resultValue, fBuf[i]);
        --i;
        if(i == -1)
          i = fSize - 1;
        size--;
        index--;
      }
    }

    return resultValue;
  }

  /**
   * Shortcut with startOffset = 0
   */
  template<typename U, class BinaryPredicateWithIndex>
  inline U foldWithIndex(int endOffsetNotIncluded, U initValue, BinaryPredicateWithIndex &op) const
  {
    return foldWithIndex(0, endOffsetNotIncluded, initValue, op);
  }

  /**
   * Shortcut for entire buffer (starting at startOffset 0)
   */
  template<typename U, class BinaryPredicateWithIndex>
  inline U foldWithIndex(U initValue, BinaryPredicateWithIndex &op) const
  {
    return foldWithIndex(0, fSize, initValue, op);
  }

private:
  inline int adjustIndexFromOffset(int offset) const
  {
    if(offset == 0)
      return fStart;

    return adjustIndex(fStart + offset);
  }

  inline int adjustIndex(int index) const
  {
    // shortcut since this is a frequent use case
    if(index == fSize)
      return 0;

    while(index < 0)
      index += fSize;

    while(index >= fSize)
      index -= fSize;

    return index;
  }

  int fSize;
  T *fBuf;
  int fStart;
};

}
}
}

#endif // __PONGASOFT_UTILS_COLLECTION_CIRCULAR_BUFFER_H__