# Compile time json for C++ (V0.1.1)

This library is a sub part of CoreKit library.

## Dependencies

In order to be able to run the example you need to install:
    - ctre located at https://github.com/hanickadot/compile-time-regular-expressions
    - fmt located at https://github.com/fmtlib/fmt

## Supported compilers

I have only tried it with g++ (11.3.0) and clang++ (14.0.0) so far but it should compile with any version supporting C++ 20 standard features.

## Capabilities

The code bellow will parse and construct a compile time equivalent for the json object passed to it. 

```CPP
constexpr auto Setting = JSON({
        "Key" : 123.2,
        "Nullable" : null,
        "Boolean" : true,
        "hi" : "asdasd",
        "list" : [ 1, -2.2, "3", 4, [ -1, 2, 3 ], {"in" : "val"} ],
        "map" : {
            "inner" : "inner value",
            "list1" : [ 1, 2, "hello", -2.2 ],
            "map1" : {
                "most" : "inner",
                "list2" : [ 0, {"in" : "val"}, "there it is" ]
            }
        }
    });
```

The recognized types for a value are : __std::string_view, nullptr_t, double, int64_t and bool__.

Now to access a value tied to a key you can do:

```cpp
constexpr auto value = Setting.Get<"Key">() // value is of double type;
```
The return type will be detected at compile time. In order to access a key that might not exist, you can do:

```cpp
constexpr auto valueWithDefault = Setting.GetOr<"NotExist">("Default value");
```

The type of default value will be deduced in compile time. Meaning you can also do:

```cpp
constexpr auto valueWithDefault = Setting.GetOr<"NotExist">(12.2);
// or
constexpr auto valueWithDefault = Setting.GetOr<"NotExist">(nullptr);
// or ...
```

To access successive keys and indices you can list them one after another:

```cpp
constexpr auto item = Setting.Get<"list", 5, "in">();
```

Or alternatively to access only an index of a list you can do:

```cpp
constexpr auto list = Setting.Get<"list">();
constexpr auto itemInList = list.Get<0>();
```
Or to access a possibly out of bound index of a list you can do:

```cpp
constexpr auto list = Setting.Get<"list">();
constexpr auto itemInList = list.Get<10>();

//or 

constexpr auto itemInList = Setting.Get<"list", 5>();
```

It is very important to note that any access to a none-existing key or an out of bound index will cause compile time error. If you are accessing a
key or and index which might not exist, just use __GetOr__ instead of __Get__.

## Installation

To install the library you can just run the following commands:

```sh
mkdir build && cd build
cmake ..
sudo make install
```

## Usage

This library is a sub part of CoreKit library. The Cmake package is basically the namespace names following each other in pascal case: CoreMetaJson The target is the namespace names with :: between them: Core::Meta::Json

This is how you can link against this library in your cmake file:

```cmake
find_package(CoreMetaJson)

target_link_libraries(YOUR_TARGET Core::Meta::Json)
```

## Usage example

The usage example can be found in Sample/Json.cpp

To compile the example, install fmt and ctre libraries mentioned on the top, and the just do:

```sh
mkdir build && cd build
cmake ../Sample
make

# To execute the example do

./MetaJson
```

# Support my work here!
https://www.buymeacoffee.com/NemesisIsAlien