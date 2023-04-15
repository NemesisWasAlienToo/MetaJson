#include <iostream>
#include <fmt/core.h>
#include <string_view>
#include <Meta/Json.hpp>

int main(int, char const *[])
{
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

    fmt::print("{}\n", Setting.GetOr<"NotExist">("Default value"));
    fmt::print("{}\n", Setting.GetOr<"list", 100>("Default list"));
    fmt::print("{}\n", std::is_same_v<decltype(Setting.Get<"Nullable">()), std::nullptr_t>);
    fmt::print("Boolean = {}\n", Setting.Get<"Boolean">());
    fmt::print("{}\n", Setting.Get<"list", 5, "in">());
    fmt::print("{}\n", Setting.Get<"map", "map1", "list2", 1, "in">());

    fmt::print("{}\n", Setting.Get<"Key">());
    
    constexpr auto valueWithDefault = Setting.GetOr<"NotExist">("Default value");

    constexpr auto list = Setting.Get<"list">();

    fmt::print("{}\n", list.Get<0>());
    fmt::print("{}\n", list.Get<1>());
    fmt::print("{}\n", list.Get<2>());
    fmt::print("{}\n", list.Get<3>());
    fmt::print("{}\n", list.Get<4, 0>());

    fmt::print("list in list : {}\n", Setting.Get<"list", 4, 0>());

    constexpr auto map = Setting.Get<"map">();

    fmt::print("{}\n", map.Get<"inner">());

    constexpr auto list1 = map.Get<"list1">();

    fmt::print("{}\n", list1.Get<2>());

    return 0;
}