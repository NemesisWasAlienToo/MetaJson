#include <variant>
#include <algorithm>
#include <string_view>
#include <ctre.hpp>
#include <cmath>
#include <type_traits>
#include <Core/Meta/String.hpp>

namespace Core::Meta
{
    template <std::size_t N = 0>
    struct Variant
    {
        bool HasIndex = false;
        std::size_t Index = 0;
        Core::Meta::String<N> String;

        constexpr Variant(std::size_t i) : HasIndex(true), Index(i) {}
        constexpr Variant(char const (&str)[N + 1]) : String(str) {}

        constexpr Core::Meta::String<N> GetKey() const
        {
            return String;
        }

        constexpr std::size_t GetIndex() const
        {
            return Index;
        }
    };

    template <std::size_t N>
    Variant(char const (&)[N]) -> Variant<N - 1>;

    Variant(std::size_t) -> Variant<0>;

    template <Core::Meta::String TJson>
    struct Skip
    {
        static constexpr auto FirstNoneWhiteSpace()
        {
            std::size_t Index = 0;

            while (Index < TJson.Length() && (TJson[Index] == ' ' || TJson[Index] == '\n'))
                Index++;

            return Index;
        }

        static constexpr auto Value = TJson.template substr<FirstNoneWhiteSpace()>();
    };

    template <Core::Meta::String TJson>
    struct Json;

    constexpr double ParseDouble(std::string_view sv)
    {
        char const *str = sv.begin();
        double result = 0.0;
        int sign = 1;
        int exp = 0;
        int num_digits = 0;
        [[maybe_unused]] bool decimal_found = false;

        // check sign
        if (*str == '-')
        {
            sign = -1;
            str++;
        }
        else if (*str == '+')
        {
            sign = 1;
            str++;
        }

        // read integer part
        while (*str >= '0' && *str <= '9')
        {
            result = result * 10.0 + (*str - '0');
            str++;
            num_digits++;
        }

        // read decimal part
        if (*str == '.')
        {
            decimal_found = true;
            str++;

            while (str != sv.end() && *str >= '0' && *str <= '9')
            {
                result = result * 10.0 + (*str - '0');
                str++;
                num_digits++;
                exp--;
            }
        }

        // apply exponent
        if (exp != 0)
        {
            auto power = [](double base, int exponent) -> double
            {
                double res = 1.0;
                if (exponent == 0)
                {
                    return res;
                }
                else if (exponent > 0)
                {
                    for (int i = 0; i < exponent; i++)
                    {
                        res *= base;
                    }
                    return res;
                }
                else
                {
                    for (int i = 0; i > exponent; i--)
                    {
                        res *= 1 / base;
                    }
                    return res;
                }
            };

            // result *= pow(10, exp);
            result *= power(10, exp);
        }

        // apply sign
        result *= sign;

        // return result
        return result;
    }

    constexpr int64_t ParseInt(std::string_view sv)
    {
        char const *str = sv.begin();
        int64_t result = 0;
        int64_t sign = 1;

        // check sign
        if (*str == '-')
        {
            sign = -1;
            str++;
        }
        else if (*str == '+')
        {
            sign = 1;
            str++;
        }

        // read integer part
        while (str != sv.end() && *str >= '0' && *str <= '9')
        {
            result = result * 10 + (*str - '0');
            str++;
        }

        // apply sign
        result *= sign;

        // return result
        return result;
    }

    template <Core::Meta::String TValue>
    constexpr auto BaseValue()
    {
        if constexpr (Core::Meta::IsEqual<TValue, "null">())
        {
            return nullptr;
        }
        else if constexpr (Core::Meta::IsEqual<TValue, "true">())
        {
            return true;
        }
        else if constexpr (Core::Meta::IsEqual<TValue, "false">())
        {
            return false;
        }

        // @todo Remove the whitespace in the end of the value
        else if constexpr (ctre::match<R"((".+")\s*)">(std::string_view{TValue}))
        {
            auto m = ctre::match<R"((".+")\s*)">(std::string_view{TValue});
            return std::string_view{m.template get<1>()};
        }

        // @todo Remove the whitespace in the end of the value
        else if constexpr (ctre::match<"(?:-|\\+)?[0-9]+\\s*">(std::string_view{TValue}))
        {
            return ParseInt(std::string_view(TValue));
        }

        // @todo Remove the whitespace in the end of the value
        else if constexpr (ctre::match<"(?:-|\\+)?[0-9]*\\.[0-9]+\\s*">(std::string_view{TValue}))
        {
            return ParseDouble(std::string_view(TValue));
        }
        else
        {
            throw std::invalid_argument("Invalid value");
        }
    }

