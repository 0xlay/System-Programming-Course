#include <iostream>
#include <format>
#include <ranges>
#include "Gamble.h"
#undef max


Gamble::Gamble()
{
    InitializeCriticalSection(&m_section);
    GenerateNewNumber();
}


Gamble::~Gamble() noexcept
{
    DeleteCriticalSection(&m_section);
}


Gamble::Gamble(Gamble&& rhs) noexcept
{
    std::swap(m_predictions, rhs.m_predictions);
    std::swap(m_section, rhs.m_section);
    std::swap(m_randomValue, rhs.m_randomValue);
}


Gamble& Gamble::operator=(Gamble&& rhs) noexcept
{
    if (this != &rhs)
    {
        std::swap(m_predictions, rhs.m_predictions);
        std::swap(m_section, rhs.m_section);
        std::swap(m_randomValue, rhs.m_randomValue);
    }
    return *this;
}


void Gamble::GenerateNewNumber()
{
    EnterCriticalSection(&m_section);

    std::mt19937_64 re;
    std::uniform_int_distribution<int> uniform_distribution(kRandomBegin, kRandomEnd);
    m_randomValue = uniform_distribution(re);

    LeaveCriticalSection(&m_section);
}


void Gamble::MakePrediction(std::string_view userName, int prediction)
{
    EnterCriticalSection(&m_section);

    m_predictions[userName.data()] = prediction;

    LeaveCriticalSection(&m_section);
}


//
// Complexity: O(n)
//
void Gamble::showWinner()
{
    int val = 0;
    for (const auto& prediction : m_predictions | std::views::values)
    {
        if (val < prediction && prediction <= m_randomValue)
        {
            val = prediction;
        }
    }

    for (const auto& [name, prediction] : m_predictions)
    {
        if (prediction == val)
        {
            std::cout << "Random value = " << m_randomValue << std::endl;
            std::cout << std::format("Win player: {} - {}!", name, prediction) << std::endl;
            break;
        }
    }
}
