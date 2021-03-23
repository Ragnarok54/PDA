#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <random>

using namespace std;

int main()
{
    // Will be used to obtain a seed for the random number engine
    std::random_device rd;

    // Standard mersenne_twister_engine seeded with rd()
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000000000);

    list<int> dataInputs = { 100, 1000, 10000, 50000, 100000, 300000, 600000, 1000000};

    for (auto testSize : dataInputs) {

        auto fileName = "..\\Input Data\\data_input_" + to_string(testSize) + ".txt";

        ofstream fout(fileName);

        for (int index = 0; index < testSize; index++) {
            fout << distrib(gen) << " ";
        }
    }
}