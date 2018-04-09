#pragma once
#include "MyEntity.h"

namespace Simplex
{

	class MyEntityManager
	{
	public:
		// Get the singleton reference or makes a new one if none exists
		static MyEntityManager* GetInstance();

		// Delete the singelton if one currently exists
		void DeleteInstance();

		// Get a specific entity pointer from the manager given a name identifier
		MyEntity* GetEntity(String name);

		// Add a specific entity to the manager given a file name and name identifier
		void AddEntity(String a_sFileName, String a_sUniqueID = "NA");

		// Delete a specific entity from the manager given a name identifier
		void DeleteEntity(String name);

	private:
		// Default Constructor (private due to being a singleton)
		MyEntityManager();

		// Default Deconstructor (private due to being a singleton)
		~MyEntityManager();

		// Equals operator overload
		MyEntityManager& operator=(MyEntityManager const& other);

		// Static insance identifier used in singleton logic
		static MyEntityManager* m_Instance;

		// Vector of all entity pointers in this manager
		std::vector<MyEntity*> m_EntityList;
	};

}