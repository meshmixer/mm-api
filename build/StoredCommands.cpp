/********************************************************************
 * (C) Copyright 2016 by Autodesk, Inc. All Rights Reserved. By using
 * this code,  you  are  agreeing  to the terms and conditions of the
 * License  Agreement  included  in  the documentation for this code.
 * AUTODESK  MAKES  NO  WARRANTIES,  EXPRESS  OR  IMPLIED,  AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT.  AUTODESK PROVIDES THE CODE ON AN 'AS-IS' BASIS AND EXPLICITLY
 * DISCLAIMS  ANY  LIABILITY,  INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES  FOR ERRORS, OMISSIONS, AND  OTHER  PROBLEMS IN THE  CODE.
 *
 * Use, duplication,  or disclosure by the U.S. Government is subject
 * to  restrictions  set forth  in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) as well as DFAR 252.227-7013(c)(1)(ii)
 * (Rights  in Technical Data and Computer Software),  as applicable.
 *******************************************************************/
#include "stdafx.h"
#include "StoredCommands.h"

using namespace mm;
using namespace rms;


#ifndef USING_MM_COMMAND_API
#define scAssert(x) lgDevAssert(x)
#else
#define scAssert(x)
#endif



inline static fstring __tofstr(const char * pString)
{
	fstring s;
	s.nSize = (unsigned int)strlen(pString);
	memcpy(s.data, pString, s.nSize);
	s.data[s.nSize+1] = 0;
	return s;
}
inline static std::vector<unsigned char> __toucstr(const std::string & str)
{
	std::vector<unsigned char> result;
	size_t n = str.length();
	for ( unsigned int k = 0; k < n; ++k )
		result.push_back(str[k]);
	result.push_back('\0');
	return result;
}

inline static void mmsc_init_vector( StoredCommands::vector_int & v, const std::vector<int> & vSource )
{
	size_t nCount = vSource.size();
	v.nElements = (unsigned int)nCount;
	for ( unsigned int k = 0; k < nCount; ++k )
		v.data[k] = vSource[k];
}
inline static void mmsc_extract_vector( std::vector<int> & vTarget, const StoredCommands::vector_int & vSource )
{
	vTarget.resize(vSource.nElements);
	for ( unsigned int k = 0; k < vSource.nElements; ++k )
		vTarget[k] = vSource.data[k];
}
inline static void mmsc_extract_vector( std::vector<int> & vTarget, const StoredCommands::vector_float & vSource )
{
	vTarget.resize(vSource.nElements);
	for ( unsigned int k = 0; k < vSource.nElements; ++k )
		vTarget[k] = (int)vSource.data[k];
}
inline static void mmsc_extract_string_from_vector( std::string & str, const StoredCommands::vector_int & vSource )
{
	char buf[4096];
	for ( unsigned int k = 0; k < vSource.nElements; ++k ) {
		scAssert(vSource.data[k] < 255);
		buf[k] = static_cast<char>(vSource.data[k]);
	}
	buf[vSource.nElements] = '\0';
	str = std::string(buf);
}

inline vec3f make_vec3f(float x, float y, float z)
{
	vec3f v = {x,y,z};
	return v;
}
inline void make_mat3f(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22, float mat[9])
{
	mat[0] = m00; mat[1] = m01; mat[2] = m02;
	mat[3] = m10; mat[4] = m11; mat[5] = m12;
	mat[6] = m20; mat[7] = m21; mat[8] = m22;
}
inline void make_mat3f(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22, mat3f & m)
{
	make_mat3f(m00,m01,m02,m10,m11,m12,m20,m21,m22,m.data);
}
inline void unpack_mat3f(float & m00, float & m01, float & m02, float & m10, float & m11, float & m12, float & m20, float & m21, float & m22, const float mat[9])
{
	m00 = mat[0]; m01 = mat[1]; m02 = mat[2];
	m10 = mat[3]; m11 = mat[4]; m12 = mat[5];
	m20 = mat[6]; m21 = mat[7]; m22 = mat[8];
}
inline void unpack_mat3f(float & m00, float & m01, float & m02, float & m10, float & m11, float & m12, float & m20, float & m21, float & m22, const mat3f & m)
{
	unpack_mat3f(m00,m01,m02,m10,m11,m12,m20,m21,m22,m.data);
}


StoredCommands::vector_int & StoredCommands::vector_int::append(int i)
{
	data[nElements++] = i;
	return *this;
}

StoredCommands::vector_float & StoredCommands::vector_float::append(float f)
{
	data[nElements++] = f;
	return *this;
}



inline static void append_vec3f( BinarySerializer & s, const vec3f & v )
{
	s.append(v.x);		s.append(v.y);		s.append(v.z);
}
inline static void restore_vec3f( BinarySerializer & s, vec3f & v )
{
	s.restore(v.x);		s.restore(v.y);		s.restore(v.z);
}
inline static void append_frame3f( BinarySerializer & s, const frame3f & f )
{
	s.append(f.origin_x);		s.append(f.origin_y);		s.append(f.origin_z);
	s.append(f.normal_x);		s.append(f.normal_y);		s.append(f.normal_z);
	s.append(f.tan1_x);			s.append(f.tan1_y);			s.append(f.tan1_z);
	s.append(f.tan2_x);			s.append(f.tan2_y);			s.append(f.tan2_z);
}
inline static void restore_frame3f( BinarySerializer & s, frame3f & f )
{
	s.restore(f.origin_x);		s.restore(f.origin_y);		s.restore(f.origin_z);
	s.restore(f.normal_x);		s.restore(f.normal_y);		s.restore(f.normal_z);
	s.restore(f.tan1_x);		s.restore(f.tan1_y);		s.restore(f.tan1_z);
	s.restore(f.tan2_x);		s.restore(f.tan2_y);		s.restore(f.tan2_z);
}


void StoredCommands::Store(BinarySerializer & s)
{
	int nCount = (int)m_vCommands.size();
	s.append(nCount);

	for ( int k = 0; k < nCount; ++k ) {
		const Command & c = m_vCommands[k];
		s.append( (int)c.eType );

		switch ( c.eType ) {
			case MouseEventCommand:
				s.append( (int)c.c.mouse.eType );
				s.append( c.c.mouse.bLeft ? 1 : 0 );
				s.append( c.c.mouse.bMiddle ? 1 : 0 );
				s.append( c.c.mouse.bRight ? 1 : 0 );
				s.append( c.c.mouse.x );
				s.append( c.c.mouse.y );
				s.append( c.c.mouse.bCoordIsAbsolute ? 1 : 0 );
				break;

			case CameraControlCommand:
				s.append( (int)c.c.camera.eType );
				s.append( c.c.camera.fVal1 );
				s.append( c.c.camera.fVal2 );
				s.append( c.c.camera.bFlag ? 1 : 0 );
				s.append( c.c.camera.nx ); s.append(c.c.camera.ny);
				s.append( c.c.camera.eye.x );  s.append( c.c.camera.eye.y );  s.append( c.c.camera.eye.z );
				s.append( c.c.camera.target.x );  s.append( c.c.camera.target.y );  s.append( c.c.camera.target.z );
				s.append( c.c.camera.up.x );  s.append( c.c.camera.up.y );  s.append( c.c.camera.up.z );
				break;

			case BeginToolCommand:
			case CompleteToolCommand:
				s.append( c.c.toolctrl.name, 32 );
				break;

			case ToolParameterCommand:
				s.append( c.c.toolparam.name, 32 );
				s.append( c.c.toolparam.eType );
				if ( c.c.toolparam.eType == ToolParam_Float || c.c.toolparam.eType == ToolParam_Utility_Float ) {
					s.append( c.c.toolparam.v.f );
				} else if ( c.c.toolparam.eType == ToolParam_Vec3 || c.c.toolparam.eType == ToolParam_Utility_Vec3  ) {
					s.append( c.c.toolparam.v.vec.x );
					s.append( c.c.toolparam.v.vec.y );
					s.append( c.c.toolparam.v.vec.z );
				} else if ( c.c.toolparam.eType == ToolParam_Mat3 || c.c.toolparam.eType == ToolParam_Utility_Mat3 ) {
					s.append( c.c.toolparam.v.mat3.data, 9 );
				} else if ( c.c.toolparam.eType == ToolParam_String || c.c.toolparam.eType == ToolParam_Utility_String ) {
					s.append( c.c.toolparam.v.str.nSize );
					if ( c.c.toolparam.v.str.nSize > 0 )
						s.append( c.c.toolparam.v.str.data, c.c.toolparam.v.str.nSize );
				} else
					s.append( c.c.toolparam.v.i );
				break;

			case SceneCommand:
				s.append( c.c.scene.eType );
				s.append( c.c.scene.str.nSize );
				if ( c.c.scene.str.nSize > 0 )
					s.append( c.c.scene.str.data, c.c.scene.str.nSize );
				s.append( c.c.scene.nObjectIDs.nElements );
				if ( c.c.scene.nObjectIDs.nElements > 0 )
					s.append( c.c.scene.nObjectIDs.data, c.c.scene.nObjectIDs.nElements );
                append_frame3f(s, c.c.scene.f);
				break;

			case SelectCommand:
				s.append( c.c.select.eType );
				s.append( c.c.select.p.x );		s.append( c.c.select.p.y );		s.append( c.c.select.p.z );
				s.append( c.c.select.d.x );		s.append( c.c.select.d.y );		s.append( c.c.select.d.z );
				s.append( c.c.select.r );
				s.append( c.c.select.n );
				s.append( c.c.select.str, 32 );
				s.append( c.c.select.vGroups.nElements );
				if ( c.c.select.vGroups.nElements > 0 )
					s.append( c.c.select.vGroups.data, c.c.select.vGroups.nElements );
				break;

			case BrushCommand:
				s.append( c.c.brush.eType );
				s.append( c.c.brush.vStamps.nElements );
				if ( c.c.brush.vStamps.nElements > 0 )
					s.append( c.c.brush.vStamps.data, c.c.brush.vStamps.nElements );
				break;

			case PartCommand:
				s.append(c.c.part.eType);
				s.append( c.c.part.filename.nSize );
				if ( c.c.part.filename.nSize > 0 )
					s.append( c.c.part.filename.data, c.c.part.filename.nSize );
				append_frame3f(s, c.c.part.f);
				s.append( c.c.part.r );
				s.append( c.c.part.bFlag );
				break;

			case StampCommand:
				s.append( c.c.stamp.eType );
				s.append( c.c.stamp.x );	s.append( c.c.stamp.y );	s.append( c.c.stamp.z );
				s.append( c.c.stamp.r );
				break;

			case SpatialQueryCommand:
				s.append( c.c.spatial.eType );
				s.append( c.c.spatial.p.x );	s.append( c.c.spatial.p.y );	s.append( c.c.spatial.p.z );
				s.append( c.c.spatial.d.x );	s.append( c.c.spatial.d.y );	s.append( c.c.spatial.d.z );
				break;

			case GenericQueryCommand:
				s.append( c.c.generic_query.eType );
				s.append( c.c.generic_query.n );
				s.append( c.c.generic_query.p );
				s.append( c.c.generic_query.str.nSize );
				if ( c.c.generic_query.str.nSize > 0 )
					s.append( c.c.generic_query.str.data, c.c.generic_query.str.nSize );
				break;
                
            case ToolParameterChangeCommand:
                break;
		}
	}
}


