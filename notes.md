# Notes

开发过程中遇到的一些值得记录的东西。

+ 智能指针的一些用法在 c++ 11 甚至 14 之后才出现，在 `CMakeLists.txt` 中指定：

  ```cmake
  set(CMAKE_CXX_STANDARD 14)
  ```

+ char * 比较要用 `strcmp`，string 比较可以用 `==`

+ ```c++
  const int *p;
  int const *p;
  int *const p;
  ```

  前两种是说指向的东西是 const，第三种是说指针本身是 const。

  `const int *` 是不能赋值给 `int *` 的，反过来可以。  

+ 在指定类型别名的时候，`using` 可以用模板，`typedef` 不可以：

  ```c++
  template<typename T>
  using V = std::vector<T>;  // ok
  
  template<typename T>
  typedef std::vector<T> V;  // error
  
  using V = std::vector<int>;  // ok
  typedef std::vector<int> V;  // error
  ```

  `using` 比较新，编译的时候需要指定 c++11:

  ```bash
  g++ -std=c++11 a.cpp
  ```

+ ```c++
  vector<vector<int>> w;
  for (vector<int> &v : w) {
      // ...
  }
  ```

  如果不用**引用传递**的话，w 本身是不会变的。事实上值传递会拷贝构造一个 v。

+ 三目运算符的运算优先级其实很低，远比加法低得多：

  ```c++
  a + b ? c : d  // 实际上是 (a + b) ? c : d
  a + (b ? c : d)  // 想表达这个意思，应该给三目运算符加括号
  ```

+ 使用 github package 来推送镜像的时候，需要一个拥有推送权限的 token：

  ```bash
  # 这一步要输密码，不是 github 账号的密码，而是拥有推送权限的 token
  docker login docker.pkg.github.com --username Gusabary
  docker tag fc:v4 docker.pkg.github.com/gusabary/flowchar/flowchar:v1.0
  docker push docker.pkg.github.com/gusabary/flowchar/flowchar:v1.0
  ```

  可以去设置中创建一个新的 token，勾选上推送权限。

##### Last-modified date: 2020.2.5, 1 p.m.