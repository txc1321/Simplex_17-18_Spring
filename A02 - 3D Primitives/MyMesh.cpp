#include "MyMesh.h"
#include <valarray>

void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 point0(0.0f, a_fHeight / 2, 0.0f);
	vector3 pointE(0.0f, -a_fHeight / 2, 0.0f);

	point0 = point0 * vector3(a_fRadius, 1.0f, a_fRadius);
	pointE = pointE * vector3(a_fRadius, 1.0f, a_fRadius);

	std::vector<vector3> verts;

	//create vertices
	for(int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), -a_fHeight / 2, sin(angle));
		verts.push_back(point * vector3(a_fRadius, 1.0f, a_fRadius));
	}

	//create side tris
	for(int i = 0; i < a_nSubdivisions; i++)
	{
		if(i == a_nSubdivisions-1)
		{
			AddTri(verts[0], verts[a_nSubdivisions-1], point0);
		}
		else
		{
			AddTri(verts[i + 1], verts[i], point0);
		}
	}

	//create floor tris
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddTri(verts[a_nSubdivisions - 1], verts[0], pointE);
		}
		else
		{
			AddTri(verts[i], verts[i + 1], pointE);
		}
	}

	//GenerateCube(a_fRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code

	vector3 pointR(0.0f, a_fHeight / 2, 0.0f);
	vector3 pointF(0.0f, -a_fHeight / 2, 0.0f);

	pointR = pointR * vector3(a_fRadius, 1.0f, a_fRadius);
	pointF = pointF * vector3(a_fRadius, 1.0f, a_fRadius);

	std::vector<vector3> floorVerts;
	std::vector<vector3> roofVerts;

	//create floor vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), -a_fHeight / 2, sin(angle));
		floorVerts.push_back(point * vector3(a_fRadius, 1.0f, a_fRadius));
	}

	//create roof vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), a_fHeight / 2, sin(angle));
		roofVerts.push_back(point * vector3(a_fRadius, 1.0f, a_fRadius));
	}

	//create roof tris
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddTri(roofVerts[a_nSubdivisions - 1], pointR, roofVerts[0]);
		}
		else
		{
			AddTri(roofVerts[i], pointR, roofVerts[i + 1]);
		}
	}

	//create side quads
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddQuad(roofVerts[a_nSubdivisions - 1], roofVerts[0], floorVerts[a_nSubdivisions - 1], floorVerts[0]);
		}
		else
		{
			AddQuad(roofVerts[i], roofVerts[i + 1], floorVerts[i], floorVerts[i + 1]);
		}
	}

	//create floor tris
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddTri(floorVerts[a_nSubdivisions - 1], floorVerts[0], pointF);
		}
		else
		{
			AddTri(floorVerts[i], floorVerts[i + 1], pointF);
		}
	}

	//GenerateCube(a_fRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> outerFloorVerts;
	std::vector<vector3> innerFloorVerts;
	std::vector<vector3> outerRoofVerts;
	std::vector<vector3> innerRoofVerts;

	//create outer floor vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), -a_fHeight / 2, sin(angle));
		outerFloorVerts.push_back(point * vector3(a_fOuterRadius, 1.0f, a_fOuterRadius));
	}

	//create inner floor vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), -a_fHeight / 2, sin(angle));
		innerFloorVerts.push_back(point * vector3(a_fInnerRadius, 1.0f, a_fInnerRadius));
	}

	//create outer roof vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), a_fHeight / 2, sin(angle));
		outerRoofVerts.push_back(point * vector3(a_fOuterRadius, 1.0f, a_fOuterRadius));
	}

	//create inner roof vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (2 * PI * i) / a_nSubdivisions;
		vector3 point(cos(angle), a_fHeight / 2, sin(angle));
		innerRoofVerts.push_back(point * vector3(a_fInnerRadius, 1.0f, a_fInnerRadius));
	}

	//create roof quads
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddQuad(outerRoofVerts[0], outerRoofVerts[a_nSubdivisions-1], innerRoofVerts[0], innerRoofVerts[a_nSubdivisions-1]);
		}
		else
		{
			AddQuad(outerRoofVerts[i + 1], outerRoofVerts[i], innerRoofVerts[i + 1], innerRoofVerts[i]);
		}
	}

	//create outer quads
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddQuad(outerRoofVerts[a_nSubdivisions - 1], outerRoofVerts[0], outerFloorVerts[a_nSubdivisions - 1], outerFloorVerts[0]);
		}
		else
		{
			AddQuad(outerRoofVerts[i], outerRoofVerts[i + 1], outerFloorVerts[i], outerFloorVerts[i+1]);
		}
	}

	//create floor quads
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddQuad(outerFloorVerts[a_nSubdivisions - 1], outerFloorVerts[0], innerFloorVerts[a_nSubdivisions - 1], innerFloorVerts[0]);
		}
		else
		{
			AddQuad(outerFloorVerts[i], outerFloorVerts[i + 1], innerFloorVerts[i], innerFloorVerts[i + 1]);
		}
	}

	//create inner quads
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i == a_nSubdivisions - 1)
		{
			AddQuad(innerFloorVerts[a_nSubdivisions - 1], innerFloorVerts[0], innerRoofVerts[a_nSubdivisions - 1], innerRoofVerts[0]);
		}
		else
		{
			AddQuad(innerFloorVerts[i], innerFloorVerts[i + 1], innerRoofVerts[i], innerRoofVerts[i + 1]);
		}
	}

	//GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}