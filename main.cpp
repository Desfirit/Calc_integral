#include <iostream>
#include <math.h>
#include <random>
#include <time.h>
#include <algorithm>
#include <iterator>
#include <iomanip>

using namespace std;


auto scaler(double min, double max)
{
    return [=](double val){
        return (max - min) * val + min;
    };
}

template<typename Fs>
auto scaled_point(Fs scaler1, Fs scaler2)
{
    return [=](double a, double b){
        return pair{scaler1(a), scaler2(b)};
    };
}

template <typename Func>
double calc_integral(Func f, double a, double b, long long ExpNmb)
{
    double xmin = a;
    double xmax = b;
    double ymin = 0;
    double ymax = f(b);

    auto scalerX(scaler(xmin,xmax));
    auto scalerY(scaler(ymin, ymax));

    auto point_maker(scaled_point(scalerX,scalerY));

    mt19937 g(time(0));
    uniform_real_distribution<> gen;

    auto is_under_integral([&](auto point){
        auto [x,y] = point;
        return f(x) > y;
    });


    long long under_integral = 0;

    for(int i = 0; i < ExpNmb; ++i)
    {
        if(is_under_integral(point_maker(gen(g),gen(g))))
            ++under_integral;
    }

    return (double)under_integral /ExpNmb * (b-a) * f(b);
}

template<typename Model>
class Tester
{
    Model model;
public:
    Tester(Model model): model{model} {}

    double make_test(const long long selection)
    {
        return model(selection);
    }

    vector<double> make_tests(const vector<long long>& inputs)
    {
        vector<double> res;
        res.reserve(inputs.size());

        transform(begin(inputs), end(inputs), back_inserter(res), [&](auto val){
            return model(val);
        });
        return res;
    }
};

double calc_eps(const vector<double>& seria, const double abs_val)
{
    return accumulate(begin(seria),end(seria),(double)0, [abs_val](double sum, double val){
        return sum + abs((val - abs_val) / abs_val);
    });
}

auto calc_serias_eps(const vector<vector<double>>& serias, vector<double>& output, const double abs_val)
{
    return transform(begin(serias),end(serias), begin(output), [abs_val](auto& seria){
        return calc_eps(seria, abs_val);
    });
}

void print_eps(vector<double>& eps)
{
    copy(begin(eps), end(eps), ostream_iterator<double>(cout, "\n"));
    cout << "---------------------------" << '\n';
}

void print_serias(vector<vector<double>>& serias)
{
    cout << "---------------------------" << '\n';
    for(int i = 0; i < serias.size(); ++i)
    {
        cout << "Seria" << i << ":\n";
        copy(begin(serias[i]), end(serias[i]), ostream_iterator<double>(cout, "\n"));
        cout << "---------------------------" << '\n';
    }
}

int main()
{
    const double res = 6;
    const vector<long long> selections{10'000,100'000, 1'000'000, 10'000'000};
    const long long serias_num (3);

    auto func([](double x){
        return pow(x,3) + 1;
    });

    Tester tester([func](auto val){
        return calc_integral(func, 0, 2, val);
    });

    vector<vector<double>> serias(serias_num);
    transform(serias.begin(),serias.end(),serias.begin(), [&tester,selections](auto& c){
        return vector<double>(tester.make_tests(selections));
    });

    cout << fixed << setprecision(10);

    print_serias(serias);

    vector<double> eps(serias_num);
    calc_serias_eps(serias,eps,res);

    cout << "Eps: " << '\n';
    print_eps(eps);

    return 0;
}
