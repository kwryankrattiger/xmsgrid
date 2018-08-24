//------------------------------------------------------------------------------
/// \file
/// \brief
/// \copyright (C) Copyright Aquaveo 2018. Distributed under the xmsng
///  Software License, Version 1.0. (See accompanying file
///  LICENSE_1_0.txt or copy at http://www.aquaveo.com/xmsng/LICENSE_1_0.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <boost/shared_ptr.hpp>
#include <xmscore/python/misc/PyUtils.h>
#include <xmsgrid/ugrid/XmUGrid.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);

void initXmUGrid(py::module &m) {
    py::class_<xms::XmUGrid, boost::shared_ptr<xms::XmUGrid>> xmUg(m, "XmUGrid");
    xmUg
        .def(py::init([]() {
            return boost::shared_ptr<xms::XmUGrid>(xms::XmUGrid::New());
        }))
        .def(py::init([](py::iterable pts, py::iterable cell_stream) {
            boost::shared_ptr<xms::VecPt3d> points = xms::VecPt3dFromPyIter(pts);
            boost::shared_ptr<xms::VecInt> cellStream = xms::VecIntFromPyIter(cell_stream);
            return boost::shared_ptr<xms::XmUGrid>(xms::XmUGrid::New(*points, *cellStream));
        }))
        // Point Functions
        .def("get_number_of_points", &xms::XmUGrid::GetNumberOfPoints)
        .def("get_points", [](xms::XmUGrid &self) -> py::iterable {
            return xms::PyIterFromVecPt3d(self.GetPoints());
        },"Get the number of points.")
        .def("set_points", [](xms::XmUGrid &self, py::iterable pts) {
            boost::shared_ptr<xms::VecPt3d> points = xms::VecPt3dFromPyIter(pts);
            self.SetPoints(*points);
        },"Set UGrid points.",py::arg("pts"))
        .def("get_point", [](xms::XmUGrid &self, int pt_index) -> py::iterable {
            return xms::PyIterFromPt3d(self.GetPoint(pt_index));
        },"Get the point",py::arg("pt_index"))
        .def("set_point", [](xms::XmUGrid &self, int pt_index, py::iterable pt) -> bool {
            xms::Pt3d point = xms::Pt3dFromPyIter(pt);
            return self.SetPoint(pt_index, point);
        },"Set the point",py::arg("pt_index"),py::arg("pt"))
        .def("get_points_from_point_idxs", [](xms::XmUGrid &self, py::iterable pt_idx) -> py::iterable {
            boost::shared_ptr<xms::VecInt> point_indexs = xms::VecIntFromPyIter(pt_idx);
            xms::VecPt3d points = self.GetPointsFromPointIdxs(*point_indexs);
            return xms::PyIterFromVecPt3d(points);
        },"Convert a vector of point indices into a vector of point 3d",py::arg("pt_idx"))
        .def("get_extents", [](xms::XmUGrid &self) -> py::iterable {
            xms::Pt3d p_min, p_max;
            self.GetExtents(p_min, p_max);
            return py::make_tuple(xms::PyIterFromPt3d(p_min), xms::PyIterFromPt3d(p_max));
        },"Get extents of all points in UGrid")
        .def("get_point_cells", [](xms::XmUGrid &self, int pt_idx) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetPointCells(pt_idx));
        },"Get the cells that are associated with the specified point",py::arg("pt_idx"))
        .def("get_common_cells", [](xms::XmUGrid &self, py::iterable pts) -> py::iterable {
            boost::shared_ptr<xms::VecInt> points = xms::VecIntFromPyIter(pts);
            return xms::PyIterFromVecInt(self.GetCommonCells(*points));
        },"Gets the common cells from a vector of points",py::arg("pts"))
        // Cell Functions
        .def("get_number_of_cells", &xms::XmUGrid::GetNumberOfCells)
        .def("get_points_of_cell", [](xms::XmUGrid &self, int cell_index) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetPointsOfCell(cell_index));
        },"Get the number of cells.")
        .def("get_cell_type", &xms::XmUGrid::GetCellType,"Get the number of cells.")
        .def("get_dimension_count", [](xms::XmUGrid &self) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetDimensionCount());
        },"Count all number of the cells with each dimenion (0, 1, 2, 3)")
        .def("get_cell_dimension", &xms::XmUGrid::GetCellDimension,
            "Get the dimension of the specified cell.",py::arg("cell_idx")
        )
        .def("get_cell_stream", [](xms::XmUGrid &self) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetCellStream());
        },"Get cell stream vector for the entire UGrid.")
        .def("set_cell_stream", [](xms::XmUGrid &self, py::iterable cell_stream) -> bool {
            boost::shared_ptr<xms::VecInt> cellStream = xms::VecIntFromPyIter(cell_stream);
            return self.SetCellStream(*cellStream);
        },"Set the ugrid cells for the entire UGrid using a cell stream.",py::arg("cell_stream"))
        .def("get_single_cell_stream", [](xms::XmUGrid &self, int cell_index) -> py::iterable {
            xms::VecInt cellStream;
            bool ret_val = self.GetSingleCellStream(cell_index, cellStream);
            return py::make_tuple(ret_val, xms::PyIterFromVecInt(cellStream));
        },"Get cell stream vector for a single cell.",py::arg("cell_index"))
        .def("get_cell_neighbors", [](xms::XmUGrid &self, int cell_index) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetCellNeighbors(cell_index));
        },"Get the cells neighboring a cell (cells associated with any of it's points)",py::arg("cell_index"))
        .def("get_plan_view_polygon", [](xms::XmUGrid &self, int cell_index) -> py::iterable {
            xms::VecPt3d polygon;
            bool ret_val = self.GetPlanViewPolygon(cell_index, polygon);
            return py::make_tuple(ret_val, xms::PyIterFromVecPt3d(polygon));
        },"Get a plan view polygon of a specified cell",py::arg("cell_index"))

      // Edges Functions
        .def("get_number_of_cell_edges", &xms::XmUGrid::GetNumberOfCellEdges,
			"Get the number of edges with specified cell", py::arg("cell_index")
		)
        .def("get_cell_edge_from_edge_index", [](xms::XmUGrid &self, int cell_index, int edge_index) -> py::iterable {
            return xms::PyIterFromIntPair(self.GetCellEdgeFromEdgeIndex(cell_index, edge_index));
        },"Get the points of a cell.",py::arg("cell_index"),py::arg("edge_index"))
        .def("get_adjacent_cells", [](xms::XmUGrid &self, int cell_index, int edge_index) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetAdjacentCells(cell_index, edge_index));
        },"Get the index of the adjacent cells (that shares the same cell edge)",py::arg("cell_index"),py::arg("edge_index"))
        .def("get_2d_adjacent_cell", &xms::XmUGrid::Get2dAdjacentCell,
			"Get the index of the adjacent cells (that shares the same cell edge)",py::arg("cell_index"),py::arg("edge_index")
		)
        .def("get_adjacent_cells_from_given_edge", [](xms::XmUGrid &self, int pt_index_1, int pt_index_2) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetAdjacentCellsFromGivenEdge(pt_index_1, pt_index_2));
        },"Get the indices of the adjacent cells (that shares the same cell edge)",py::arg("pt_index_1"),py::arg("pt_index_2"))
        .def("get_adjacent_cells_from_given_edge", [](xms::XmUGrid &self, py::iterable edge) -> py::iterable {
            std::pair<int, int> a_edge = xms::IntPairFromPyIter(edge);
            return xms::PyIterFromVecInt(self.GetAdjacentCellsFromGivenEdge(a_edge));
        },"Get the index of the adjacent cells (that shares the same cell edge)",py::arg("edge"))
        .def("get_edges_from_point", [](xms::XmUGrid &self, int pt_id) -> py::iterable {
            xms::VecInt cell_idxs, edge_idxs;
            bool succeeded = self.GetEdgesFromPoint(pt_id, cell_idxs, edge_idxs);
			return py::make_tuple(succeeded, xms::PyIterFromVecInt(cell_idxs),
				xms::PyIterFromVecInt(edge_idxs));
        },"Get the Edges associated with a point.",py::arg("pt_id"))
        .def("get_edges_of_cell", [](xms::XmUGrid &self, int cell_index) -> py::iterable {
            return xms::PyIterFromVecIntPair(self.GetEdgesOfCell(cell_index));
        },"Get the Edges of a cell.",py::arg("cell_index"))
        // Face Functions
        .def("get_number_of_cell_faces", &xms::XmUGrid::GetNumberOfCellFaces,
			"Get the number of cell faces for given cell.",py::arg("cell_index"))
        .def("get_cell_face", [](xms::XmUGrid &self, int cell_idx, int face_idx) -> py::iterable {
            return xms::PyIterFromVecInt(self.GetCellFace(cell_idx, face_idx));
        },"Get the cell face for given cell and face index.",py::arg("cell_index"),py::arg("face_index"))
        .def("get_faces_of_cell", [](xms::XmUGrid &self, int cell_idx) -> py::iterable {
            return xms::PyIterFromVecInt2d(self.GetFacesOfCell(cell_idx));
        },"Get the faces of a cell.",py::arg("cell_index"))
        .def("get_faces_from_point", [](xms::XmUGrid &self, int point_id) -> py::iterable {
            xms::VecInt cell_idxs, face_idxs;
            bool ret = self.GetFacesFromPoint(point_id, cell_idxs, face_idxs);
            return py::make_tuple(ret,
                                  xms::PyIterFromVecInt(cell_idxs),
                                  xms::PyIterFromVecInt(face_idxs));
        },"Get the cell face associated with a point.",py::arg("point_id"))
        .def("get_cell_face_neighbor", [](xms::XmUGrid &self, int cell_idx, int face_idx) -> int {
            return self.GetCellFaceNeighbor(cell_idx, face_idx);
        },"Get the cell face neighbors for given cell and face index.",py::arg("cell_index"),py::arg("face_index"))
    ;

    // UGrid CellType
    py::enum_<xms::XmUGridCellType>(xmUg, "xmugrid_celltype_enum",
                                    "ugrid_celltype_enum for XmUGrid class")
        .value("XMU_INVALID_CELL_TYPE", xms::XmUGridCellType::XMU_INVALID_CELL_TYPE)
        .value("XMU_EMPTY_CELL", xms::XmUGridCellType::XMU_EMPTY_CELL)
        .value("XMU_VERTEX", xms::XmUGridCellType::XMU_VERTEX)
        .value("XMU_POLY_VERTEX", xms::XmUGridCellType::XMU_POLY_VERTEX)
        .value("XMU_LINE", xms::XmUGridCellType::XMU_LINE)
        .value("XMU_POLY_LINE", xms::XmUGridCellType::XMU_POLY_LINE)
        .value("XMU_TRIANGLE", xms::XmUGridCellType::XMU_TRIANGLE)
        .value("XMU_TRIANGLE_STRIP", xms::XmUGridCellType::XMU_TRIANGLE_STRIP)
        .value("XMU_POLYGON", xms::XmUGridCellType::XMU_POLYGON)
        .value("XMU_PIXEL", xms::XmUGridCellType::XMU_PIXEL)
        .value("XMU_QUAD", xms::XmUGridCellType::XMU_QUAD)
        .value("XMU_TETRA", xms::XmUGridCellType::XMU_TETRA)
        .value("XMU_VOXEL", xms::XmUGridCellType::XMU_VOXEL)
        .value("XMU_HEXAHEDRON", xms::XmUGridCellType::XMU_HEXAHEDRON)
        .value("XMU_WEDGE", xms::XmUGridCellType::XMU_WEDGE)
        .value("XMU_PYRAMID", xms::XmUGridCellType::XMU_PYRAMID)
        .value("XMU_PENTAGONAL_PRISM", xms::XmUGridCellType::XMU_PENTAGONAL_PRISM)
        .value("XMU_HEXAGONAL_PRISM", xms::XmUGridCellType::XMU_HEXAGONAL_PRISM)
        .value("XMU_QUADRATIC_EDGE", xms::XmUGridCellType::XMU_QUADRATIC_EDGE)
        .value("XMU_QUADRATIC_TRIANGLE", xms::XmUGridCellType::XMU_QUADRATIC_TRIANGLE)
        .value("XMU_QUADRATIC_QUAD", xms::XmUGridCellType::XMU_QUADRATIC_QUAD)
        .value("XMU_QUADRATIC_POLYGON", xms::XmUGridCellType::XMU_QUADRATIC_POLYGON)
        .value("XMU_QUADRATIC_TETRA", xms::XmUGridCellType::XMU_QUADRATIC_TETRA)
        .value("XMU_QUADRATIC_HEXAHEDRON", xms::XmUGridCellType::XMU_QUADRATIC_HEXAHEDRON)
        .value("XMU_QUADRATIC_WEDGE", xms::XmUGridCellType::XMU_QUADRATIC_WEDGE)
        .value("XMU_QUADRATIC_PYRAMID", xms::XmUGridCellType::XMU_QUADRATIC_PYRAMID)
        .value("XMU_BIQUADRATIC_QUAD", xms::XmUGridCellType::XMU_BIQUADRATIC_QUAD)
        .value("XMU_TRIQUADRATIC_HEXAHEDRON", xms::XmUGridCellType::XMU_TRIQUADRATIC_HEXAHEDRON)
        .value("XMU_QUADRATIC_LINEAR_QUAD", xms::XmUGridCellType::XMU_QUADRATIC_LINEAR_QUAD)
        .value("XMU_QUADRATIC_LINEAR_WEDGE", xms::XmUGridCellType::XMU_QUADRATIC_LINEAR_WEDGE)
        .value("XMU_BIQUADRATIC_QUADRATIC_WEDGE", xms::XmUGridCellType::XMU_BIQUADRATIC_QUADRATIC_WEDGE)
        .value("XMU_BIQUADRATIC_QUADRATIC_HEXAHEDRON", xms::XmUGridCellType::XMU_BIQUADRATIC_QUADRATIC_HEXAHEDRON)
        .value("XMU_BIQUADRATIC_TRIANGLE", xms::XmUGridCellType::XMU_BIQUADRATIC_TRIANGLE)
        .value("XMU_CUBIC_LINE", xms::XmUGridCellType::XMU_CUBIC_LINE)
        .value("XMU_CONVEX_POINT_SET", xms::XmUGridCellType::XMU_CONVEX_POINT_SET)
        .value("XMU_POLYHEDRON", xms::XmUGridCellType::XMU_POLYHEDRON)
        .value("XMU_PARAMETRIC_CURVE", xms::XmUGridCellType::XMU_PARAMETRIC_CURVE)
        .value("XMU_PARAMETRIC_SURFACE", xms::XmUGridCellType::XMU_PARAMETRIC_SURFACE)
        .value("XMU_PARAMETRIC_TRI_SURFACE", xms::XmUGridCellType::XMU_PARAMETRIC_TRI_SURFACE)
        .value("XMU_PARAMETRIC_QUAD_SURFACE", xms::XmUGridCellType::XMU_PARAMETRIC_QUAD_SURFACE)
        .value("XMU_PARAMETRIC_TETRA_REGION", xms::XmUGridCellType::XMU_PARAMETRIC_TETRA_REGION)
        .value("XMU_PARAMETRIC_HEX_REGION", xms::XmUGridCellType::XMU_PARAMETRIC_HEX_REGION)
        .value("XMU_HIGHER_ORDER_EDGE", xms::XmUGridCellType::XMU_HIGHER_ORDER_EDGE)
        .value("XMU_HIGHER_ORDER_TRIANGLE", xms::XmUGridCellType::XMU_HIGHER_ORDER_TRIANGLE)
        .value("XMU_HIGHER_ORDER_QUAD", xms::XmUGridCellType::XMU_HIGHER_ORDER_QUAD)
        .value("XMU_HIGHER_ORDER_POLYGON", xms::XmUGridCellType::XMU_HIGHER_ORDER_POLYGON)
        .value("XMU_HIGHER_ORDER_TETRAHEDRON", xms::XmUGridCellType::XMU_HIGHER_ORDER_TETRAHEDRON)
        .value("XMU_HIGHER_ORDER_WEDGE", xms::XmUGridCellType::XMU_HIGHER_ORDER_WEDGE)
        .value("XMU_HIGHER_ORDER_PYRAMID", xms::XmUGridCellType::XMU_HIGHER_ORDER_PYRAMID)
        .value("XMU_HIGHER_ORDER_HEXAHEDRON", xms::XmUGridCellType::XMU_HIGHER_ORDER_HEXAHEDRON)
        .value("XMU_NUMBER_OF_CELL_TYPES", xms::XmUGridCellType::XMU_NUMBER_OF_CELL_TYPES)
        .export_values();

    // UGrid FaceOrientation
    py::enum_<xms::XmUGridFaceOrientation>(xmUg, "xmugrid_faceorientation_enum",
                                    "xmugrid_faceorientation_enum for XmUGrid class")
        .value("XMU_ORIENTATION_UNKNOWN", xms::XmUGridFaceOrientation::XMU_ORIENTATION_UNKNOWN)
        .value("XMU_ORIENTATION_SIDE", xms::XmUGridFaceOrientation::XMU_ORIENTATION_SIDE)
        .value("XMU_ORIENTATION_TOP", xms::XmUGridFaceOrientation::XMU_ORIENTATION_TOP)
        .value("XMU_ORIENTATION_BOTTOM", xms::XmUGridFaceOrientation::XMU_ORIENTATION_BOTTOM)
        .export_values();
}