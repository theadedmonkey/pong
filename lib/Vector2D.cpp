#include "Vector2D.h"
#include <math.h>

// constructors
Vector2D::Vector2D() {}
Vector2D::Vector2D(float nx, float ny) : x(nx), y(ny) {}

// zero
void Vector2D::zero() {
  x = y = 0.0f;
}

// equality
bool Vector2D::operator==(const Vector2D &v2) const {
  return x == v2.x && y == v2.y;
}
bool Vector2D::operator!=(const Vector2D &v2) const {
  return x != v2.x || y != v2.y;
}

// negative
Vector2D Vector2D::operator -() const {
  return Vector2D(-x, -y);
}

// binary addition
Vector2D Vector2D::operator+(const Vector2D &v2) const {
  return Vector2D(x + v2.x, y + v2.y);
}

// binary subtraction
Vector2D Vector2D::operator-(const Vector2D &v2) const {
  return Vector2D(x - v2.x, y - v2.y);
}

// scalar multiplication
Vector2D Vector2D::operator*(float scalar) const {
  return Vector2D(x * scalar, y * scalar);
}

// scalar division
Vector2D Vector2D::operator/(float scalar) const {
  /*
   * NOTE: no check for division by 0
   */
  float oneOverScalar = 1.0f / scalar;
  return Vector2D(x * oneOverScalar, y * oneOverScalar);
}

// combined addition
Vector2D Vector2D::operator+=(const Vector2D& v2) {
  x += v2.x;
  y += v2.y;
  return *this;
}

// combined subtraction
Vector2D Vector2D::operator-=(const Vector2D& v2) {
  x -= v2.x;
  y -= v2.y;
  return *this;
}

// combined scalar multiplication
Vector2D Vector2D::operator*=(float scalar) {
  x *= scalar;
  y *= scalar;
  return *this;
}

// combined scalar division
Vector2D Vector2D::operator/=(float scalar) {
  /*
   * NOTE: no check for division by 0
   */
  float oneOverScalar = 1.0f / scalar;
  x *= oneOverScalar;
  y *= oneOverScalar;
  return *this;
}

// dot product
float Vector2D::operator*(const Vector2D &v2) const {
  return x * v2.x + y * v2.y;
}

// normalize
void Vector2D::normalize() {
  float magnitudeSquared = x*x + y *y;
  if (magnitudeSquared > 0.0f) {
    float oneOverMagnitude = 1.0f / sqrt(magnitudeSquared);
    x *= oneOverMagnitude;
    y *= oneOverMagnitude;
  }
}

// magnitude
float Vector2D::magnitude() {
  return sqrt(x*x + y*y);
}

// distance
float Vector2D::distance(const Vector2D &v2) {
  float dx = x - v2.x;
  float dy = y - v2.y;
  return sqrt(dx * dx + dy * dy);
}