void StoredCommands::Restore(unsigned char * pBytes, size_t nSize)
{
	rms::BinarySerializer s;
	s.buffer().resize(nSize);
	memcpy(&s.buffer()[0], pBytes, nSize);
	Restore(s);
}
void StoredCommands::Restore(BinarySerializer & s)
{
	s.beginRestore();

	int nCount;
	s.restore(nCount);

	m_vCommands.resize(nCount);
	for ( int k = 0; k < nCount; ++k ) {
		Command & c = m_vCommands[k];

		int eType, nButton, nAbsolute, nVal;
		s.restore(eType);
		c.eType = (CommandType)eType;

		switch ( c.eType ) {
			case MouseEventCommand:
				s.restore(eType);
				c.c.mouse.eType = (MouseEventType)eType;
				s.restore(nButton);
				c.c.mouse.bLeft = (nButton) ? true : false;
				s.restore(nButton);
				c.c.mouse.bMiddle = (nButton) ? true : false;
				s.restore(nButton);
				c.c.mouse.bRight = (nButton) ? true : false;
				s.restore( c.c.mouse.x );
				s.restore( c.c.mouse.y );
				s.restore(nAbsolute);
				c.c.mouse.bCoordIsAbsolute = (nAbsolute) ? true : false;
				break;

			case CameraControlCommand:
				s.restore(eType);
				c.c.camera.eType = (CameraCmdType)eType;
				s.restore( c.c.camera.fVal1 );
				s.restore( c.c.camera.fVal2 );
				s.restore(nVal);
				c.c.camera.bFlag = (nVal) ? true : false;
				s.restore( c.c.camera.nx ); s.restore(c.c.camera.ny);
				s.restore( c.c.camera.eye.x );  s.restore( c.c.camera.eye.y );  s.restore( c.c.camera.eye.z );
				s.restore( c.c.camera.target.x );  s.restore( c.c.camera.target.y );  s.restore( c.c.camera.target.z );
				s.restore( c.c.camera.up.x );  s.restore( c.c.camera.up.y );  s.restore( c.c.camera.up.z );
				break;

			case BeginToolCommand:
			case CompleteToolCommand:
				s.restore( c.c.toolctrl.name, 32 );
				break;

			case ToolParameterCommand:
				s.restore( c.c.toolparam.name, 32 );
				s.restore( c.c.toolparam.eType );
				if ( c.c.toolparam.eType == ToolParam_Float || c.c.toolparam.eType == ToolParam_Utility_Float ) {
					s.restore( c.c.toolparam.v.f );
				} else if ( c.c.toolparam.eType == ToolParam_Vec3 || c.c.toolparam.eType == ToolParam_Utility_Vec3 ) {
					s.restore( c.c.toolparam.v.vec.x );
					s.restore( c.c.toolparam.v.vec.y );
					s.restore( c.c.toolparam.v.vec.z );
				} else if ( c.c.toolparam.eType == ToolParam_Mat3 || c.c.toolparam.eType == ToolParam_Utility_Mat3 ) {
					s.restore( c.c.toolparam.v.mat3.data, 9 );
				} else if ( c.c.toolparam.eType == ToolParam_String || c.c.toolparam.eType == ToolParam_Utility_String ) {
					s.restore( c.c.toolparam.v.str.nSize );
					if ( c.c.toolparam.v.str.nSize > 0 )
						s.restore( c.c.toolparam.v.str.data, c.c.toolparam.v.str.nSize );
				} else
					s.restore( c.c.toolparam.v.i );
				break;

			case SceneCommand:
				s.restore( c.c.scene.eType );
				s.restore( c.c.scene.str.nSize );
				if ( c.c.scene.str.nSize > 0 )
					s.restore( c.c.scene.str.data, c.c.scene.str.nSize );
				s.restore( c.c.scene.nObjectIDs.nElements );
				if ( c.c.scene.nObjectIDs.nElements > 0 )
					s.restore( c.c.scene.nObjectIDs.data, c.c.scene.nObjectIDs.nElements );
                restore_frame3f(s, c.c.scene.f);
				break;

			case SelectCommand:
				s.restore( c.c.select.eType );
				s.restore( c.c.select.p.x );	s.restore( c.c.select.p.y );	s.restore( c.c.select.p.z );
				s.restore( c.c.select.d.x );	s.restore( c.c.select.d.y );	s.restore( c.c.select.d.z );
				s.restore( c.c.select.r );
				s.restore( c.c.select.n );
				s.restore( c.c.select.str, 32 );
				s.restore( c.c.select.vGroups.nElements );
				if ( c.c.select.vGroups.nElements > 0 )
					s.restore( c.c.select.vGroups.data, c.c.select.vGroups.nElements );
				break;


			case BrushCommand:
				s.restore( c.c.brush.eType );
				s.restore( c.c.brush.vStamps.nElements );
				if ( c.c.brush.vStamps.nElements > 0 )
					s.restore( c.c.brush.vStamps.data, c.c.brush.vStamps.nElements );
				break;

			case PartCommand:
				s.restore(c.c.part.eType);
				s.restore( c.c.part.filename.nSize );
				if ( c.c.part.filename.nSize > 0 )
					s.restore( c.c.part.filename.data, c.c.part.filename.nSize );
				restore_frame3f(s, c.c.part.f);
				s.restore( c.c.part.r );
				s.restore( c.c.part.bFlag );
				break;

			case StampCommand:
				s.restore( c.c.stamp.eType );
				s.restore( c.c.stamp.x );	s.restore( c.c.stamp.y );	s.restore( c.c.stamp.z );
				s.restore( c.c.stamp.r );
				break;


			case SpatialQueryCommand:
				s.restore( c.c.spatial.eType );
				s.restore( c.c.spatial.p.x );	s.restore( c.c.spatial.p.y );	s.restore( c.c.spatial.p.z );
				s.restore( c.c.spatial.d.x );	s.restore( c.c.spatial.d.y );	s.restore( c.c.spatial.d.z );
				break;

			case GenericQueryCommand:
				s.restore( c.c.generic_query.eType );
				s.restore( c.c.generic_query.n );
				s.restore( c.c.generic_query.p );
				s.restore( c.c.generic_query.str.nSize );
				if ( c.c.generic_query.str.nSize > 0 )
					s.restore( c.c.generic_query.str.data, c.c.generic_query.str.nSize );
				break;
                
            case ToolParameterChangeCommand:
                break;

		}

	}

}








void StoredCommands::Store_Results(rms::BinarySerializer & s)
{
	int nCount = (int)m_vCommands.size();
	s.append(nCount);

	for ( int k = 0; k < nCount; ++k ) {
		const Command & c = m_vCommands[k];
		s.append( (int)c.eType );

		switch ( c.eType ) {
			case MouseEventCommand:
				break;

			case CameraControlCommand:
				s.append( c.c.camera.eType );
				//s.append( c.r.camera.eType );
				append_frame3f(s, c.r.camera.f );
				append_vec3f(s, c.r.camera.target);
				s.append(c.r.camera.fov);
				s.append(c.r.camera.nx);   s.append(c.r.camera.ny);
				break;

			case BeginToolCommand:
			case CompleteToolCommand:
				break;

			case ToolParameterCommand:
				s.append( c.r.toolparam.OK );
				s.append( c.r.toolparam.name, 32 );
				s.append( c.r.toolparam.eType );
				if ( c.r.toolparam.eType == ToolParam_Get_Float ) {
					s.append( c.r.toolparam.v.f );
				} else if ( c.r.toolparam.eType == ToolParam_Get_Vec3  ) {
					s.append( c.r.toolparam.v.vec.x );
					s.append( c.r.toolparam.v.vec.y );
					s.append( c.r.toolparam.v.vec.z );
				} else if ( c.r.toolparam.eType == ToolParam_Get_Mat3 ) {
					s.append( c.r.toolparam.v.mat3.data, 9 );
				} else
					s.append( c.r.toolparam.v.i );
				break;

			case SceneCommand:
				s.append( c.c.scene.eType );
				s.append( c.r.scene.OK );
				s.append( c.r.scene.str.nSize );
				if ( c.r.scene.str.nSize > 0 )
					s.append( c.r.scene.str.data, c.r.scene.str.nSize );
				s.append( c.r.scene.nObjectIDs.nElements );
				if ( c.r.scene.nObjectIDs.nElements > 0 )
					s.append( c.r.scene.nObjectIDs.data, c.r.scene.nObjectIDs.nElements );
				append_frame3f(s, c.r.scene.f);
				break;

			case SelectCommand:
				s.append( c.c.select.eType );
				s.append( c.r.select.OK );
				s.append( c.r.select.vGroups.nElements );
				if ( c.r.select.vGroups.nElements > 0 )
					s.append( c.r.select.vGroups.data, c.r.select.vGroups.nElements );
				break;

			case PartCommand:
				s.append( c.c.part.eType );
				s.append( c.r.part.OK );
				s.append( c.r.part.vNewGroups.nElements );
				if ( c.r.part.vNewGroups.nElements > 0)
					s.append( c.r.part.vNewGroups.data, c.r.part.vNewGroups.nElements );
				break;

			case StampCommand:
				s.append( c.c.stamp.eType );
				s.append( c.r.stamp.OK );
				s.append( c.r.stamp.nNewGroupID );
				break;

			case SpatialQueryCommand:
				s.append( c.c.spatial.eType );
				s.append( c.r.spatial.OK );
				s.append ( c.r.spatial.v.nElements );
				if ( c.r.spatial.v.nElements > 0 )
					s.append( c.r.spatial.v.data, c.r.spatial.v.nElements );
				break;

			case GenericQueryCommand:
				s.append( c.c.generic_query.eType );
				s.append( c.r.generic_query.OK );
				s.append( c.r.generic_query.p );
				s.append ( c.r.generic_query.vList.nElements );
				if ( c.r.generic_query.vList.nElements > 0 )
					s.append( c.r.generic_query.vList.data, c.r.generic_query.vList.nElements );
				break;
                
            case ToolParameterChangeCommand:
            case BrushCommand:
                break;
		}
	}

}




