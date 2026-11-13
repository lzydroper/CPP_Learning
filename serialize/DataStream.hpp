#pragma once

#include <vector>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace serialize {


enum DataType
{
    BOOL = 0,
    CHAR = 1,
    INT32 = 2,
    INT64 = 3,
    FLOAT = 4,
    DOUBLE = 5,
    STRING = 6,
    VECTOR = 7,
    CUSTOM = 8,
};

template <typename T> struct DataTypeMap;
template <> struct DataTypeMap<bool>               { static constexpr char value = DataType::BOOL; };
template <> struct DataTypeMap<char>               { static constexpr char value = DataType::CHAR; };
template <> struct DataTypeMap<int32_t>            { static constexpr char value = DataType::INT32; };
template <> struct DataTypeMap<int64_t>            { static constexpr char value = DataType::INT64; };
template <> struct DataTypeMap<float>              { static constexpr char value = DataType::FLOAT; };
template <> struct DataTypeMap<double>             { static constexpr char value = DataType::DOUBLE; };

class DataStream
{
private:
    std::vector<char> m_data;
    // 数据流中正在读取的数据的起始索引，即指向type
    size_t m_index = 0;
public:
public:
    DataStream() = default;
    ~DataStream() = default;
public:
    /// @brief 写入数据到数据流
    /// @param data 要写入的数据指针
    /// @param size 要写入的数据大小
    void write(const char* data, size_t size);

    /// @brief 写入数据到数据流
    /// @param value 要写入的数据
    /// @tparam T 要写入的数据类型
    /// @note 数据类型必须是可序列化的类型
    template <typename T>
    void write(T value);

    // 重载写入字符串
    void write(const char* value);
    void write(const std::string& value);

    // 重载写入集合
    template <typename T>
    void write(const std::vector<T>& values);

    /// @brief 重载<<运算符，用于链式调用
    /// @param value 要写入的数据
    /// @tparam T 要写入的数据类型
    /// @note 数据类型必须是可序列化的类型
    /// @return DataStream& 当前数据流对象
    template <typename T>
    DataStream& operator<<(const T& value);

    /// @brief 从数据流读取数据
    /// @param value 要读取的数据
    /// @tparam T 要读取的数据类型
    /// @note 数据类型必须是可序列化的类型
    /// @return bool 是否成功读取数据
    template <typename T>
    bool read(T& value);

    // 重载读取字符串
    bool read(std::string& value);

    // 重载读取集合
    template <typename T>
    bool read(std::vector<T>& values);

    /// @brief 重载>>运算符，用于链式调用
    /// @param value 要读取的数据
    /// @tparam T 要读取的数据类型
    /// @note 数据类型必须是可序列化的类型，失败会跳过这个value
    /// @return DataStream& 当前数据流对象
    template <typename T>
    DataStream& operator>>(T& value);

    /// @brief 将当前数据流保存到二进制文件
    /// @param filename 文件路径
    /// @return bool 是否保存成功
    bool save(const std::string& filename);

    /// @brief 从二进制文件加载数据到当前数据流
    /// @param filename 文件路径
    /// @return bool 是否加载成功
    bool load(const std::string& filename);

private:
    /// @brief 预留数据流容量，避免频繁重新分配内存
    /// @param size 要预留的内存大小
    /// @note 数据类型必须是可序列化的类型
    void reserve(size_t size);
}; // DataStream


/// @brief 写入数据到数据流
/// @param value 要写入的数据
/// @tparam T 要写入的数据类型
/// @note 数据类型必须是可序列化的类型
template<typename T>
inline void DataStream::write(T value)
{
    char type = DataTypeMap<T>::value;
    write(reinterpret_cast<const char*>(&type), sizeof(char));
    write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
inline void DataStream::write(const std::vector<T>& values)
{
    char type = DataType::VECTOR;
    write(reinterpret_cast<const char*>(&type), sizeof(char));
    int32_t size = values.size();
    write(reinterpret_cast<const char*>(&size), sizeof(int32_t));
    for (int32_t i = 0; i < size; ++i)
        write(values[i]);
}

/// @brief 重载<<运算符，用于链式调用
/// @param value 要写入的数据
/// @tparam T 要写入的数据类型
/// @note 数据类型必须是可序列化的类型
/// @return DataStream& 当前数据流对象
template<typename T>
inline DataStream& DataStream::operator<<(const T& value)
{
    write(value);
    return *this;
}

/// @brief 从数据流读取数据
/// @param value 要读取的数据
/// @tparam T 要读取的数据类型
/// @note 数据类型必须是可序列化的类型
/// @return bool 是否成功读取数据
template<typename T>
inline bool DataStream::read(T& value)
{
    char type = DataTypeMap<T>::value;
    if (m_index >= m_data.size() ||
        static_cast<char>(m_data[m_index]) != type)
        return false;
    m_index += sizeof(char);
    memcpy(&value, m_data.data() + m_index, sizeof(T));
    m_index += sizeof(T);
    return true;
}

template<typename T>
inline bool DataStream::read(std::vector<T>& values)
{
    if (m_index >= m_data.size() ||
        static_cast<char>(m_data[m_index]) != DataType::VECTOR)
        return false;
    m_index += sizeof(char);
    int32_t size = 0;
    memcpy(&size, m_data.data() + m_index, sizeof(int32_t));
    m_index += sizeof(int32_t);
    values.resize(size);
    for (auto it = values.begin(); it != values.end(); ++it)
        read(*it);
    return true;
}

/// @brief 重载>>运算符，用于链式调用
/// @param value 要读取的数据
/// @tparam T 要读取的数据类型
/// @note 数据类型必须是可序列化的类型，失败会跳过这个value
/// @return DataStream& 当前数据流对象
template<typename T>
inline DataStream& DataStream::operator>>(T& value)
{
    read(value);
    return *this;
}

} // namespace serialize