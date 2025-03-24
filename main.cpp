#include <iostream>
#include <fstream>
using namespace std;

class DeviceReader {
private:
    string device_path;

public:
    DeviceReader(string device_path)
    {
        device_path = device_path;
    }

    int read_from_device(void)
    {
        std::fstream device_file("/dev/sysinfo");

        if (!device_file)
        {
            std::cerr << "Error opening file" << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(device_file, line))
        {
            std::cout << line << std::endl;
        }

        device_file.close();
        return 0;
    }

    
};

int main(void)
{
    DeviceReader s1("servername");
    s1.read_from_device();

    return EXIT_SUCCESS;
}