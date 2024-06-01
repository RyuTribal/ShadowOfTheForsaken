#include "AssetManager.h"


namespace SOF {
	AssetManager::AssetManager(std::string path)
	{
		this->m_FolderPath = path;
		this->m_AssetBundleTool.WriteFiles(path + "assets/bundle/");

		// load index.yaml
		this->m_ResourceIndex["lower"] = m_Resource{ glTex, 0, 4  };
	}

	AssetManager::~AssetManager() 
	{ 
	}

	int AssetManager::GetGLTexID(std::string key) 
	{
		if (this->m_ResourceIndex.find(key) == this->m_ResourceIndex.end()) {
			throw std::exception("Key does not exist in lookup");
		}

		m_ResourceType res_type = this->m_ResourceIndex[key].type;
		if (res_type != glTex) {
			throw std::exception("This resource is not a GL texture asset");
		}

		int offset = this->m_ResourceIndex[key].offset;
		int len = this->m_ResourceIndex[key].chunk_len;

		char* buff = new char[len];
		this->ReadFromStream(offset, len, buff);
		std::cout.write(buff, len) << std::endl;
		
		bool debug_skip_parse = true;
		if (!debug_skip_parse) {
			unsigned int tex_id;
			glGenTextures(1, &tex_id);

			delete[] buff;
			return tex_id;
		}
		
		delete[] buff;
		return -1;
	}

	void AssetManager::ReadFromStream(int offset, int chunk_len, char* out) 
	{
		std::ifstream stream;
		stream.open(this->m_FolderPath + "bundle.txt");
		stream.seekg(offset);
		stream.read(out, chunk_len);
		stream.close();
	}
}