#pragma once

#include <string_view>
#include <algorithm>

namespace Core::Meta
{
    template <std::size_t N>
    struct String;

    template <>
    struct String<0>
    {
        constexpr String() = default;
        constexpr String(String const &) = default;
        constexpr String(char const (&)[1]) {}
        constexpr size_t Length() const { return 0; }
        constexpr operator std::string_view() const
        {
            return std::string_view{};
        }

        template <std::size_t Start, std::size_t TSize = static_cast<std::size_t>(-1)>
        constexpr auto substr() const
        {
            return String<0>("");
        }
    };

    template <std::size_t N>
    struct String
    {
        char data[N];

        constexpr String() = default;
        constexpr String(String const &Other) { std::copy_n(Other.data, N, data); }
        constexpr String(char const (&str)[N + 1]) { std::copy_n(str, N, data); }
        constexpr size_t Length() const { return N; }
        constexpr char const &operator[](std::size_t Index) const { return data[Index]; }
        constexpr operator std::string_view() const
        {
            return std::string_view{data, &data[N]};
        }

        template <std::size_t Start, std::size_t TSize = static_cast<std::size_t>(-1)>
        constexpr auto substr() const
        {
            // static_assert(Start < N);

            if constexpr (Start < N)
            {
                constexpr auto M = std::min(TSize, N - Start);

                String<M> res;

                std::copy_n(&data[Start], M, res.data);

                return res;
            }
            else
            {
                return String<0>("");
            }
        }
    };

    template<std::size_t N, std::size_t M>
    constexpr bool operator==(String<N> const& L, String<M> const& R)
    {
        if constexpr (N != M)
            return false;

        for (size_t i = 0; i < M; i++)
            if (L[i] != R[i])
                return false;

        return true;
    }

    template<std::size_t N>
    constexpr bool operator==(String<N> const& L, std::string_view Other)
    {
        if (N != Other.size())
            return false;

        for (size_t i = 0; i < Other.size(); i++)
            if (L[i] != Other[i])
                return false;

        return true;
    }

    template <std::size_t N> String(char const (&)[N]) -> String<N - 1>;
    template <std::size_t N> String(String<N>) -> String<N>;
}