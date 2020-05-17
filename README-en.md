# FlowChar

![](https://img.shields.io/badge/version-v1.1-9cf)

A tiny tool for conversion from pseudo-code to flowchart in character format.

## Demo

pseudo-code:

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

flowchart:

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

[*more demos*](./demo.md)

## Usage

First clone the repository and build:

```bash
git clone https://github.com/Gusabary/FlowChar.git
cd FlowChar

# for linux
chmod +x ./build.sh
./build.sh

# for windows
.\build.bat
```

Then execute the binary with pseudo-code file and, optionally, output file specified. If no output file is provided, the emitted flowchart would be printed to stdout:

```bash
cd build
./flowchar -c ../examples/simple -o ../examples/simple-chart
```

Use `-h` option to get help:

```bash
./flowchar -h
```

### Docker

Alternatively, pull a docker image and run with the directory including pseudo-file code mounted.

```bash
# from dockerhub
docker pull gusabary/flowchar:v1.0

# or from github
docker pull docker.pkg.github.com/gusabary/flowchar/flowchar:v1.0

# run
docker run -v /path/to/dir:/app/files gusabary/flowchar:v1.0 -c files/code -o files/chart
```

Note: the directory including pseudo-code file should be mounted as `/app/files` directory in the container.

A token may be needed if the image is pulled from github docker registry, [here](https://help.github.com/en/github/managing-packages-with-github-packages/configuring-docker-for-use-with-github-packages) for reference.

### Pseudo-code Syntax

Only sequential structure, branch structure controlled by `if-else` and loop structure controlled by `while` are supported now.

+ A string terminated with a semicolon is a statement, which will appear as a box in flowchart:

  ```
  a;
  do this;
  ```

+ `if` keyword should be followed by condition wrapped in brackets and statement block wrapped in parenthesis:

  ```
  if (condition) {
      statementA;
      statementB;
  }
  ```

  Optionally, `else` keyword can be used with another statement block wrapped in parenthesis:

  ```
  if (condition) {
      ok;
  }
  else {
      no;
  }
  ```

+ `while` keyword should be followed by condition wrapped in brackets and statement block wrapped in parenthesis:

  ```
  while (condition) {
      loop;
  }
  ```

+ A statement block can be a composition of simple statements, `if` statements and `while` statements.

## License

[MIT](./LICENSE)

##### Last-modified date: 2020.5.17, 4 p.m.