void StoredCommands::Restore_Results(unsigned char * pBytes, size_t nSize)
{
	rms::BinarySerializer s;
	s.buffer().resize(nSize);
	memcpy(&s.buffer()[0], pBytes, nSize);
	Restore_Results(s);
}
void StoredCommands::Restore_Results(BinarySerializer & s)
{
	s.beginRestore();

	int nCount;
	s.restore(nCount);

	m_vCommands.resize(nCount);
	for ( int k = 0; k < nCount; ++k ) {
		Command & c = m_vCommands[k];

		int eType;
		s.restore(eType);
		c.eType = (CommandType)eType;

		switch ( c.eType ) {
			case MouseEventCommand:
				break;

			case CameraControlCommand:
				s.restore( c.c.camera.eType );
				restore_frame3f(s, c.r.camera.f );
				restore_vec3f(s, c.r.camera.target);
				s.restore(c.r.camera.fov);
				s.restore(c.r.camera.nx);   s.restore(c.r.camera.ny);
				break;

			case BeginToolCommand:
			case CompleteToolCommand:
				break;

			case ToolParameterCommand:
				s.restore( c.r.toolparam.OK );
				s.restore( c.r.toolparam.name, 32 );
				s.restore( c.r.toolparam.eType );
				if ( c.r.toolparam.eType == ToolParam_Get_Float ) {
					s.restore( c.r.toolparam.v.f );
				} else if ( c.r.toolparam.eType == ToolParam_Get_Vec3  ) {
					s.restore( c.r.toolparam.v.vec.x );
					s.restore( c.r.toolparam.v.vec.y );
					s.restore( c.r.toolparam.v.vec.z );
				} else if ( c.r.toolparam.eType == ToolParam_Get_Mat3 ) {
					s.restore( c.r.toolparam.v.mat3.data, 9 );
				} else
					s.restore( c.r.toolparam.v.i );
				break;

			case SceneCommand:
				s.restore( c.c.scene.eType );
				s.restore( c.r.scene.OK );
				s.restore( c.r.scene.str.nSize );
				if ( c.r.scene.str.nSize > 0 )
					s.restore( c.r.scene.str.data, c.r.scene.str.nSize );
				s.restore( c.r.scene.nObjectIDs.nElements );
				if ( c.r.scene.nObjectIDs.nElements > 0 )
					s.restore( c.r.scene.nObjectIDs.data, c.r.scene.nObjectIDs.nElements );
				restore_frame3f(s, c.r.scene.f);
				break;

			case SelectCommand:
				s.restore( c.c.select.eType );
				s.restore( c.r.select.OK );
				s.restore( c.r.select.vGroups.nElements );
				if ( c.r.select.vGroups.nElements > 0 )
					s.restore( c.r.select.vGroups.data, c.r.select.vGroups.nElements );
				break;

			case PartCommand:
				s.restore( c.c.part.eType );
				s.restore( c.r.part.OK );
				s.restore( c.r.part.vNewGroups.nElements );
				if ( c.r.part.vNewGroups.nElements > 0)
					s.restore( c.r.part.vNewGroups.data, c.r.part.vNewGroups.nElements );
				break;

			case StampCommand:
				s.restore( c.c.stamp.eType );
				s.restore( c.r.stamp.OK );
				s.restore( c.r.stamp.nNewGroupID );
				break;

			case SpatialQueryCommand:
				s.restore( c.c.spatial.eType );
				s.restore( c.r.spatial.OK );
				s.restore ( c.r.spatial.v.nElements );
				if ( c.r.spatial.v.nElements > 0 )
					s.restore( c.r.spatial.v.data, c.r.spatial.v.nElements );
				break;

			case GenericQueryCommand:
				s.restore( c.c.generic_query.eType );
				s.restore( c.r.generic_query.OK );
				s.restore( c.r.generic_query.p );
				s.restore ( c.r.generic_query.vList.nElements );
				if ( c.r.generic_query.vList.nElements > 0 )
					s.restore( c.r.generic_query.vList.data, c.r.generic_query.vList.nElements );
				break;

            case ToolParameterChangeCommand:
            case BrushCommand:
                break;
		}

	}

}




void StoredCommands::Store_Internal()
{
	m_internalStore = rms::BinarySerializer();
	this->Store(m_internalStore);
}
size_t StoredCommands::Store_GetSize()
{
	return m_internalStore.buffer().size();
}
void StoredCommands::Store_GetBuffer(unsigned char * pBytes, size_t nSize )
{
	memcpy(pBytes, &m_internalStore.buffer()[0], nSize);
}




void StoredCommands::AppendMouseDownEvent( bool bLeftDown, bool bMiddleDown, bool bRightDown, float x, float y, bool bAbsolute )
{
	Command c;  c.init();
	c.eType = MouseEventCommand;
	c.c.mouse.init();
	c.c.mouse.eType = MouseDown;
	c.c.mouse.bLeft = bLeftDown;
	c.c.mouse.bMiddle = bMiddleDown;
	c.c.mouse.bRight = bRightDown;
	c.c.mouse.x = x;
	c.c.mouse.y = y;
	c.c.mouse.bCoordIsAbsolute = bAbsolute;
	append_command(c);
}

void StoredCommands::AppendMouseMoveEvent( float x, float y, bool bAbsolute )
{
	Command c;  c.init();
	c.eType = MouseEventCommand;
	c.c.mouse.init();
	c.c.mouse.eType = MouseMove;
	c.c.mouse.x = x;
	c.c.mouse.y = y;
	c.c.mouse.bCoordIsAbsolute = bAbsolute;
	append_command(c);
}

void StoredCommands::AppendMouseUpEvent( bool bLeftUp, bool bMiddleUp, bool bRightUp, float x, float y, bool bAbsolute )
{
	Command c;  c.init();
	c.eType = MouseEventCommand;
	c.c.mouse.init();
	c.c.mouse.eType = MouseUp;
	c.c.mouse.bLeft = bLeftUp;
	c.c.mouse.bMiddle = bMiddleUp;
	c.c.mouse.bRight = bRightUp;
	c.c.mouse.x = x;
	c.c.mouse.y = y;
	c.c.mouse.bCoordIsAbsolute = bAbsolute;
	append_command(c);
}








/*
 * Camera manipulation
 */

#define MMAPI_INIT_CAM_COMMAND(c, cmdtype) c.init(); c.eType = CameraControlCommand; c.c.camera.eType = cmdtype; 

void StoredCommands::CameraControl_Begin()
{
	Command c;  
	MMAPI_INIT_CAM_COMMAND(c, CamManip);
	c.c.camera.bFlag = true;
	append_command(c);
}
void StoredCommands::CameraControl_End()
{
	Command c;  
	MMAPI_INIT_CAM_COMMAND(c, CamManip);
	c.c.camera.bFlag = false;
	append_command(c);
}
void StoredCommands::CameraControl_EnableOrbitSnap()
{
	Command c;  
	MMAPI_INIT_CAM_COMMAND(c, CamToggleSnap);
	c.c.camera.bFlag = true;
	append_command(c);
}
void StoredCommands::CameraControl_DisableOrbitSnap()
{
	Command c;  
	MMAPI_INIT_CAM_COMMAND(c, CamToggleSnap);
	c.c.camera.bFlag = false;
	append_command(c);
}
void StoredCommands::CameraControl_FreeOrbit(float fAngleDegreesX, float fAngleDegreesY)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamOrbit;
	c.c.camera.fVal1 = fAngleDegreesX;  c.c.camera.fVal2 = fAngleDegreesY;
	append_command(c);
}
void StoredCommands::CameraControl_TurntableOrbit(float fAngleDegreesX, float fAngleDegreesY)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamTurntable;
	c.c.camera.fVal1 = fAngleDegreesX;  c.c.camera.fVal2 = fAngleDegreesY;
	append_command(c);
}
void StoredCommands::CameraControl_Pan(float fDistanceX, float fDistanceY)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamPan;
	c.c.camera.fVal1 = fDistanceX;  c.c.camera.fVal2 = fDistanceY;
	append_command(c);
}
void StoredCommands::CameraControl_DollyZoom(float fDistanceZ)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamDolly;
	c.c.camera.fVal1 = fDistanceZ;
	append_command(c);
}
void StoredCommands::CameraControl_RecenterView()
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamRecenter;
	c.c.camera.bFlag = false;
	append_command(c);
}
void StoredCommands::CameraControl_RecenterViewAtCursor()
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamRecenter;
	c.c.camera.bFlag = true;
	append_command(c);
}
void StoredCommands::CameraControl_SetSpecificView(const vec3f & eye, const vec3f & target, const vec3f & up)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamSet;
	c.c.camera.eye = eye;
	c.c.camera.target = target;
	c.c.camera.up = up;
	append_command(c);
}

void StoredCommands::CameraControl_SetOrthographicView()
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamOrthographic;
	c.c.camera.bFlag = false;
	append_command(c);
}
void StoredCommands::CameraControl_SetPerspectiveView()
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamPerspective;
	c.c.camera.bFlag = false;
	append_command(c);
}

// nMode  0=SmoothNormals, 1=FaceNormals, 2=GroupNormals
void StoredCommands::ViewControl_SetSurfaceNormalMode(int nMode)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = SetViewNormalMode;
	c.c.camera.nx = nMode;
	append_command(c);
}

// nMode  0=VertexColors, 1=GroupColors
void StoredCommands::ViewControl_SetTriangleColorMode(int nMode)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = SetViewColorMode;
	c.c.camera.nx = nMode;
	append_command(c);
}


