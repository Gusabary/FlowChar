# FlowChar

![](https://img.shields.io/badge/version-v1.0-9cf)

一个用来从伪代码生成纯字符格式流程图的小工具。

## 背景

前几天学习红黑树，插入和删除的操作过于复杂，心想画个小流程图好让笔记看上去更直观些，又懒得用画图工具，于是就一个字符一个字符地敲，敲完发现其实也没比用画图工具轻松多少。后来我就想搞一个小工具，只要输入一些很简单的伪代码就能生成一张纯字符格式的流程图，于是就有了这个仓库。

## 示例

伪代码：

```
be born;
while (alive) {
    if (happy) {
        smile;
    }
    else {
        try to be happy;
    }
}
die;
```

流程图：

```
              +-------------+                   
              |   be born   |                   
              +-------------+                   
                     |                          
                     V                          
             N /-----------\                    
+--------------|   alive   |<------------------+
|              \-----------/                   |
|                    | Y                       |
|                    V                         |
|            Y /-----------\ N                 |
|         +----|   happy   |----+              |
|         |    \-----------/    |              |
|         |                     |              |
|         V                     V              |
|   +-----------+    +---------------------+   |
|   |   smile   |    |   try to be happy   |   |
|   +-----------+    +---------------------+   |
|         |                     |              |
|         +--------->O<---------+              |
|                    |                         |
|                    V                         |
|                    O-------------------------+
|                                               
|                                               
|               +---------+                     
+-------------->|   die   |                     
                +---------+                     
```

[*更多示例*](./demo.md)

## 如何使用

首先克隆仓库并进行构建：

```bash
git clone https://github.com/Gusabary/FlowChar.git
cd FlowChar
chmod +x ./build.sh
./build.sh
```

然后指定伪代码文件，可选地，可以指定流程图输出文件，缺省的话会打印到标准输出：

```bash
./flowchar -c ../examples/simple -o ../examples/simple-chart
```

使用 `-h` 选项以打印帮助信息：

```bash
./flowchar -h
```

### Docker

或者直接拉取 Docker 镜像，挂载并运行：

```bash
# from dockerhub
docker pull gusabary/flowchar:v1.0

# or from github
docker pull docker.pkg.github.com/gusabary/flowchar/flowchar:v1.0

# run
docker run -v /path/to/dir:/app/files gusabary/flowchar:v1.0 -c files/code -o files/chart
```

需要将伪代码文件所在目录挂载为容器的 `/app/files` 目录。

从 github docker registry 拉取镜像的话可能会需要 token，可以参考[这里](<https://help.github.com/en/github/managing-packages-with-github-packages/configuring-docker-for-use-with-github-packages>)。

### 伪代码语法

目前仅支持顺序结构，`if-else` 控制的选择结构以及 `while` 控制的循环结构。

+ 一串以分号结尾的字符串是一个语句，会出现在流程图的一个方框中：

  ```
  a;
  do this;
  ```

+ `if` 关键字后要求跟一对圆括号，其中为 `if` 的判断条件，然后是由一对花括号包裹着的语句块：

  ```
  if (condition) {
      statementA;
      statementB;
  }
  ```

  可选地，使用 `else` 关键字，并跟上一对花括号包裹着的语句块：

  ```
  if (condition) {
      ok;
  }
  else {
      no;
  }
  ```

+ `while` 关键字后要求跟一对圆括号，其中为 `while` 的判断条件，然后是由一对花括号包裹着的语句块：

  ```
  while (condition) {
      loop;
  }
  ```

+ 语句块可以是简单语句、`if` 语句和 `while` 语句的组合。

## 使用许可

[MIT](./LICENSE)

##### Last-modified date: 2020.2.5, 3 p.m.