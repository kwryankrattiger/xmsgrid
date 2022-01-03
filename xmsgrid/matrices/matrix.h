#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief routines for manipulating matrices
///
///        The routines in this file come from "Numerical Recipes in C"
///        by Press et al.
///        important: to use these routines properly, the matrices
///        and vectors should be declared as pointers and initialized
///        from [1..n] using the routines defined in this file.
/// \ingroup matrices
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

#include <xmsgrid/xmsgrid_export.h>
// 3. Standard Library Headers

// 4. External Library Headers
#include <xmscore/points/pt.h>

// 5. Shared Headers

// 6. Non-shared Headers

//----- Namespace declaration --------------------------------------------------

namespace xms
{
XMSGRID_EXPORT int mxLUDecomp(double** mat, int n, int* indx, double* d);
XMSGRID_EXPORT int mxLUBcksub(double** mat, int n, const int* indx, double* b);
XMSGRID_EXPORT bool mxiLudcmp3(double mat[3][3], int indx[3], double* d);
XMSGRID_EXPORT void mxiLubksb3(const double mat[3][3], const int indx[3], double b[3]);
XMSGRID_EXPORT bool mxSolveNxN(double** A, double* x, double* b, int n);
XMSGRID_EXPORT bool mxSolveBandedEquations(double** a, double* x, double* b, int numeqs, int halfbandwidth);
XMSGRID_EXPORT bool mxSolve3x3(double A[3][3], double x[3], double b[3]);
XMSGRID_EXPORT int mxInvert4x4(const double matrix[4][4], double inv[4][4]);
XMSGRID_EXPORT void mxPointMult(Pt3d* pt, const double ctm[4][4]);
XMSGRID_EXPORT void mxCopy4x4(double copy[4][4], const double matrix[4][4]);
XMSGRID_EXPORT void mxIdent4x4(double matrix[4][4]);
XMSGRID_EXPORT void mxMult4x4(double product[4][4], const double matrix1[4][4], const double matrix2[4][4]);
XMSGRID_EXPORT void mxRotate4x4(double xrot, double yrot, double zrot, double rMatt[4][4]);
XMSGRID_EXPORT void mxTranslate4x4(const Pt3d& a_translation, double a_mx[4][4]);
} // namespace xms
