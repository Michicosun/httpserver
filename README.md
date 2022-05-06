# Simple HTTP Server

This is very light HTTP server, written on C++, uses a blocking queue to distribute user requests to threads in thread pool.

# Build
```bash
git clone https://github.com/Michicosun/httpserver
```

```bash
$ cd httpserver
$ rm -rf build
$ mkdir build && cd build
$ cmake .. -G Ninja
$ ninja httpserver
```

The executable file will be generated along the path /build/bin

# Startup

It is enough just to run the executable file, the 2nd argument is the port on which the server will listen for connections

```bash
$ ./bin/httpserver 31337
```

Also you can insert an additional parameter -e, for more informative logs

```bash
$ ./bin/httpserver 31337 -e
```

# Usage

By default server can serve 4 main HTTP requests: GET, POST, PUT, DELETE

- GET - returns the file by path
- POST - rewrites file content with data in body section of POST request
- PUT - appends body section of request to file's data
- DELETE - deletes file










