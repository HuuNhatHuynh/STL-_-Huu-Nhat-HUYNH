#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>



struct Vertex
{
    float x, y, z;
};

struct Normal
{
    float x, y, z;
};

struct Facet
{
    Vertex vertices[3];
    Normal normal;
};

struct Mesh
{
    std::vector<Facet> facets;
    std::string name;
    std::string header;
};

void parse_file(const char * filename, Mesh & mesh);//lire fichier STL binaire

void write_file(const char * filename, const Mesh & mesh);//ecrire fichier STL binaire



////////////////////////////////////////////////////////////////////////
////////////////////////////Lecture de STL//////////////////////////////
////////////////////////////////////////////////////////////////////////



void consume(std::istream & is, const std::string & expected)
{
    std::string line, label;
    std::getline(is, line);
    std::istringstream ss(line);
    ss >> label;
    if(label != expected)
        throw;
}

std::istringstream getlinestream(std::istream & is)
{
    std::string line;
    std::getline(is, line);
    return std::istringstream(line);
}

template<typename T> T little_endian_to_native(T v)
{
    T vn = 0;
    for(unsigned int i = 0; i < sizeof(T); ++i) {
        vn += (T)((uint8_t*)&v)[i] << (8 * i);
    }
    return vn;
}

template<> float little_endian_to_native(float v)
{
    unsigned int vntemp = little_endian_to_native(*(uint32_t*)&v);
    return *(float*)&vntemp;
}

template<typename T> T read_binary_value(std::istream & is);

template<> uint16_t read_binary_value(std::istream & is)
{
    uint16_t value;
    is.read((char*)&value, 2);
    value = little_endian_to_native(value);
    return value;
}

template<> uint32_t read_binary_value(std::istream & is)
{
    uint32_t value;
    is.read((char*)&value, 4);
    value = little_endian_to_native(value);
    return value;
}

template<> float read_binary_value(std::istream & is)
{
    float value;
    is.read((char*)&value, 4);
    value = little_endian_to_native(value);
    return value;
}

template<> Normal read_binary_value(std::istream & is)
{
    Normal n;
    n.x = read_binary_value<float>(is);
    n.y = read_binary_value<float>(is);
    n.z = read_binary_value<float>(is);
    return n;
}

template<> Vertex read_binary_value(std::istream & is)
{
    Vertex v;
    v.x = read_binary_value<float>(is);
    v.y = read_binary_value<float>(is);
    v.z = read_binary_value<float>(is);
    return v;
}

const size_t STL_BINARY_HDR_SIZE = 80;

const size_t STL_BINARY_META_SIZE = sizeof(uint32_t); 

const size_t STL_BINARY_TRIANGLE_SIZE =
    3 * sizeof(float) +     
    3 * 3 * sizeof(float) + 
    sizeof(uint16_t);       

void parse_binary_stream(std::istream & is, Mesh & mesh)
{
    char header[STL_BINARY_HDR_SIZE + 1];
    is.read(header, STL_BINARY_HDR_SIZE);
    header[STL_BINARY_HDR_SIZE] = '\0';
    mesh.header = header;

    auto num_triangles = read_binary_value<uint32_t>(is);

    for(uint32_t ti = 0; ti < num_triangles; ++ti)
    {
        Facet facet = {};
        facet.normal = read_binary_value<Normal>(is);
        for(int vi = 0; vi < 3; ++vi)
        {
            facet.vertices[vi] = read_binary_value<Vertex>(is);
        }
        auto attrib_byte_count = read_binary_value<uint16_t>(is);
        mesh.facets.push_back(facet);
    }
}

void parse_stream(std::istream & is, Mesh & mesh)
{
    char header_start[6] = "";
    is.read(header_start, 5);
    header_start[5] = '\0';
    is.seekg(0, is.end);
    int file_size = is.tellg();

    is.seekg(0, is.beg);

    parse_binary_stream(is, mesh);
}

void parse_file(const char * filename, Mesh & mesh)
{
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::binary);
    parse_stream(ifs, mesh);
    ifs.close();
}



////////////////////////////////////////////////////////////////////////
//////////////////////////////Ecriture STL//////////////////////////////
////////////////////////////////////////////////////////////////////////



template<typename T> T native_to_little_endian(T v)
{
    T vn = 0;
    for(unsigned int i = 0; i < sizeof(T); ++i) {
        ((uint8_t*)&vn)[i] = (v >> (8 * i)) & 0xFF;
    }
    return vn;
}

template<> float native_to_little_endian(float v)
{
    unsigned int vntemp = native_to_little_endian(*(uint32_t*)&v);
    return *(float*)&vntemp;
}

template<typename T> void write_binary_value(std::ostream & os, const T & value);

template<> void write_binary_value(std::ostream & os, const uint16_t & value)
{
    uint16_t le_value = native_to_little_endian(value);
    os.write((char*)&le_value, 2);
}

template<> void write_binary_value(std::ostream & os, const uint32_t & value)
{
    uint32_t le_value = native_to_little_endian(value);
    os.write((char*)&le_value, 4);
}

template<> void write_binary_value(std::ostream & os, const float & value)
{
    float le_value = native_to_little_endian(value);
    os.write((char*)&le_value, 4);
}

template<> void write_binary_value(std::ostream & os, const Vertex & v)
{
    write_binary_value(os, v.x);
    write_binary_value(os, v.y);
    write_binary_value(os, v.z);
}

template<> void write_binary_value(std::ostream & os, const Normal & n)
{
    write_binary_value(os, n.x);
    write_binary_value(os, n.y);
    write_binary_value(os, n.z);
}

void write_stream(std::ostream & os, const Mesh & mesh)
{
    std::string padded_header = mesh.header;
    padded_header.resize(STL_BINARY_HDR_SIZE, '\0');
    os << padded_header;
    write_binary_value<uint32_t>(os, mesh.facets.size());
    for(auto & facet : mesh.facets)
    {
        write_binary_value(os, facet.normal);
        for(int vi = 0; vi < 3; ++vi)
        {
            write_binary_value(os, facet.vertices[vi]);
        }
        write_binary_value(os, uint16_t(0));
    }
}

void write_file(const char * filename, const Mesh & mesh)
{
    std::ofstream ofs(filename, std::ifstream::binary);
    write_stream(ofs, mesh);
}

