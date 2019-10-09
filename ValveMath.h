#pragma once

#include "MainInclude.hpp"
#include <stdint.h>

namespace ValveMath
{
#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define RADPI 57.295779513082f
#define rad(a) a * 0.01745329251

	void AngleVectorsM(const Vector & angles, Vector * forward);
	void VectorTransformM(const Vector in1, float in2[3][4], Vector& out);
	void SinCosM(float a, float* s, float* c);
	void VectorAnglesM(Vector forward, Vector& angles);
	void AngleVectorsM(const Vector& angles, Vector* forward, Vector* right, Vector* up);
	void NormalizeM(Vector& vIn, Vector& vOut);
	void CalcAngleM(Vector src, Vector dst, Vector& angles);
	bool IsVisibleM(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID);
	void CalcAngleYawOnlyM(Vector src, Vector dst, Vector& angles);
	void NormalizeVectorM(Vector& vec);
	Vector AngleVectorsM(const Vector meme);
	float distance_point_to_lineM(Vector Point, Vector LineOrigin, Vector Dir);
	void MatrixMultiplyM(matrix3x4_t& ps, matrix3x4_t& pc, matrix3x4_t& pout);
	void VectorRotateM(const Vector& in1, const QAngle& in2, Vector& out);
	void VectorRotateM(const float* in1, const matrix3x4_t& in2, float* out);
	void VectorRotateM(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void MatrixAnglesM(const matrix3x4_t& matrix, float* angles);
	void MatrixCopyM(const matrix3x4_t& source, matrix3x4_t& target);
	void AngleMatrixM(const QAngle& angles, const Vector& position, matrix3x4_t& matrix_out);
	void AngleMatrixM(const Vector angles, matrix3x4_t& matrix);
	void VectorRotateM(const Vector& in1, const Vector& in2, Vector& out);
	void AngleMatrixZZ(const Vector angles, matrix3x4_t& matrix);
	void MatrixSetColumnZZ(const Vector& in, int column, matrix3x4_t& out);
	void AngleMatrixZZ(const Vector& angles, const Vector& position, matrix3x4_t& matrix_out);
	void MatrixCopyZZ(const matrix3x4_t& source, matrix3x4_t& target);
	void MatrixMultiplyZZ(matrix3x4_t& in1, const matrix3x4_t& in2);
	void VectorRotateZZ(const float* in1, const matrix3x4_t& in2, float* out);
	void VectorRotateZZ(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void VectorRotate(const Vector& in1, const Vector& in2, Vector& out);

}