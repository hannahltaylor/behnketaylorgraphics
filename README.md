# behnketaylorgraphics
Advanced Computer Graphics Final Project

Tutorial: https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map 
          https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation


Compile command:
g++ -I..\ingredients\glad\include\ -I..\ingredients\ .\*.cpp ..\ingredients\glad\src\glad.c ..\ingredients\glutils.cpp ..\ingredients\trianglemesh.cpp ..\ingredients\teapotpatch.cpp ..\ingredients\teapot.cpp ..\ingredients\glslprogram.cpp ..\ingredients\texture.cpp ..\ingredients\stbimpl.cpp ..\ingredients\skybox.cpp -lglfw3 -lopengl32  -lglu32 -lgdi32