void StoredCommands::ViewControl_SetShowWireframe(bool bShow)
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShowWireframe);
	c.c.camera.bFlag = bShow;
	append_command(c);
}
void StoredCommands::ViewControl_SetShowBoundaries(bool bShow)
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShowBoundaries);
	c.c.camera.bFlag = bShow;
	append_command(c);
}
void StoredCommands::ViewControl_SetShowGrid(bool bShow)
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShowGrid);
	c.c.camera.bFlag = bShow;
	append_command(c);
}
void StoredCommands::ViewControl_SetShowPrinterBed(bool bShow)
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShowPrinterBed);
	c.c.camera.bFlag = bShow;
	append_command(c);
}
void StoredCommands::ViewControl_SetTransparentTarget(bool bEnable)
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetTransparentTarget);
	c.c.camera.bFlag = bEnable;
	append_command(c);
}


void StoredCommands::ViewControl_SetDefaultShader()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShader_Default);
	append_command(c);
}
void StoredCommands::ViewControl_SetXRayShader()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShader_XRay);
	append_command(c);
}
void StoredCommands::ViewControl_SetTextureShader()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShader_Texture);
	append_command(c);
}
void StoredCommands::ViewControl_SetUVShader()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShader_UV);
	append_command(c);
}
void StoredCommands::ViewControl_SetOverhangShader()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, SetShader_Overhang);
	append_command(c);
}

void StoredCommands::ViewControl_ShowObjectBrowser()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, ShowObjectBrowser);
	append_command(c);
}
void StoredCommands::ViewControl_HideObjectBrowser()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, HideObjectBrowser);
	append_command(c);
}
void StoredCommands::ViewControl_TakeFocus()
{
	Command c;  	MMAPI_INIT_CAM_COMMAND(c, TakeFocus);
	append_command(c);
}







StoredCommands::Key StoredCommands::CameraControl_QueryCamera()
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamQuery;
	return append_command(c);
}
bool StoredCommands::CameraControl_QueryCameraResult(Key k, frame3f & f, vec3f & target, camera_info & cam_info )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	f = c.r.camera.f;
	target = c.r.camera.target;
	cam_info.horzFOVDegrees = c.r.camera.fov;
	cam_info.width = c.r.camera.nx;
	cam_info.height = c.r.camera.ny;
	return true;
}

StoredCommands::Key StoredCommands::CameraControl_QueryEyeRayAtPixel(int x, int y)
{
	Command c;  c.init();	c.eType = CameraControlCommand;
	c.c.camera.eType = CamGetRay;
	c.c.camera.nx = x;
	c.c.camera.ny = y;
	return append_command(c);
}
bool StoredCommands::CameraControl_QueryEyeRayAtPixelResult(Key k, vec3f & ray_origin, vec3f & ray_direction)
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	vec3f o = { c.r.camera.f.origin_x, c.r.camera.f.origin_y, c.r.camera.f.origin_z };
	vec3f d = { c.r.camera.f.normal_x, c.r.camera.f.normal_y, c.r.camera.f.normal_z };
	ray_origin = o;
	ray_direction = d;
	return true;
}



void StoredCommands::AppendBeginToolCommand( std::string toolName )
{
	Command c;  c.init();
	c.eType = BeginToolCommand;
	sprintf_s(c.c.toolctrl.name, sizeof(c.c.toolctrl.name), "%s", toolName.c_str());
	append_command(c);
}



void StoredCommands::AppendCompleteToolCommand( std::string command )
{
	Command c;  c.init();
	c.eType = CompleteToolCommand;
	sprintf_s(c.c.toolctrl.name, sizeof(c.c.toolctrl.name), "%s", command.c_str());
	append_command(c);
}





void StoredCommands::AppendToolParameterCommand( std::string paramName, float fValue )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", paramName.c_str());
	c.c.toolparam.eType = ToolParam_Float;
	c.c.toolparam.v.f = fValue;
	append_command(c);
}

void StoredCommands::AppendToolParameterCommand( std::string paramName, int nValue )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", paramName.c_str());
	c.c.toolparam.eType = ToolParam_Int;
	c.c.toolparam.v.i = nValue;
	append_command(c);
}

void StoredCommands::AppendToolParameterCommand( std::string paramName, bool bValue )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", paramName.c_str());
	c.c.toolparam.eType = ToolParam_Bool;
	c.c.toolparam.v.i = (bValue) ? 1 : 0;
	append_command(c);
}
void StoredCommands::AppendToolParameterCommand( std::string paramName, float x, float y, float z )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", paramName.c_str());
	c.c.toolparam.eType = ToolParam_Vec3;
	c.c.toolparam.v.vec = make_vec3f(x,y,z);
	append_command(c);
}
void StoredCommands::AppendToolParameterCommand( std::string paramName, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22 )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", paramName.c_str());
	c.c.toolparam.eType = ToolParam_Mat3;
	make_mat3f(m00,m01,m02,m10,m11,m12,m20,m21,m22, c.c.toolparam.v.mat3);
	append_command(c);
}





StoredCommands::Key StoredCommands::AppendGetToolParameterCommand( std::string paramName )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", paramName.c_str());
	c.c.toolparam.eType = ToolParam_Get;
	return append_command(c);
}

bool StoredCommands::GetToolParameterCommandResult( Key k, any_result & value )
{
	// 0=float, 1=int, 2=bool, 3=vec3, 4=m
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.toolparam.OK == 0 )
		return false;
	if ( c.r.toolparam.eType == ToolParam_Get_Float ) {
		value.f = c.r.toolparam.v.f; 
		value.type = 0; 
		return true;
	} else if ( c.r.toolparam.eType == ToolParam_Get_Int ) {
		value.i = c.r.toolparam.v.i; 
		value.type = 1; 
		return true;
	} else if ( c.r.toolparam.eType == ToolParam_Get_Bool ) {
		value.b = (c.r.toolparam.v.i == 0) ? false : true; 
		value.type = 2; 
		return true;
	} else if ( c.r.toolparam.eType == ToolParam_Get_Vec3 ) {
		value.x = c.r.toolparam.v.vec.x; value.y = c.r.toolparam.v.vec.y; value.z = c.r.toolparam.v.vec.z; 
		value.type = 3; 
		return true;
	} else if ( c.r.toolparam.eType == ToolParam_Get_Mat3 ) {
		unpack_mat3f(value.m[0], value.m[1], value.m[2], value.m[3], value.m[4], value.m[5], value.m[6], value.m[7], value.m[8], c.r.toolparam.v.mat3);
		value.type = 4; 
		return true;
	} else 
		return false;
}

bool StoredCommands::GetToolParameterCommandResult( Key k, float & fValue )
{
	any_result r;
	if ( GetToolParameterCommandResult(k,r) && r.type == 0 ) {
		fValue = r.f;
		return true;
	} 
	return false;
}
bool StoredCommands::GetToolParameterCommandResult( Key k, int & nValue )
{
	any_result r;
	if ( GetToolParameterCommandResult(k,r) && r.type == 1 ) {
		nValue = r.i;
		return true;
	} 
	return false;
}
bool StoredCommands::GetToolParameterCommandResult( Key k, bool & bValue )
{
	any_result r;
	if ( GetToolParameterCommandResult(k,r) && r.type == 2 ) {
		bValue = r.b;
		return true;
	} 
	return false;
}
bool StoredCommands::GetToolParameterCommandResult( Key k, float & x, float & y, float & z )
{
	any_result r;
	if ( GetToolParameterCommandResult(k,r) && r.type == 3 ) {
		x = r.x; y = r.y; z = r.z;
		return true;
	} 
	return false;
}
bool StoredCommands::GetToolParameterCommandResult( Key k, float & m00, float & m01, float & m02, float & m10, float & m11, float & m12, float & m20, float & m21, float & m22 )
{
	any_result r;
	if ( GetToolParameterCommandResult(k,r) && r.type == 4 ) {
		unpack_mat3f(m00, m01, m02, m10, m11, m12, m20, m21, m22, r.m);
		return true;
	} 
	return false;
}



StoredCommands::Key StoredCommands::AppendToolQuery_NewGroups()
{
	Command c;  c.init();
	c.eType = GenericQueryCommand;
	c.c.generic_query.eType = ToolManager_NewGroups;
	return append_command(c);
}
bool StoredCommands::GetToolQueryResult_NewGroups(StoredCommands::Key k, std::vector<int> & vGroups)
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	mmsc_extract_vector(vGroups, c.r.generic_query.vList);
	return true;
}


StoredCommands::Key StoredCommands::AppendToolQuery_NewObjects()
{
	Command c;  c.init();
	c.eType = GenericQueryCommand;
	c.c.generic_query.eType = ToolManager_NewObjects;
	return append_command(c);
}
bool StoredCommands::GetToolQueryResult_NewObjects(StoredCommands::Key k, std::vector<int> & vObjects)
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	mmsc_extract_vector(vObjects, c.r.generic_query.vList);
	return true;
}



StoredCommands::Key StoredCommands::AppendQueryCommand_ConvertScalarToWorld(float f)
{
	Command c; c.init(); c.eType = GenericQueryCommand;
	c.c.generic_query.eType = ToolManager_ScalarToWorld;
	c.c.generic_query.p.x = f;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_ConvertScalarToWorld(Key k, float & fResult)
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	fResult = c.r.generic_query.p.x;
	return ( c.r.generic_query.OK != 0 );
}


StoredCommands::Key StoredCommands::AppendQueryCommand_ConvertScalarToScene(float f)
{
	Command c; c.init(); c.eType = GenericQueryCommand;
	c.c.generic_query.eType = ToolManager_ScalarToScene;
	c.c.generic_query.p.x = f;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_ConvertScalarToScene(Key k, float & fResult)
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	fResult = c.r.generic_query.p.x;
	return ( c.r.generic_query.OK != 0 );
}



StoredCommands::Key StoredCommands::AppendQueryCommand_ConvertPointToWorld(float fPoint[3])
{
	Command c; c.init(); c.eType = GenericQueryCommand;
	c.c.generic_query.eType = ToolManager_PointToWorld;
	c.c.generic_query.p = make_vec3f(fPoint[0],fPoint[1],fPoint[2]);
	return append_command(c);
}
bool StoredCommands::GetQueryResult_ConvertPointToWorld(Key k, float fPoint[3])
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	fPoint[0] = c.r.generic_query.p.x; fPoint[1] = c.r.generic_query.p.y; fPoint[2] = c.r.generic_query.p.z; 
	return ( c.r.generic_query.OK != 0 );
}

