#pragma once
#include <random>

namespace app
{
	void random_seed(std::default_random_engine& dre);

    //random value
    template <typename T, T MinVal = std::numeric_limits<T>::min(), T MaxVal = std::numeric_limits<T>::max()>
    class random_value
    {
    public:
        random_value() : Uid(MinVal, MaxVal) { random_seed(Dre); }
        random_value(const T& min_val, const T& max_val) : Uid(min_val, max_val) { random_seed(Dre); }

        T operator()() { return Uid(Dre); }
        T get() { return Uid(Dre); }
        void get(T& val) { val = Uid(Dre); }

        //generate range
        template <typename TI> void get(TI first, TI last)
        {
            std::generate(first, last, [&]() { return get(); });
        }

    private:
        std::default_random_engine Dre;
        std::uniform_int_distribution<T> Uid;
    };
}
