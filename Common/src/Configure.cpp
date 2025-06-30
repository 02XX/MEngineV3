#include "Configure.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>


namespace MEngine
{
Configure::Configure()
{
    std::cout << "Configuration file path: " << mPath.string() << std::endl;
    std::ifstream file(mPath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open configuration file: " + mPath.string());
    }
    try
    {
        mJson = Json::parse(file);
        std::cout << "Configuration file loaded successfully." << std::endl;
        file.close();
    }
    catch (const Json::exception &e)
    {
        file.close();
        throw std::runtime_error("JSON parsing failed: " + std::string(e.what()));
    }
}

const Json &Configure::GetJson() const
{
    if (mJson.empty())
    {
        throw std::runtime_error("Configuration file is not loaded or empty.");
    }
    return mJson;
}
} // namespace MEngine