StoredCommands::Key StoredCommands::AppendQueryCommand_ConvertPointToScene(float fPoint[3])
{
	Command c; c.init(); c.eType = GenericQueryCommand;
	c.c.generic_query.eType = ToolManager_PointToScene;
	c.c.generic_query.p = make_vec3f(fPoint[0],fPoint[1],fPoint[2]);
	return append_command(c);
}
bool StoredCommands::GetQueryResult_ConvertPointToScene(Key k, float fPoint[3])
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	fPoint[0] = c.r.generic_query.p.x; fPoint[1] = c.r.generic_query.p.y; fPoint[2] = c.r.generic_query.p.z; 
	return ( c.r.generic_query.OK != 0 );
}




void StoredCommands::AppendToolUtilityCommand( std::string commandName )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", commandName.c_str());
	c.c.toolparam.eType = ToolParam_Utility;
	append_command(c);
}
void StoredCommands::AppendToolUtilityCommand( std::string commandName, int nValue )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", commandName.c_str());
	c.c.toolparam.eType = ToolParam_Utility_Int;
	c.c.toolparam.v.i = nValue;
	append_command(c);
}
void StoredCommands::AppendToolUtilityCommand( std::string commandName, std::string sValue )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", commandName.c_str());
	c.c.toolparam.eType = ToolParam_Utility_String;
	c.c.toolparam.v.str = __tofstr(sValue.c_str());
	append_command(c);
}
void StoredCommands::AppendToolUtilityCommand( std::string commandName, const vec3f & p )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", commandName.c_str());
	c.c.toolparam.eType = ToolParam_Utility_Vec3;
	c.c.toolparam.v.vec = p;
	append_command(c);
}
void StoredCommands::AppendToolUtilityCommand( std::string commandName, const vec3f & v0, const vec3f & v1, float r0, float r1 )
{
	Command c;  c.init();
	c.eType = ToolParameterCommand;
	sprintf_s(c.c.toolparam.name, sizeof(c.c.toolparam.name), "%s", commandName.c_str());
	c.c.toolparam.eType = ToolParam_Utility_Mat3;
	make_mat3f(v0.x,v0.y,v0.z, v1.x,v1.y,v1.z, r0,r1,0.0f, c.c.toolparam.v.mat3);
	append_command(c);
}






#define MMAPI_INIT_SCENE_COMMAND(c, cmdtype) c.init(); c.eType = SceneCommand; c.c.scene.eType = cmdtype; 

#define MMAPI_SCENE_COMMAND(cmdtype) Command c; c.init(); c.eType = SceneCommand; c.c.scene.eType = cmdtype; append_command(c);
#define MMAPI_SCENE_COMMAND_STR(cmdtype, pString) Command c; c.init(); c.eType = SceneCommand; c.c.scene.eType = cmdtype; c.c.scene.str = __tofstr(pString); return append_command(c);



bool StoredCommands::GetSceneCommandResult_IsOK( Key k )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	return ( c.r.scene.OK == 0 ) ? false : true;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_SaveScreenShot(const char * pFilename)
{
	MMAPI_SCENE_COMMAND_STR(SaveScreenShot, pFilename);
}

StoredCommands::Key StoredCommands::AppendSceneCommand_OpenMixFile( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(OpenMixFile, pFilename);
}

StoredCommands::Key StoredCommands::AppendSceneCommand_ExportMixFile( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(ExportMixFile, pFilename);
}

void StoredCommands::AppendSceneCommand_Clear()
{
	MMAPI_SCENE_COMMAND(ClearScene);
}

StoredCommands::Key StoredCommands::AppendSceneCommand_AppendMeshFile( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(AppendMeshFile, pFilename);
}
bool StoredCommands::GetSceneCommandResult_AppendMeshFile( StoredCommands::Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK == 0 )
		return false;
	mmsc_extract_vector(vObjects, c.r.scene.nObjectIDs);
	return true;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_AppendPackedMeshFile( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(AppendPackedMeshFile, pFilename);
}
bool StoredCommands::GetSceneCommandResult_AppendPackedMeshFile( Key k, int & nObjectID )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK == 0 )
		return false;
	nObjectID = c.r.scene.nObjectIDs.data[0];
	return true;
}
bool StoredCommands::GetSceneCommandResult_AppendPackedMeshFile( Key k, any_result & nObjectID )
{
	bool bOK = GetSceneCommandResult_AppendPackedMeshFile(k, nObjectID.i);
	return bOK;
}



StoredCommands::Key StoredCommands::AppendSceneCommand_AppendMeshFileAsReference( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(AppendMeshFileAsReference, pFilename);
}
bool StoredCommands::GetSceneCommandResult_AppendMeshFileAsReference( StoredCommands::Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK == 0 )
		return false;
	mmsc_extract_vector(vObjects, c.r.scene.nObjectIDs);
	return true;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_ExportMeshFile_CurrentSelection( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(ExportMeshFile_SelectedObjects, pFilename);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_ExportAsPackedMeshFile( const char * pFilename, int nObjectID )
{
	Command c; MMAPI_INIT_SCENE_COMMAND(c, ExportAsPackedMeshByID);
	c.c.scene.str = __tofstr(pFilename);
	c.c.scene.nObjectIDs.append(nObjectID);
	return append_command(c);
}



StoredCommands::Key StoredCommands::AppendSceneCommand_CreatePivot( frame3f f )
{
    Command c;  MMAPI_INIT_SCENE_COMMAND(c, CreatePivot);
    c.c.scene.f = f;
    return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_CreatePivot( Key k, int & nObjectID )
{
    if ( k >= m_vCommands.size() )
        return false;
    Command & c = m_vCommands[k];
    if ( c.r.scene.OK == 0 )
        return false;
    nObjectID = c.r.scene.nObjectIDs.data[0];
    return true;
}
bool StoredCommands::GetSceneCommandResult_CreatePivot( Key k, any_result & nObjectID )
{
	int nid;
	if ( GetSceneCommandResult_CreatePivot(k, nid) ) {
		nObjectID.type = 1;
		nObjectID.i = nid;
		return true;
	}
	return false;
}



StoredCommands::Key StoredCommands::AppendSceneCommand_LinkPivot( int nPivotID, int nLinkToID )
{
    Command c;  MMAPI_INIT_SCENE_COMMAND(c, LinkPivot);
    c.c.scene.nObjectIDs.append(nPivotID);
    c.c.scene.nObjectIDs.append(nLinkToID);
    return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_UnlinkPivot( int nPivotID )
{
    Command c;  MMAPI_INIT_SCENE_COMMAND(c, LinkPivot);
    c.c.scene.nObjectIDs.append(nPivotID);
    c.c.scene.nObjectIDs.append(-1);
    return append_command(c);
}



StoredCommands::Key StoredCommands::AppendSceneCommand_CreateLiveMeshObject( const char * pFilename )
{
	MMAPI_SCENE_COMMAND_STR(CreateLiveMeshObject, pFilename);
}
bool StoredCommands::GetSceneCommandResult_CreateLiveMeshObject( Key k, std::string & portName, int & nObjectID )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK == 0 )
		return false;
	portName = std::string(c.r.scene.str.data);
	nObjectID = c.r.scene.nObjectIDs.data[0];
	return true;
}
bool StoredCommands::GetSceneCommandResult_CreateLiveMeshObject( Key k, std::vector<unsigned char> & portName, any_result & nObjectID )
{
	std::string port; int nid;
	if ( GetSceneCommandResult_CreateLiveMeshObject(k, port, nid) ) {
		portName = __toucstr(port);
		nObjectID.type = 1;
		nObjectID.i = nid;
		return true;
	}
	return false;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_RequestLiveMeshLock( const char * pPortName )
{
	MMAPI_SCENE_COMMAND_STR(RequestLiveMeshLock, pPortName);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_ReleaseLiveMeshLock( const char * pPortName )
{
	MMAPI_SCENE_COMMAND_STR(ReleaseLiveMeshLock, pPortName);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_NotifyLiveMeshUpdate( const char * pPortName )
{
	MMAPI_SCENE_COMMAND_STR(NotifyLiveMeshUpdated, pPortName);
}



StoredCommands::Key StoredCommands::AppendSceneCommand_CreateTrackingLiveMesh( const char * pFilename, int nObjectID, int nUDPNotificationPort)
{
    Command c;  MMAPI_INIT_SCENE_COMMAND(c, CreateTrackingLiveMesh);
    c.c.scene.nObjectIDs.append(nObjectID);
    c.c.scene.nObjectIDs.append(nUDPNotificationPort);
    return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_CreateTrackingLiveMesh( Key k, std::string & portName )
{
	if ( k < m_vCommands.size() && m_vCommands[k].r.scene.OK ) {
		Command & c = m_vCommands[k];
		portName = std::string(c.r.scene.str.data);	
		return true;
	}
	return false;
}
bool StoredCommands::GetSceneCommandResult_CreateTrackingLiveMesh( Key k, std::vector<unsigned char> & portName )
{
	std::string s;
	bool bOK = GetSceneCommandResult_CreateTrackingLiveMesh(k, s);
	portName = __toucstr(s);
	return bOK;
}
StoredCommands::Key StoredCommands::AppendSceneCommand_HaltTrackingLiveMesh( const char * pPortName )
{
	MMAPI_SCENE_COMMAND_STR(CreateTrackingLiveMesh, pPortName);
}




StoredCommands::Key StoredCommands::AppendSceneCommand_ListObjects()
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = ListObjects;
	return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_ListObjects( Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK == 0 )
		return false;
	mmsc_extract_vector(vObjects, c.r.scene.nObjectIDs);
	return true;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_ListSelectedObjects()
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = ListSelectedObjects;
	return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_ListSelectedObjects( Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK == 0 )
		return false;
	mmsc_extract_vector(vObjects, c.r.scene.nObjectIDs);
	return true;
}



StoredCommands::Key StoredCommands::AppendSceneCommand_SelectObjects(const std::vector<int> & vObjects)
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = SelectObjects;
	mmsc_init_vector(c.c.scene.nObjectIDs, vObjects);
	return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_SelectObjects( Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	mmsc_extract_vector(vObjects, c.r.scene.nObjectIDs);
	return ( c.r.scene.OK == 0 ) ? false : true;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_DeleteSelectedObjects()
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = DeleteSelected;
	return append_command(c);
}


StoredCommands::Key StoredCommands::AppendSceneCommand_SetAsTarget()
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = SetAsTarget;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_ClearTarget()
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = ClearTarget;
	return append_command(c);
}




StoredCommands::Key StoredCommands::AppendSceneCommand_GetObjectName(int nObjectID)
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = GetObjectName;
	c.c.scene.nObjectIDs.append(nObjectID);
	return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_GetObjectName( Key k, std::string & objectName )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK != 0 )
		objectName = std::string(c.r.scene.str.data);
	return ( c.r.scene.OK == 0 ) ? false : true;
}
bool StoredCommands::GetSceneCommandResult_GetObjectName( Key k, std::vector<unsigned char> & objectName )
{
	std::string objName;
	if ( GetSceneCommandResult_GetObjectName(k, objName) ) {
		for ( unsigned int k = 0; k < objName.length(); ++k )
			objectName.push_back(objName[k]);
		objectName.push_back('\0');
		return true;
	} 
	return false;
}

StoredCommands::Key StoredCommands::AppendSceneCommand_SetObjectName(int nObjectID, const std::string & objectName )
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = SetObjectName;
	c.c.scene.nObjectIDs.append(nObjectID);
	c.c.scene.str = __tofstr(objectName.c_str());
	return append_command(c);
}

