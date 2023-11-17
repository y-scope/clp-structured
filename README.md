# clp-s

clp-s is a tool that can compress JSON logs efficiently with search capability.

## Requirements

+ We have built and tested clp-s on the OSes listed [below](#native-environment).
    + If you have trouble building for another OS, file an issue, and we may be able to help.
+ A compiler that can support C++17 (e.g. gcc-8)

## Building

* To build, we require some source dependencies, packages from package managers, 
  and libraries built from source.

### Source Dependencies

We use both git submodules and third-party source packages. 
To download all, you can run this script:

```shell
tools/scripts/deps-download/download-all.sh
```

This will download:

* [ANTLR v4](https://www.antlr.org/download.html) (v4.11.1)
* [abseil-cpp](https://github.com/abseil/abseil-cpp.git) (20230802.1)
* [date](https://github.com/HowardHinnant/date.git) (v3.0.1)
* [nlohmann_json](https://github.com/nlohmann/json.git) (v3.10.4)
* [simdjson](https://github.com/simdjson/simdjson.git) (v3.1.7)
* [spdlog](https://github.com/gabime/spdlog.git) (v1.10.0)

### Environment

A handful of packages and libraries are required to build clp-s. There are two options to use them:

* Install them on your machine and build clp-s natively
* Build clp-s within a prebuilt docker container that contains the libraries;
  However, this won't work if you need additional libraries that aren't already in the container.

#### Native Environment

See the relevant README for your OS:

* [Ubuntu 20.04 or Ubuntu 22.04](./tools/scripts/lib_install/ubuntu-focal-and-jammy/README.md)

### Build

* Configure the cmake project:
  ```shell
  cmake -S . -B build
  ```

* Build:
  ```shell
  cmake --build build -j
  ```

On some machines certain, compiler flags need to be set for the `simdjson` property to prevent it 
from creating binaries for ISAs that the assembler doesn't support. An example disabling Ice Lake 
binaries supporting avx512 can be found commented out in [CMakeLists.txt](CMakeLists.txt).

## Running

### Compression

```bash
./clp-s c archive_dir /home/my/log
```

+ `archive_dir` specifies the output directory of the compressed logs.
+ `/home/my/log` specifies the path of the log file.

You can also specify zstd compression level and the maximum encoding size of an archive. 
The encoding size is the in-memory size of dictionaries plus encoded tables. e.g.

```bash
./clp-s c --max-encoding-size 65536 --compression-level 3 archive_dir /home/my/log
```

You can also specify one column that should be treated as a timestamp. This will affect 
encoding for string timestamps, and will provide a timestamp-range index on that column.

```bash
./clp-s c archive_dir /home/my/log --timestamp-key '@timestamp'
```

### Decompression

```bash
./clp-s x input_dir output_dir
```

+ `archive_dir` specifies the directory of the compressed logs.
+ `output_dir` specifies the output directory for the decompressed logs.

### Search

```bash
./clp-s q archive_dir query
```

+ `archive_dir` specifies the directory containing the clp-s archive.
+ `query` specifies a KQL query to run.

Here are some examples of KQL queries. They support field search and wildcard search.
```
ts > 1649923037 AND ts <= 1649923038
id: 22149
message: error* AND level: ERROR
NOT level: INFO OR message: "unsuccessful operation*"
```
