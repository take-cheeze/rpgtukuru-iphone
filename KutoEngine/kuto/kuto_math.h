/**
 * @file
 * @brief Math Library
 * @author project.kuto
 */
#pragma once

#include "kuto_vector2.h"
#include "kuto_vector3.h"
#include "kuto_vector4.h"
#include "kuto_matrix.h"
#include "kuto_color.h"
#include "kuto_viewport.h"
#include "kuto_point2.h"

template<typename T>
inline float distance(T const& a, T const& b) { return (a - b).length(); }