StoredCommands::Key StoredCommands::AppendSceneCommand_FindObjectByName(const std::string & objectName)
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = FindObjectByName;
	c.c.scene.str = __tofstr(objectName.c_str());
	return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_FindObjectByName( Key k, int & nObjectID )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK != 0 )
		nObjectID = c.r.scene.nObjectIDs.data[0];
	return ( c.r.scene.OK == 0 ) ? false : true;
}
bool StoredCommands::GetSceneCommandResult_FindObjectByName( Key k, any_result & nObjectID )
{
	int nObjID;
	if ( GetSceneCommandResult_FindObjectByName(k, nObjID) ) {
		nObjectID.type = 1;
		nObjectID.i = nObjID;
		return true;
	}
	return false;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_GetObjectType( int nObjectID )
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = GetObjectType;
	c.c.scene.nObjectIDs.append(nObjectID);
	return append_command(c);
}
bool StoredCommands::GetSceneCommandResult_GetObjectType( StoredCommands::Key k, int & nObjectType )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK != 0 )
		nObjectType = c.r.scene.nObjectIDs.data[0];
	return ( c.r.scene.OK == 0 ) ? false : true;
}
bool StoredCommands::GetSceneCommandResult_GetObjectType( Key k, any_result & nObjectType )
{
	int ntype;
	if ( GetSceneCommandResult_GetObjectType(k, ntype) ) {
		nObjectType.type = 1;
		nObjectType.i = ntype;
		return true;
	}
	return false;
}


StoredCommands::Key StoredCommands::AppendSceneCommand_GetObjectFrame( int nObjectID )
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = GetObjectFrame;
	c.c.scene.nObjectIDs.append(nObjectID);
	return append_command(c);
}
 bool StoredCommands::GetSceneCommandResult_GetObjectFrame( StoredCommands::Key k, frame3f & f )
 {
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	if ( c.r.scene.OK != 0 )
		f = c.r.scene.f;
	return ( c.r.scene.OK == 0 ) ? false : true;
 }


 StoredCommands::Key StoredCommands::AppendSceneCommand_SetObjectFrame( int nObjectID, const frame3f & f )
{
	Command c;  c.init();
	c.eType = SceneCommand;
	c.c.scene.eType = SetObjectFrame;
	c.c.scene.nObjectIDs.append(nObjectID);
	c.c.scene.f = f;
	return append_command(c);
}
 bool StoredCommands::GetSceneCommandResult_SetObjectFrame( StoredCommands::Key k )
 {
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	return ( c.r.scene.OK == 0 ) ? false : true;
 }



StoredCommands::Key StoredCommands::AppendSceneCommand_SetVisible( int nObjectID )
{
	Command c;  
	MMAPI_INIT_SCENE_COMMAND(c, SetVisible);
	c.c.scene.nObjectIDs.append(nObjectID);
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_SetHidden( int nObjectID )
{
	Command c;  
	MMAPI_INIT_SCENE_COMMAND(c, SetHidden);
	c.c.scene.nObjectIDs.append(nObjectID);
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSceneCommand_ShowAll()
{
	Command c;  
	MMAPI_INIT_SCENE_COMMAND(c, ShowAll);
	return append_command(c);
}



StoredCommands::Key StoredCommands::AppendSceneCommand_SelectPrinter( const char * pPrinterName )
{
	MMAPI_SCENE_COMMAND_STR(SelectPrinter, pPrinterName);
}



/*
 * Spatial Query Commands
 */
#define MMAPI_INIT_SQUERY_COMMAND(c, cmdtype) c.init(); c.eType = SpatialQueryCommand; c.c.spatial.eType = cmdtype;


StoredCommands::Key StoredCommands::AppendQueryCommand_FindNearestPoint( float x, float y, float z )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, NearestPointSpatialQuery);
	c.c.spatial.p.x = x;	c.c.spatial.p.y = y;	c.c.spatial.p.z = z;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendQueryCommand_FindNearestPoint( const vec3f & p )
{
	return AppendQueryCommand_FindNearestPoint(p.x,p.y,p.z);
}
bool StoredCommands::GetQueryResult_FindNearestPoint( Key k, frame3f * pFrame )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	pFrame->origin_x = c.r.spatial.v.data[0];	pFrame->origin_y = c.r.spatial.v.data[1];	pFrame->origin_z = c.r.spatial.v.data[2];
	pFrame->normal_x = c.r.spatial.v.data[3];	pFrame->normal_y = c.r.spatial.v.data[4];	pFrame->normal_z = c.r.spatial.v.data[5];
	pFrame->tan1_x = c.r.spatial.v.data[6];		pFrame->tan1_y = c.r.spatial.v.data[7];		pFrame->tan1_z = c.r.spatial.v.data[8];
	pFrame->tan2_x = c.r.spatial.v.data[9];		pFrame->tan2_y = c.r.spatial.v.data[10];	pFrame->tan2_z = c.r.spatial.v.data[11];
	return true;
}



StoredCommands::Key StoredCommands::AppendQueryCommand_FindRayIntersection( float ox, float oy, float oz, float dx, float dy, float dz )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, RayIntersectionSpatialQuery);
	c.c.spatial.p = make_vec3f(ox,oy,oz);
	c.c.spatial.d = make_vec3f(dx,dy,dz);
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendQueryCommand_FindRayIntersection( const vec3f & o, const vec3f & d )
{
	return AppendQueryCommand_FindRayIntersection(o.x,o.y,o.z, d.x,d.y,d.z);
}
bool StoredCommands::GetQueryResult_FindRayIntersection( StoredCommands::Key k, frame3f * pFrame )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	pFrame->origin_x = c.r.spatial.v.data[0];	pFrame->origin_y = c.r.spatial.v.data[1];	pFrame->origin_z = c.r.spatial.v.data[2];
	pFrame->normal_x = c.r.spatial.v.data[3];	pFrame->normal_y = c.r.spatial.v.data[4];	pFrame->normal_z = c.r.spatial.v.data[5];
	pFrame->tan1_x = c.r.spatial.v.data[6];		pFrame->tan1_y = c.r.spatial.v.data[7];		pFrame->tan1_z = c.r.spatial.v.data[8];
	pFrame->tan2_x = c.r.spatial.v.data[9];		pFrame->tan2_y = c.r.spatial.v.data[10];	pFrame->tan2_z = c.r.spatial.v.data[11];
	return true;
}






StoredCommands::Key StoredCommands::AppendQueryCommand_GetBoundingBox(  )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, SelectedObjectsBoundingBoxQuery);
	return append_command(c);
}
bool StoredCommands::GetQueryResult_GetBoundingBox( Key k, float fMin[3], float fMax[3] )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	fMin[0] = c.r.spatial.v.data[0];	fMin[1] = c.r.spatial.v.data[1];	fMin[2] = c.r.spatial.v.data[2];
	fMax[0] = c.r.spatial.v.data[3];	fMax[1] = c.r.spatial.v.data[4];	fMax[2] = c.r.spatial.v.data[5];
	return true;
}



StoredCommands::Key StoredCommands::AppendQueryCommand_GetObjectBoundingBox( int nObjectID )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, ObjectBoundingBoxQuery);

	c.c.spatial.p.x = nObjectID;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_GetObjectBoundingBox( Key k, float fMin[3], float fMax[3] )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;

	fMin[0] = c.r.spatial.v.data[0];	fMin[1] = c.r.spatial.v.data[1];	fMin[2] = c.r.spatial.v.data[2];
	fMax[0] = c.r.spatial.v.data[3];	fMax[1] = c.r.spatial.v.data[4];	fMax[2] = c.r.spatial.v.data[5];
	return true;
}
StoredCommands::Key StoredCommands::AppendQueryCommand_GetObjectLocalFrame( int nObjectID )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, ObjectLocalFrameQuery);

	c.c.spatial.p.x = nObjectID;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_GetObjectLocalFrame( Key k, frame3f * pFrame )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;

	float * pData[] = { & pFrame->origin_x, &pFrame->origin_y, &pFrame->origin_z, 
						&pFrame->tan1_x, &pFrame->tan1_y, &pFrame->tan1_z, 
						&pFrame->tan2_x, &pFrame->tan2_y, &pFrame->tan2_z,
						&pFrame->normal_x, &pFrame->normal_y, &pFrame->normal_z };
	for ( int k = 0; k < 12; ++k )
		*pData[k] = c.r.spatial.v.data[k];
	return true;
}




StoredCommands::Key StoredCommands::AppendQueryCommand_GetSelectedFacesBoundingBox(  )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, SelectedFacesBoundingBoxQuery);
	return append_command(c);
}
bool StoredCommands::GetQueryResult_GetSelectedFacesBoundingBox( Key k, float fMin[3], float fMax[3] )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	fMin[0] = c.r.spatial.v.data[0];	fMin[1] = c.r.spatial.v.data[1];	fMin[2] = c.r.spatial.v.data[2];
	fMax[0] = c.r.spatial.v.data[3];	fMax[1] = c.r.spatial.v.data[4];	fMax[2] = c.r.spatial.v.data[5];
	return true;
}


