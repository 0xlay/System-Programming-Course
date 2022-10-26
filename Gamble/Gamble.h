#pragma once
#include <Windows.h>
#include <string>
#include <random>
#include <unordered_map>


class Gamble final
{
    static constexpr int kRandomBegin = 1;
    static constexpr int kRandomEnd   = 100;

public:
    Gamble();
    ~Gamble() noexcept;

    Gamble(const Gamble&) = delete;
    Gamble& operator=(const Gamble&) = delete;

    Gamble(Gamble&& rhs) noexcept;
    Gamble& operator=(Gamble&& rhs) noexcept;

    void GenerateNewNumber();
    void MakePrediction(std::string_view userName, int prediction);

    void showWinner();

private:
    std::unordered_map<std::string, int> m_predictions;
    int                                  m_randomValue{};
    CRITICAL_SECTION                     m_section{};
};
