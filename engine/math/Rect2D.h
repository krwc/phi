#ifndef PHI_MATH_RECT_2D_H
#define PHI_MATH_RECT_2D_H

namespace phi {

/**
 * Defines rectangular area starting at given (x,y) and extending to (x+w, y+h)
 */
struct Rect2D {
    int x, y, w, h;
};

} // namespace phi

#endif