StoredCommands::Key StoredCommands::AppendQueryCommand_GetSelectedFacesCentroid(  )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, SelectedFacesCentroidQuery);
	return append_command(c);
}
bool StoredCommands::GetQueryResult_GetSelectedFacesCentroid( Key k, float fCentroid[3] )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	fCentroid[0] = c.r.spatial.v.data[0];	fCentroid[1] = c.r.spatial.v.data[1];	fCentroid[2] = c.r.spatial.v.data[2];
	return true;
}




// test if point is inside selected object
StoredCommands::Key StoredCommands::AppendQueryCommand_IsInsideObject( const vec3f & p )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, IsInsideObjectQuery);
	c.c.spatial.p = p;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_IsInsideObject( Key k )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	return ( c.r.spatial.OK != 0 );
}


StoredCommands::Key StoredCommands::AppendQueryCommand_SetObjectTypeFilter( int nFilter )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, SetObjectTypeFilter);
	c.c.spatial.p.x = nFilter;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendQueryCommand_ClearObjectTypeFilter()
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, ClearObjectTypeFilter);
	return append_command(c);
}


// find all objects hit by ray. Results are returned sorted by ray-hit parameter.
StoredCommands::Key StoredCommands::AppendQueryCommand_FindObjectsHitByRay( const vec3f & o, const vec3f & d )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, RayHitObjectsQuery);
	c.c.spatial.p = o; c.c.spatial.d = d;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_FindObjectsHitByRay( Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	mmsc_extract_vector(vObjects, c.r.spatial.v);
	return true;
}

// find closest object to point
StoredCommands::Key StoredCommands::AppendQueryCommand_FindNearestObject( const vec3f & p )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, NearestObjectQuery);
	c.c.spatial.p = p;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_FindNearestObject( Key k, int & nObjectID )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	nObjectID = (int)c.r.spatial.v.data[0];
	return true;
}
bool StoredCommands::GetQueryResult_FindNearestObject( Key k, any_result & nObjectID )
{
	return GetQueryResult_FindNearestObject(k, nObjectID.i);
}

// find objects within distance of point.
StoredCommands::Key StoredCommands::AppendQueryCommand_FindObjectsWithinDistance( const vec3f & p, float fDistance )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, ObjectsWithinRadiusQuery);
	c.c.spatial.p = p;  c.c.spatial.d.x = fDistance; 
	return append_command(c);
}
bool StoredCommands::GetQueryResult_FindObjectsWithinDistance( Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	mmsc_extract_vector(vObjects, c.r.spatial.v);
	return true;
}

// test if two objects intersect (requires that objects be meshes)
StoredCommands::Key StoredCommands::AppendQueryCommand_TestIntersection( int nObjectID, int nTestWithObjectID )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, ObjectIntersectionQuery);
	c.c.spatial.p.x = nObjectID; c.c.spatial.p.y = nTestWithObjectID;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_TestIntersection( Key k )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	return ( c.r.spatial.OK != 0 );
}
	
// Find all objects intersecting first object
StoredCommands::Key StoredCommands::AppendQueryCommand_FindIntersectingObjects( int nObjectID )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, FindIntersectingObjects);
	c.c.spatial.p.x = nObjectID;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_FindIntersectingObjects( Key k, std::vector<int> & vObjects )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	mmsc_extract_vector(vObjects, c.r.spatial.v);
	return true;
}



StoredCommands::Key StoredCommands::AppendQueryCommand_ListNumberOfHoles()
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, ListNumberOfHoles);
	return append_command(c);
}
bool StoredCommands::GetQueryResult_ListNumberOfHoles( Key k, int & nHoles )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	nHoles = (int)c.r.spatial.v.data[0];
	return true;
}
bool StoredCommands::GetQueryResult_ListNumberOfHoles( Key k, any_result & nHoles )
{
	return GetQueryResult_FindNearestObject(k, nHoles.i);
}



StoredCommands::Key StoredCommands::AppendQueryCommand_FindClosestHole(const vec3f & p)
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, FindClosestHole);
	c.c.spatial.p = p;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_FindClosestHole( Key k, int & nHoleID )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;
	nHoleID = (int)c.r.spatial.v.data[0];
	return true;
}
bool StoredCommands::GetQueryResult_FindClosestHole( Key k, any_result & nHoleID )
{
	return GetQueryResult_FindNearestObject(k, nHoleID.i);
}



StoredCommands::Key StoredCommands::AppendQueryCommand_GetHoleBoundingBox( int nHoleID )
{
	Command c;  	MMAPI_INIT_SQUERY_COMMAND(c, GetHoleBoundingBox);
	c.c.spatial.p.x = nHoleID;
	return append_command(c);
}
bool StoredCommands::GetQueryResult_GetHoleBoundingBox( Key k, float fMin[3], float fMax[3] )
{
	if ( k >= m_vCommands.size() )		return false;
	Command & c = m_vCommands[k];
	if ( c.r.spatial.OK == 0 )			return false;

	fMin[0] = c.r.spatial.v.data[0];	fMin[1] = c.r.spatial.v.data[1];	fMin[2] = c.r.spatial.v.data[2];
	fMax[0] = c.r.spatial.v.data[3];	fMax[1] = c.r.spatial.v.data[4];	fMax[2] = c.r.spatial.v.data[5];
	return true;
}





/*
 * Selection Commands
 */
#define MMAPI_INIT_SELECT_COMMAND(c, cmdtype) c.init(); c.eType = SelectCommand; c.c.select.eType = cmdtype; c.c.select.str[0] = '\0';


void StoredCommands::AppendSelectCommand_All( )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectAll);
	append_command(c);
}

void StoredCommands::AppendSelectUtilityCommand( std::string commandName )
{
	Command c;
	MMAPI_INIT_SELECT_COMMAND(c, SelectUtility);
	sprintf_s(c.c.select.str, sizeof(c.c.select.str), "%s", commandName.c_str());
	append_command(c);
}
void StoredCommands::AppendSelectUtilityCommand( std::string commandName, int nArgument )
{
	Command c;
	MMAPI_INIT_SELECT_COMMAND(c, SelectUtility);
	sprintf_s(c.c.select.str, sizeof(c.c.select.str), "%s", commandName.c_str());
	c.c.select.n = nArgument;
	append_command(c);
}


StoredCommands::Key StoredCommands::AppendSelectCommand_NearestComponent( float cx, float cy, float cz )
{
	return AppendSelectCommand_NearestComponent(cx,cy,cz,0);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_NearestComponent( float cx, float cy, float cz, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectNearestComponent);
	c.c.select.p.x = cx;	c.c.select.p.y = cy;	c.c.select.p.z = cz;
	c.c.select.n = nMode;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_ContainingComponent( float cx, float cy, float cz )
{
	return AppendSelectCommand_ContainingComponent(cx,cy,cz, 0);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_ContainingComponent( float cx, float cy, float cz, int nMode)
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectContainingComponent);
	c.c.select.p.x = cx;	c.c.select.p.y = cy;	c.c.select.p.z = cz;
	c.c.select.n = nMode;
	return append_command(c);
}


StoredCommands::Key StoredCommands::AppendSelectCommand_FirstComponentIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz )
{
	return AppendSelectCommand_FirstComponentIntersectingRay(ox,oy,oz, dx,dy,dz, 0);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_FirstComponentIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectFirstComponentIntersectingRay);
	c.c.select.p.x = ox;	c.c.select.p.y = oy;	c.c.select.p.z = oz;
	c.c.select.d.x = dx;	c.c.select.d.y = dy;	c.c.select.d.z = dz;
	c.c.select.n = nMode;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_AllComponentsIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz )
{
	return AppendSelectCommand_AllComponentsIntersectingRay(ox,oy,oz, dx,dy,dz, 0);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_AllComponentsIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectAllComponentsIntersectingRay);
	c.c.select.p.x = ox;	c.c.select.p.y = oy;	c.c.select.p.z = oz;
	c.c.select.d.x = dx;	c.c.select.d.y = dy;	c.c.select.d.z = dz;
	c.c.select.n = nMode;
	return append_command(c);
}


// parameter is 3D point
StoredCommands::Key StoredCommands::AppendSelectCommand_NearestTriangle( float cx, float cy, float cz )
{
	return AppendSelectCommand_NearestTriangle(cx,cy,cz,1);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_NearestTriangle( float cx, float cy, float cz, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectNearestTriangle);
	c.c.select.p.x = cx;	c.c.select.p.y = cy;	c.c.select.p.z = cz;
	c.c.select.n = nMode;
	return append_command(c);
}

// parameters are ray origin & direction
StoredCommands::Key StoredCommands::AppendSelectCommand_FirstTriangleIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz )
{
	return AppendSelectCommand_FirstTriangleIntersectingRay(ox,oy,oz,dx,dy,dz,1);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_FirstTriangleIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectFirstTriangleIntersectingRay);
	c.c.select.p.x = ox;	c.c.select.p.y = oy;	c.c.select.p.z = oz;
	c.c.select.d.x = dx;	c.c.select.d.y = dy;	c.c.select.d.z = dz;
	c.c.select.n = nMode;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_AllTrianglesIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz )
{
	return AppendSelectCommand_AllTrianglesIntersectingRay(ox,oy,oz,dx,dy,dz,1);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_AllTrianglesIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectAllTrianglesIntersectingRay);
	c.c.select.p.x = ox;	c.c.select.p.y = oy;	c.c.select.p.z = oz;
	c.c.select.d.x = dx;	c.c.select.d.y = dy;	c.c.select.d.z = dz;
	c.c.select.n = nMode;
	return append_command(c);
}




StoredCommands::Key StoredCommands::AppendSelectCommand_InsideSphere( float cx, float cy, float cz, float r )
{
	return AppendSelectCommand_InsideSphere(cx, cy, cz, r, 0);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_InsideSphere( float cx, float cy, float cz, float r, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectInsideSphere);
	c.c.select.p.x = cx;	c.c.select.p.y = cy;	c.c.select.p.z = cz;
	c.c.select.n = nMode;
	return append_command(c);
}
bool StoredCommands::GetSelectCommandResult_InsideSphere( Key k )
{
	if ( k >= m_vCommands.size() )
		return false;
	return ( m_vCommands[k].r.select.OK == 0 ) ? false : true;
}


