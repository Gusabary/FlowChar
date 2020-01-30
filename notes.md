# Notes

开发过程中遇到的一些值得记录的东西。

+ 智能指针的一些用法在 c++ 11 甚至 14 之后才出现，在 `CMakeLists.txt` 中指定：

  ```cmake
  set(CMAKE_CXX_STANDARD 14)
  ```

+ char * 比较要用 `strcmp`，string 比较可以用 `==`

##### Last-modified date: 2020.1.30, 8 p.m.