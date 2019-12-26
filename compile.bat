@echo off 

start cmd /k "clang++ main.cpp -O3 Bwindow.cpp BDevice.cpp BModel.cpp BTexture.cpp math/BMatrix.cpp BCamera.cpp && a"
