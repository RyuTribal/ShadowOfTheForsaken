#pragma once
#include <unordered_map>
#include <iostream>
#include <utility>
#include <glad/gl.h>
#include "BundleTool.h"


namespace SOF 
{
	class AssetManager
	{
	public:
		AssetManager(std::string folder_path);
		~AssetManager();
		int GetGLTexID(std::string resource_key);

	private:
		enum m_ResourceType {
			glTex
		};

		struct m_Resource {
			m_ResourceType type;
			int offset;
			int chunk_len;
		};

		std::string m_FolderPath;
		std::unordered_map<std::string, m_Resource> m_ResourceIndex;
		SOF::BundleTool m_AssetBundleTool;

		void ReadFromStream(int offset, int chunk_len, char* char_buff);
	};
}

