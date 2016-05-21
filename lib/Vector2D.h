#ifndef VECTOR2D_H
#define VECTOR2D_H

/*
 * TODO:
 * = operator
 * empty constructor
 * copy constructor
 * scalar left multiplication: i.e 2 * v
 * cross product
 */
class Vector2D {
  public:
    float x, y;

    // constructors
    Vector2D();
    Vector2D(float nx, float ny);

    // zero
    void zero();

    // equality
    bool operator==(const Vector2D &v2) const;
    bool operator!=(const Vector2D &v2) const;

    // negative
    Vector2D operator -() const;

    // binary addition
    Vector2D operator+(const Vector2D &v2) const;

    // binary subtraction
    Vector2D operator-(const Vector2D &v2) const;

    // scalar multiplication
    Vector2D operator*(float scalar) const;

    // scalar division
    Vector2D operator/(float scalar) const;

    // combined addition
    Vector2D operator+=(const Vector2D& v2);

    // combined subtraction
    Vector2D operator-=(const Vector2D& v2);

    // combined scalar multiplication
    Vector2D operator*=(float scalar);

    // combined scalar division
    Vector2D operator/=(float scalar);

    // dot product
    float operator*(const Vector2D &v2) const;

    // normalize
    void normalize();

    // magnitude
    float magnitude();

    // distance
    float distance(const Vector2D &v2);

};

#endif