    template <Core::Meta::String TJson, char Stop = '}'>
    struct OneValue
    {
        static constexpr auto Indicies = []
        {
            std::size_t Index = 0;

            while (Index < TJson.Length() && TJson[Index] != ',' && TJson[Index] != Stop)
            {
                // if (sv[Index] != ' ')
                //     EndIndex = Index;

                Index++;
            }

            struct tmp
            {
                std::size_t EndIndex;
                std::size_t Index;
            };

            return tmp{Index, Index + 1};
        }();

        static constexpr auto Value = BaseValue<TJson.template substr<0, Indicies.EndIndex>()>();
        static constexpr auto Rest = TJson.template substr<Indicies.Index>();
    };

    template <Core::Meta::String TJson, char Stop>
        requires(TJson[0] == '"')
    struct OneValue<TJson, Stop>
    {
        static constexpr auto Indicies = []
        {
            static_assert(TJson.Length());

            std::size_t Index = 1;
            [[maybe_unused]] std::size_t EndIndex = 0;

            while (Index < TJson.Length() && ((TJson[Index] != ',' && TJson[Index] != Stop) || !EndIndex))
            {
                if (TJson[Index] == '"')
                    EndIndex = Index - 1;

                Index++;
            };

            // if (Index >= sv.length())
            //     return "";

            struct tmp
            {
                std::string_view Result;
                std::size_t Index;
            };

            return tmp{std::string_view{TJson}.substr(1, EndIndex), Index + 1};
        }();

        static constexpr auto Value = Indicies.Result;
        static constexpr auto Rest = TJson.template substr<Indicies.Index>();
    };

    template <Core::Meta::String TJson>
    struct ListValue
    {
        using TValue = OneValue<Skip<TJson>::Value, ']'>;
        static constexpr auto Value = TValue::Value;

        using Next = ListValue<TValue::Rest>;

        template <Variant TKey, Variant... TRest>
        static constexpr auto Get()
        {
            static_assert(TKey.HasIndex);

            if constexpr (!TKey.GetIndex())
            {
                if constexpr (sizeof...(TRest))
                    return Value.template Get<TRest...>();
                else
                    return Value;
            }
            else
            {
                return Next::template Get<TKey.GetIndex() - 1, TRest...>();
            }
        }

        template <Variant TKey, Variant... TRest>
        static constexpr auto GetOr(auto Default)
        {
            static_assert(TKey.HasIndex);

            if constexpr (!TKey.GetIndex())
            {
                if constexpr (sizeof...(TRest))
                    return Value.template GetOr<TRest...>(std::move(Default));
                else
                    return Value;
            }
            else
            {
                return Next::template GetOr<TKey.GetIndex() - 1, TRest...>(std::move(Default));
            }
        }
    };

    template <>
    struct ListValue<"">
    {
        template <Variant TKey, Variant... TRest>
        static constexpr auto Get()
        {
            throw std::out_of_range("This index does not exist");
        }

        template <Variant TKey, Variant... TRest>
        static constexpr auto GetOr(auto Default)
        {
            return Default;
        }
    };

    template <Core::Meta::String TJson, char Stop>
        requires(TJson[0] == '[')
    struct OneValue<TJson, Stop>
    {
        static constexpr auto Indicies = []
        {
            std::size_t Count = 1;
            std::size_t Index = 1;
            std::size_t EndIndex = 0;

            while (Index < TJson.Length() && (Count | (TJson[Index] != ',' && TJson[Index] != Stop)))
            {
                if (TJson[Index] == '[')
                {
                    Count++;
                }
                else if (TJson[Index] == ']')
                {
                    Count--;
                    EndIndex = Index;
                }

                Index++;
            }

            // if (Index > TJson.Length())
            //     return "";

            // return tmp{std::string_view{TJson}.substr(1, EndIndex), Index + 1};

            struct tmp
            {
                std::size_t EndIndex;
                std::size_t Index;
            };

            return tmp{EndIndex, Index + 1};
        }();