StoredCommands::Key StoredCommands::AppendSelectCommand_InsideBox( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax )
{
	return AppendSelectCommand_InsideBox( xmin, xmax, ymin, ymax, zmin, zmax, 0 );
}
StoredCommands::Key StoredCommands::AppendSelectCommand_InsideBox( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectInsideBox);
	c.c.select.p.x = xmin;	c.c.select.p.y = ymin;	c.c.select.p.z = zmin;
	c.c.select.d.x = xmax;	c.c.select.d.y = ymax;	c.c.select.d.z = zmax;
	c.c.select.n = nMode;
	return append_command(c);
}
bool StoredCommands::GetSelectCommandResult_InsideBox( Key k )
{
	if ( k >= m_vCommands.size() )
		return false;
	return ( m_vCommands[k].r.select.OK == 0 ) ? false : true;
}



StoredCommands::Key StoredCommands::AppendSelectCommand_IntersectingObject( int nObjectID, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectIntersectingObject);
	c.c.select.n = nObjectID;
	c.c.select.r = (float)nMode;
	return append_command(c);
}


StoredCommands::Key StoredCommands::AppendSelectCommand_ByFaceGroups( const std::vector<int> & vGroupIDs )
{
	return AppendSelectCommand_ByFaceGroups(vGroupIDs, 0);
}
StoredCommands::Key StoredCommands::AppendSelectCommand_ByFaceGroups( const std::vector<int> & vGroupIDs, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectFaceGroups);
	mmsc_init_vector( c.c.select.vGroups, vGroupIDs );
	c.c.select.r = (float)nMode;
	return append_command(c);
}
bool StoredCommands::GetSelectCommandResult_ByFaceGroups( Key k )
{
	if ( k >= m_vCommands.size() )
		return false;
	return ( m_vCommands[k].r.select.OK == 0 ) ? false : true;
}


StoredCommands::Key StoredCommands::AppendSelectCommand_HoleBorderRing( int nHoleID, int nMode )
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, SelectHoleBorderRing);
	c.c.select.n = nHoleID;
	c.c.select.r = (float)nMode;
	return append_command(c);
}
bool StoredCommands::GetSelectCommandResult_HoleBorderRing( Key k )
{
	if ( k >= m_vCommands.size() )
		return false;
	return ( m_vCommands[k].r.select.OK == 0 ) ? false : true;
}




StoredCommands::Key StoredCommands::AppendSelectCommand_ListSelectedFaceGroups()
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, ListSelectedFaceGroups);
	return append_command(c);
}
bool StoredCommands::GetSelectCommandResult_ListSelectedFaceGroups( Key k, std::vector<int> & vGroupIDs )
{
	if ( k >= m_vCommands.size() || m_vCommands[k].r.select.OK == 0)
		return false;
	Command & c = m_vCommands[k];
	mmsc_extract_vector( vGroupIDs, c.r.select.vGroups );
	return true;
}


StoredCommands::Key StoredCommands::AppendSelectCommand_HasValidSelection()
{
	Command c;  
	MMAPI_INIT_SELECT_COMMAND(c, HasValidSelection);
	return append_command(c);
}
bool StoredCommands::GetSelectCommandResult_HasValidSelection( Key k )
{
	if ( k >= m_vCommands.size() )
		return false;
	Command & c = m_vCommands[k];
	return (m_vCommands[k].r.select.OK != 0);
}



StoredCommands::Key StoredCommands::AppendActionCommand_BrushStroke3D( const std::vector<brush_stamp> & vPoints )
{
	Command c; c.init();
	c.eType = BrushCommand;
	c.c.brush.eType = Stroke3D;
	size_t nCount = vPoints.size();
	c.c.brush.vStamps.nElements = (int)nCount * 4;
	for ( unsigned int k = 0; k < nCount; ++k ) {
		c.c.brush.vStamps.data[4*k] = vPoints[k].x;
		c.c.brush.vStamps.data[4*k+1] = vPoints[k].y;
		c.c.brush.vStamps.data[4*k+2] = vPoints[k].z;
		c.c.brush.vStamps.data[4*k+3] = vPoints[k].fPressure;
	}
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendActionCommand_BrushStamp3D( const vec3f & v0 )
{
	Command c; c.init();
	c.eType = BrushCommand;
	c.c.brush.eType = Stroke3D;
	c.c.brush.vStamps.append(v0.x); c.c.brush.vStamps.append(v0.y); c.c.brush.vStamps.append(v0.z); c.c.brush.vStamps.append(1.0f);
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendActionCommand_LinearBrushStroke3D( const vec3f & v0, const vec3f & v1, int nSteps )
{
	Command c; c.init();
	c.eType = BrushCommand;
	c.c.brush.eType = Stroke3D;
	if ( nSteps < 2) nSteps = 2;
	for ( int k = 0; k < nSteps; ++k ) {
		float t = (float)k / (float)(nSteps-1);
		float x = (1.0f-t)*v0.x + (t)*v1.x;
		float y = (1.0f-t)*v0.y + (t)*v1.y;
		float z = (1.0f-t)*v0.z + (t)*v1.z;
		c.c.brush.vStamps.append(x); c.c.brush.vStamps.append(y); c.c.brush.vStamps.append(z); c.c.brush.vStamps.append(1.0f);
	}
	return append_command(c);
}




StoredCommands::Key StoredCommands::AppendActionCommand_DropPartAtPoint( const char * pPartPath, const frame3f & f, float fRadius, bool bInteractive )
{
	Command c; c.init();
	c.eType = PartCommand;
	c.c.part.eType = DropPart;
	c.c.part.filename = __tofstr(pPartPath);
	c.c.part.f = f;
	c.c.part.r = fRadius;
	c.c.part.bFlag = bInteractive;
	return append_command(c);
}

StoredCommands::Key StoredCommands::AppendActionCommand_UpdateDropPart( const frame3f & f, float fRadius, bool bMinimizeRotation )
{
	Command c; c.init();
	c.eType = PartCommand;
	c.c.part.eType = UpdatePart;
	c.c.part.f = f;
	c.c.part.r = fRadius;
	c.c.part.bFlag = bMinimizeRotation;
	return append_command(c);
}
StoredCommands::Key StoredCommands::AppendActionCommand_AcceptDropPart( )
{
	Command c; c.init();
	c.eType = PartCommand;
	c.c.part.eType = AcceptPart;
	return append_command(c);
}
bool StoredCommands::GetActionCommandResult_DropPart( Key k, std::vector<int> & vNewGroups )
{
	if ( k >= m_vCommands.size() || m_vCommands[k].r.part.OK == 0 )
		return false;
	for ( unsigned int i = 0; i < m_vCommands[k].r.part.vNewGroups.nElements; ++i )
		vNewGroups.push_back( m_vCommands[k].r.part.vNewGroups.data[i] );
	return true;
}



StoredCommands::Key StoredCommands::AppendActionCommand_InsertPolygon( float x, float y, float z, float fRadius )
{
	Command c; c.init();
	c.eType = StampCommand;
	c.c.stamp.eType = InsertPolygonStamp;
	c.c.stamp.x = x; c.c.stamp.y = y; c.c.stamp.z = z;
	c.c.stamp.r = fRadius;
	return append_command(c);
}
bool StoredCommands::GetActionCommandResult_InsertPolygon( StoredCommands::Key k, int & nNewGroupID )
{
	if ( k >= m_vCommands.size() || m_vCommands[k].r.stamp.OK == 0 )
		return false;
	nNewGroupID = m_vCommands[k].r.stamp.nNewGroupID;
	return true;
}
bool StoredCommands::GetActionCommandResult_InsertPolygon( Key k, any_result & nNewGroupID )
{
	int gid;
	if ( GetActionCommandResult_InsertPolygon(k, gid) ) {
		nNewGroupID.type = 1;
		nNewGroupID.i = gid;
		return true;
	}
	return false;
}





void StoredCommands::Command::init()
{
	this->c.scene.str.nSize = 0;
	this->c.scene.nObjectIDs.nElements = 0;
	this->r.scene.str.nSize = 0;
	this->r.scene.nObjectIDs.nElements = 0;

	this->c.select.vGroups.nElements = 0;
	this->r.select.vGroups.nElements = 0;

	this->c.toolparam.v.str.nSize = 0;

	this->c.brush.vStamps.nElements = 0;

	this->c.part.filename.nSize = 0;

	this->c.generic_query.str.nSize = 0;
	this->r.generic_query.vList.nElements = 0;
}



#ifdef USING_MM_COMMAND_API

void StoredCommands::Execute(mm::MainWindow * pMainWin)
{
	// do nothing
}

bool StoredCommands::Execute_IO(MainWindow * pMainWin, unsigned int k)
{
	return false;
}
bool StoredCommands::Execute_Tool(MainWindow * pMainWin, unsigned int k)
{
	return false;
}
bool StoredCommands::Execute_Tool_Utility(MainWindow * pMainWin, unsigned int k)
{
	return false;
}
bool StoredCommands::Execute_Query(MainWindow * pMainWin, unsigned int k)
{
	return false;
}
bool StoredCommands::Execute_Scene(MainWindow * pMainWin, unsigned int k)
{
	return false;
}
bool StoredCommands::Execute_Select(MainWindow * pMainWin, unsigned int k)
{
	return false;
}

#else

#include <QtCore/QCoreApplication>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>

#include <mmMainWindow.hxx>
#include <SceneUIContext.h>
#include <ToolManager.h>

void StoredCommands::Execute(mm::MainWindow * pMainWin)
{
	int nCount = (int)m_vCommands.size();
	for ( int k = 0; k < nCount; ++k ) {
		const Command & c = m_vCommands[k];
		switch ( c.eType ) {

			case MouseEventCommand:
			case CameraControlCommand: {
					bool bOK = Execute_IO(pMainWin, k);
                    lgDevAssert(bOK);
				} break;


			case BeginToolCommand:
			case CompleteToolCommand:
			case ToolParameterCommand: {
					bool bOK = Execute_Tool(pMainWin, k);
                    lgDevAssert(bOK);
				} break;

			case SceneCommand: {
					bool bOK = Execute_Scene(pMainWin, k);
                    lgDevAssert(bOK);
				} break;

			case SelectCommand: {
					bool bOK = Execute_Select(pMainWin, k);
                    lgDevAssert(bOK);
				} break;

			case BrushCommand:
			case PartCommand:
			case StampCommand:
				{
					bool bOK = Execute_Tool(pMainWin, k);
                    lgDevAssert(bOK);
				} break;

			case GenericQueryCommand:
			case SpatialQueryCommand: {
					bool bOK = Execute_Query(pMainWin, k);
                    lgDevAssert(bOK);
				} break;


			default:
				break;		// do nothing
		}
	}
}

#endif
