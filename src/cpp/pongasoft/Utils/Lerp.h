#pragma once

namespace pongasoft {
namespace Utils {
/**
 * Util class to compute linear interpolation. Note that assembly code totally removes this class which is great!
 * ex: Utils::Lerp(MAX_ZOOM_FACTOR_Y, 1.0).computeY(fPropZoomFactorY.getValue());
 */
template <typename T>
class Lerp
{
public:
  Lerp(T iX1, T iY1, T iX2, T iY2) : fA((iY1 - iY2) / (iX1 - iX2)), fB(iY1 - fA * iX1) {}

  /**
   * Shortcut for when x=0 => iY0 and x=1.0 => iY1
   */
  Lerp(T iY0, T iY1) : fA(iY1 - iY0), fB(iY0) {};

  inline T computeY(T iX) const
  {
    return (iX * fA) + fB;
  }

  inline T computeX(T iY) const
  {
    DCHECK_F(fA != 0);
    return (iY - fB) / fA;
  }

private:
  const T fA;
  const T fB;
};
}
}