        static constexpr auto Value = ListValue<TJson.template substr<1, Indicies.EndIndex>()>{};
        static constexpr auto Rest = TJson.template substr<Indicies.Index>();
    };

    template <Core::Meta::String TJson, char Stop>
        requires(TJson[0] == '{')
    struct OneValue<TJson, Stop>
    {
        static constexpr auto Indicies = []
        {
            std::size_t Count = 1;
            std::size_t Index = 1;
            std::size_t EndIndex = 0;

            while (Index < TJson.Length() && (Count | (TJson[Index] != ',' && TJson[Index] != Stop)))
            {
                if (TJson[Index] == '{')
                {
                    Count++;
                }
                else if (TJson[Index] == '}')
                {
                    Count--;
                    EndIndex = Index;
                }

                Index++;
            }

            // if (Index > TJson.Length())
            //     return "";

            // return tmp{std::string_view{TJson}.substr(1, EndIndex), Index + 1};

            struct tmp
            {
                std::size_t EndIndex;
                std::size_t Index;
            };

            return tmp{EndIndex, Index + 1};
        }();

        static constexpr auto Value = Json<TJson.template substr<0, Indicies.EndIndex>()>{};
        static constexpr auto Rest = TJson.template substr<Indicies.Index>();
    };

    template <Core::Meta::String TJson>
    struct OneKey
    {

        static constexpr auto Indicies = []
        {
            static_assert(TJson.Length() && TJson[0] == '"');

            std::size_t Index = 1;
            [[maybe_unused]] std::size_t EndIndex = 0;

            while (Index < TJson.Length() && TJson[Index] != ':')
            {
                if (TJson[Index] == '"')
                    EndIndex = Index - 1;

                Index++;
            }

            // if (Index >= TJson.Length())
            //     return static_cast<std::size_t>(0);

            // return std::make_tuple(EndIndex, Index + 1);

            struct tmp
            {
                std::size_t EndIndex;
                std::size_t Index;
            };

            return tmp{EndIndex, Index + 1};
        }();

        static constexpr auto Value = TJson.template substr<1, Indicies.EndIndex>();
        static constexpr auto Rest = TJson.template substr<Indicies.Index>();
    };

    template <Core::Meta::String TJson>
    struct Pair
    {
        using TKey = OneKey<Skip<TJson>::Value>;
        static constexpr auto Key = OneKey<Skip<TJson>::Value>::Value;

        using TValue = OneValue<Skip<TKey::Rest>::Value>;
        static constexpr auto Value = TValue::Value;
        static constexpr auto Rest = TValue::Rest;
    };

    template <Core::Meta::String TJson> // requires(TJson[0] == '{' && TJson[TJson.Length() - 1] == '}')
    struct Json
    {
        using Head = Pair<TJson.template substr<1>()>;
        static constexpr auto Rest = Head::Rest;

        using Next = Json<Rest>;

        template <Variant TKey, Variant... TRest>
        static constexpr auto Get()
        {
            static_assert(!TKey.HasIndex);

            if constexpr (Core::Meta::IsEqual<TKey.GetKey(), Head::Key>())
                if constexpr (sizeof...(TRest))
                    return Head::Value.template Get<TRest...>();
                else
                    return Head::Value;
            else
                return Next::template Get<TKey, TRest...>();
        }

        template <Variant TKey, Variant... TRest>
        static constexpr auto GetOr(auto Default)
        {
            static_assert(!TKey.HasIndex);

            if constexpr (Core::Meta::IsEqual<TKey.GetKey(), Head::Key>())
                if constexpr (sizeof...(TRest))
                    return Head::Value.template GetOr<TRest...>(std::move(Default));
                else
                    return Head::Value;
            else
                return Next::template GetOr<TKey, TRest...>(std::move(Default));
        }
    };

    template <>
    struct Json<"">
    {
        template <auto... TRest>
        static constexpr auto Get()
        {
            throw std::out_of_range("This key does not exist");
        }

        template <auto... TRest>
        static constexpr auto GetOr(auto Default)
        {
            return Default;
        }
    };
}

#define JSON(...)                    \
    ::Core::Meta::Json<#__VA_ARGS__> \
    {                                \
    }
