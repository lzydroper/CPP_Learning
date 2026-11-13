#include "DataStream.hpp"

void serialize::DataStream::write(const char* data, size_t size)
{
    reserve(size);
    auto m_size = m_data.size();
    m_data.resize(m_size + size);
    std::memcpy(m_data.data() + m_size, data, size);
}

void serialize::DataStream::write(const char* value)
{
    char type = DataType::STRING;
    write(reinterpret_cast<const char*>(&type), sizeof(char));
    int32_t size = static_cast<int32_t>(strlen(value));
    write(reinterpret_cast<const char*>(&size), sizeof(int32_t));
    write(value, size);
}

void serialize::DataStream::write(const std::string& value)
{
    write(value.c_str());
}

bool serialize::DataStream::read(std::string& value)
{
    if (m_index >= m_data.size() ||
        static_cast<char>(m_data[m_index]) != DataType::STRING)
        return false;
    m_index += sizeof(char);
    int32_t size = 0;
    memcpy(&size, m_data.data() + m_index, sizeof(int32_t));
    m_index += sizeof(int32_t);
    value = std::string(m_data.data() + m_index, size);
    m_index += size;
    return true;
}

bool serialize::DataStream::save(const std::string& filename)
{
    // 以二进制、截断模式打开文件
    std::ofstream ofs(filename, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open())
    return false;
    
    // 写入数据大小和完整字节流数据
    uint64_t size = m_data.size();
    ofs.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
    if (size > 0)
        ofs.write(m_data.data(), size);

    // 检查写入过程是否发生错误
    return ofs.good();
}

bool serialize::DataStream::load(const std::string& filename)
{
    // 以二进制模式打开文件
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open())
        return false;

    uint64_t size = 0;
    if (!ifs.read(reinterpret_cast<char*>(&size), sizeof(uint64_t)))
        return false;

    m_index = 0;
    m_data.clear();

    if (size > 0)
    {
        reserve(size);
        m_data.resize(size);
        if (!ifs.read(m_data.data(), size))
            return false;
    }
    
    return true;
}

void serialize::DataStream::reserve(size_t size)
{
    auto m_size = m_data.size();
    auto m_capa = m_data.capacity();
    if (m_size + size > m_capa)
    {
        while (m_size + size > m_capa)
            if (m_capa == 0)
                m_capa = 1;
            else
                m_capa *= 2;
        m_data.reserve(m_capa);
    }
}