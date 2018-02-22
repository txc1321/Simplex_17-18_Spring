#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	static DWORD dStartTime = GetTickCount();
	DWORD dTimer = GetTickCount() - dStartTime;
	float fTimer = dTimer / 1000.0f;

	static float fPercentage = 0.0f;
	vector3 v3Start(-10.0f, 0.0f, 0.0f);
	vector3 v3End(10.0f, 0.0f, 0.0f);
	vector3 v3Current = glm::lerp(v3Start, v3End, fPercentage);
	fPercentage += 0.01f;


	m_m4Model = glm::translate(IDENTITY_M4, v3Current);

	m_pMesh->Render(m4Projection, m4View, m_m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

#pragma region Debugging Information
	//Print info on the screen
	uint nEmptyLines = 21;
	for (uint i = 0; i < nEmptyLines; ++i)
		m_pMeshMngr->PrintLine("");//Add a line on top
								   //m_pMeshMngr->Print("						");
	m_pMeshMngr->PrintLine(m_pSystem->GetAppName(), C_YELLOW);

	//m_pMeshMngr->Print("						");
	m_pMeshMngr->Print("Timer: ");//Add a line on top
	m_pMeshMngr->PrintLine(std::to_string(fTimer), C_YELLOW);

	//m_pMeshMngr->Print("						");
	m_pMeshMngr->Print("FPS:");
	m_pMeshMngr->Print(std::to_string(m_pSystem->GetFPS()), C_RED);
#pragma endregion
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}