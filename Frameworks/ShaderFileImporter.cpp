#include "ShaderFileImporter.h"

#include <fstream>
#include <cassert>

ShaderFileImporter ShaderFileImporter::LoadFile(const wchar_t * file_name)
{
	ShaderFileImporter output;
	std::ifstream ifs;

	ifs.open(file_name, std::ios::in | std::ios::binary);

	assert(ifs);

	ifs.seekg(0, std::fstream::end);
	std::streamoff eof_pos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::fstream::beg);
	std::streamoff beg_pos = ifs.tellg();
	output.m_size = (unsigned int)(eof_pos - beg_pos);

	output.m_data.reset(new char[output.m_size]);

	ifs.read(output.m_data.get(), output.m_size);

	ifs.close();

	return std::move(output);
}

ShaderFileImporter::ShaderFileImporter()
	: m_size(0)
{
}

ShaderFileImporter::ShaderFileImporter(ShaderFileImporter && in)
{
	m_data = std::move(in.m_data);
	m_size = in.m_size;
}