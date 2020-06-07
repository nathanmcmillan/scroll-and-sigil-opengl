#ifndef QUATERNION_H
#define QUATERNION_H

#include <math.h>

#include "math_util.h"
#include "vector.h"

typedef float quaternion;

void quaternion_identity(quaternion *out);

void euler_to_quaternion(quaternion *out, float x, float y, float z);
void quaternion_to_euler(vec3 *out, quaternion *in);
void quaternion_multiply(quaternion *out, quaternion *a, quaternion *b);
float quaternion_dot(quaternion *a, quaternion *b);
float quaternion_magnitude(quaternion *out);
void quaternion_normalize(quaternion *out);
void quaternion_negate(quaternion *out, quaternion *in);
void quaternion_scale(quaternion *out, quaternion *in, float scale);
void quaternion_conjugate(quaternion *out, quaternion *in);
void quaternion_inverse(quaternion *out, quaternion *in);
void rotation_and_position_to_matrix(float *matrix, quaternion *rotation, float *position);

#endif
