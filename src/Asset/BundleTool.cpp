#include "BundleTool.h"

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>


namespace SOF {

	BundleTool::BundleTool()  {
		this->m_GlobalOffset = 0;
	}

	BundleTool::~BundleTool() {}

	void BundleTool::WriteFiles(std::string folder_path)
	{
		YAML::Node index_node;
		index_node[YAML::Null] = YAML::Null; // Is this too much voodoo? -T.A.D
		this->m_GlobalOffset = 0;

		std::cout << folder_path << std::endl;

		std::ofstream strm_bundle;
		strm_bundle.open(folder_path + "bundle.txt");
		GetDataJPG("assets/test32.png", "test32x32", &strm_bundle, index_node);
		GetDataJPG("assets/test64.png", "test64x64", &strm_bundle, index_node);
		strm_bundle.close();

		std::ofstream strm_index;
		strm_index.open(folder_path + "index.yaml");
		strm_index << index_node;
		strm_index.close(); 
	}

	void BundleTool::GetDataJPG(std::string file, std::string name, std::ofstream* strm_bundle, YAML::Node index_node) 
	{
		int width, height, no_channels;
		unsigned char* data = stbi_load(file.c_str(), &width, &height, &no_channels, STBI_default);
		int data_size = width * height;
		index_node[name].push_back(0); // asset type
		index_node[name].push_back(this->m_GlobalOffset);
		index_node[name].push_back(data_size);
		index_node[name]["meta"]["res_x"] = width;
		index_node[name]["meta"]["res_y"] = height;
		this->m_GlobalOffset += data_size;
		strm_bundle->write(reinterpret_cast<const char*>(data), data_size);
		stbi_image_free(data);
	}
}