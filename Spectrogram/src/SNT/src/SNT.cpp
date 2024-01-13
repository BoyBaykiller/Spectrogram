#include <SNT/SNT.h>
#include <SNT/PlayDevice.h>

namespace SNT
{
    static std::vector<std::string> MakeListFromNullTermCStrings(const char* input)
    {
        auto list = std::vector<std::string>();

        while (input[0] != '\0')
        {
            std::string newEl = input;
            input += newEl.size() + 1;

            list.emplace_back(newEl);
        }

        return list;
    }

    std::vector<std::string> GetAllCaptureDeviceNames()
    {
        const char* cNames = alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
        std::vector<std::string> names = MakeListFromNullTermCStrings(cNames);
        return names;
    }

    std::string GetDefaultCaptureDeviceName()
    {
        const char* cName = alcGetString(nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
        std::string name = cName;
        return name;
    }
}