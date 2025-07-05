#include "MEngineEditor.hpp"
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>
using namespace MEngine::Editor;
namespace nlohmann
{
template <> struct adl_serializer<WindowConfig>
{
    static void to_json(json &j, const WindowConfig &config)
    {
        j["WindowConfig"]["Width"] = config.width;
        j["WindowConfig"]["Height"] = config.height;
        j["WindowConfig"]["Title"] = config.title;
        j["WindowConfig"]["Resizable"] = config.resizable;
        j["WindowConfig"]["Fullscreen"] = config.fullscreen;
        j["WindowConfig"]["VSync"] = config.vsync;
    }

    static void from_json(const json &j, WindowConfig &config)
    {
        config.width = j["WindowConfig"].value("Width", 1280);
        config.height = j["WindowConfig"].value("Height", 720);
        config.title = j["WindowConfig"].value("Title", "MEngine Editor");
        config.fullscreen = j["WindowConfig"].value("Fullscreen", false);
        config.resizable = j["WindowConfig"].value("Resizable", true);
        config.vsync = j["WindowConfig"].value("VSync", true);
    }
};
} // namespace nlohmann
