#pragma once
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace SOF {
	class BundleTool
	{
	public:
		BundleTool();
		~BundleTool();
		void WriteFiles(std::string folder_path);

	private:
		int m_GlobalOffset;
		void GetDataJPG(std::string file, std::string name, std::ofstream* strm_bundle, YAML::Node index_node);
	};
}
