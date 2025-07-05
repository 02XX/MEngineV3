#include "MEngineEditor.hpp"

int main()
{
    MEngine::Editor::MEngineEditor editor;
    editor.Init();
    editor.Run(0.016f); // 60 FPS
    editor.Shutdown();
    return 0;
}