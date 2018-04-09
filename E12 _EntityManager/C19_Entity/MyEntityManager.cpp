#include "MyEntity.h"
#include "MyEntityManager.h"
using namespace Simplex;

MyEntityManager* Simplex::MyEntityManager::m_Instance;

MyEntityManager* Simplex::MyEntityManager::GetInstance()
{
	if (m_Instance != nullptr)
	{
		return m_Instance;
	}

	return m_Instance = new MyEntityManager();
}

void Simplex::MyEntityManager::DeleteInstance()
{
	if (m_Instance != nullptr)
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
}


MyEntity* Simplex::MyEntityManager::GetEntity(String name)
{
	if (m_EntityList.size() == 0)
	{
		return nullptr;
	}

	return m_EntityList[0]->m_IDMap.find(name)->second;
}

void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	m_EntityList.push_back(new MyEntity(a_sFileName, a_sUniqueID));
}

void Simplex::MyEntityManager::DeleteEntity(String name)
{
	if (m_EntityList.size() == 0)
	{
		return;
	}

	MyEntity* item = m_EntityList[0]->m_IDMap.find(name)->second;

	for (unsigned int i = 0; i < m_EntityList.size(); i++)
	{
		if (item == m_EntityList[i])
		{
			m_EntityList.erase(m_EntityList.begin() + i);
		}
	}
	SafeDelete(item);
}

Simplex::MyEntityManager::MyEntityManager()
{
	m_EntityList = std::vector<MyEntity*>();
}

Simplex::MyEntityManager::~MyEntityManager() 
{
	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		MyEntity* entity = m_EntityList[i];
		SafeDelete(entity);
	}
	m_EntityList.clear();
}

MyEntityManager& Simplex::MyEntityManager::operator=(MyEntityManager const& other)
{ 
	return *this; 
}

