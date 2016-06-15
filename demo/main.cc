#include <kxhuffman/huffman.h>

#include <string>
#include <fstream>
#include <cstdio>

using namespace kxh;

void filename_and_extension (const std::string& path,
                             std::string& name, std::string& extension)
{
    std::size_t pos = path.rfind('.');
    if (pos != std::string::npos)
    {
        name = path.substr(0, pos);
        extension = path.substr(pos+1);
    }
    else
    {
        name = path;
        extension = "";
    }
}

std::string read_file (const char* path)
{
    std::ifstream f(path, std::ios::binary);
    f.seekg(0, std::ios::end);
    std::size_t size = f.tellg();
    f.seekg(0, std::ios::beg);
    std::string data(size, 0);
    f.read(&data[0], size);
    return data;
}

void write_file (const char* path, const std::string& data)
{
    std::ofstream f(path, std::ios::binary);
    f.write(&data[0], data.size());
}

int main (int argc, const char** argv)
{
    try
    {
        if (argc != 2)
        {
            fprintf(stderr, "Usage: %s <file>\n", argv[0]);
            return 0;
        }

        const char* path = argv[1];
        std::string filename, extension;
        filename_and_extension(path, filename, extension);

        if (extension != "hef")
        {
            printf("Encoding %s\n", path);
            fflush(stdout);

            std::string data = read_file(path);
            BinaryBlob blob = kxh::encode<char>(data.begin(), data.end());
            std::string out = std::string(path) + ".hef";
            write_file(out.c_str(), blob);

            double ratio = (double) blob.size() / (double) data.size();
            printf("Compression ratio: %f%%\n", ratio*100);
        }
        else
        {
            printf("Decoding %s\n", path);
            fflush(stdout);

            BinaryBlob blob = read_file(path);
            std::string data;
            kxh::decode<char>(blob, data);
            write_file(filename.c_str(), data);
        }
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "exception: %s\n", e.what());
    }

    return 0;
}
