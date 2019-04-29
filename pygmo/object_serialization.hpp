/* Copyright 2017-2018 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#ifndef PYGMO_OBJECT_SERIALIZATION_HPP
#define PYGMO_OBJECT_SERIALIZATION_HPP

#include <pygmo/python_includes.hpp>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/python/import.hpp>
#include <boost/python/object.hpp>
#include <vector>

#include <pagmo/s11n.hpp>

#include <pygmo/common_utils.hpp>

namespace boost
{

namespace serialization
{

template <typename Archive>
inline void save(Archive &ar, const boost::python::object &o, unsigned)
{
    using namespace boost::python;
    // This will dump to a bytes object.
    object tmp = import("pygmo").attr("get_serialization_backend")().attr("dumps")(o);
    // This gives a null-terminated char * to the internal
    // content of the bytes object.
    auto ptr = PyBytes_AsString(tmp.ptr());
    if (!ptr) {
        pygmo_throw(PyExc_TypeError, "the serialization backend's dumps() function did not return a bytes object");
    }
    // NOTE: this will be the length of the bytes object *without* the terminator.
    const auto size = len(tmp);
    // NOTE: we store as char here because that's what is returned by the CPython function.
    // From Python it seems like these are unsigned chars, but this should not concern us.
    std::vector<char> v(ptr, ptr + size);
    ar << v;
}

template <typename Archive>
inline void load(Archive &ar, boost::python::object &o, unsigned)
{
    using namespace boost::python;
    // Extract the char vector.
    std::vector<char> v;
    ar >> v;
    auto b = pygmo::make_bytes(v.data(), boost::numeric_cast<Py_ssize_t>(v.size()));
    o = import("pygmo").attr("get_serialization_backend")().attr("loads")(b);
}

template <class Archive>
inline void serialize(Archive &ar, boost::python::object &o, unsigned version)
{
    split_free(ar, o, version);
}

} // namespace serialization

} // namespace boost

#endif
