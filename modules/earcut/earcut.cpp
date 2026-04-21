/**************************************************************************/
/*  earcut.cpp                                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "earcut.h"

using Coord = real_t;
using Point = std::array<Coord, 2>;
using Points = std::vector<Point>;
using N = uint32_t;

Earcut *Earcut::singleton = NULL;

Earcut *Earcut::get_singleton() {
	return singleton;
}

Vector<int> Earcut::execute(const Vector<Vector2> &p_vertices, const Vector<int> &p_holes) {
	if (p_vertices.empty())
		return Vector<int>();

	std::vector<Points> polygon;

	if (p_holes.empty()) {
		Points points;
		for (int i = 0; i < p_vertices.size(); ++i) {
			points.push_back({ p_vertices[i].x, p_vertices[i].y });
		}
		polygon.push_back(points);
	} else {
		// ERR_EXPLAIN("Holes indexed wrong");
		ERR_FAIL_COND_V(p_holes[p_holes.size() - 1] > p_vertices.size(), Vector<int>());
		int start = 0;
		for (int i = 0; i < p_holes.size(); ++i) {
			Points points;
			for (int j = start; j < p_holes[i]; ++j) {
				points.push_back({ p_vertices[j].x, p_vertices[j].y });
			}
			polygon.push_back(points);
			start = p_holes[i];
		}
	}

	// Fill polygon structure with actual data. Any winding order works.
	// The first polyline defines the main polygono.
	// Following polylines define holes.

	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygon);
	Vector<int> result;
	result.resize(indices.size());
	// for (std::vector<N>::iterator i = indices.begin(); i != indices.end(); ++i) {
	// 	result.set(i, *i);
	// }
	for (int i = 0; i < indices.size(); ++i) {
		result.set(i, indices[i]);
	}

	return result;
}

void Earcut::_bind_methods() {
	ClassDB::bind_method(D_METHOD("execute", "vertices", "holes"), &Earcut::execute);
}

Earcut::Earcut() {
	singleton = this;
}
