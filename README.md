# MEngine

MEngine是基于Vulkan的一个渲染器。该引擎分为5层，由上到下分别是工具层（Tool）、功能层（Function）、资源层（Resource）、核心层（Core）、平台层（Platform）以及公用的第三方插件（3rdParty），目前实现了前向渲染管线以及基于物理的渲染。

窗口使用GLFW、UI使用ImGUI、使用VMA对vulkan资源进行管理
