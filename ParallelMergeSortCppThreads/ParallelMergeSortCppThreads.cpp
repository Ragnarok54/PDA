#include <iostream> 
#include <fstream> 
#include <string> 
#include <algorithm> 
#include <vector> 
#include <list> 
#include <thread> 

int thread_count = 64;

using namespace std;
using Iterator = vector<int>::iterator;

void merge_sort(Iterator left, Iterator right) {
    if (right > left + 1) {
        auto middle = left + ((right - left) / 2);

        if (thread_count > 1) {
            thread_count--;

            thread th(merge_sort, left, middle);
            th.join();

            merge_sort(middle, right);
            thread_count++;
        }
        else {
            merge_sort(left, middle);
            merge_sort(middle, right);
        }

        vector<int> temp(right - left);
        merge(left, middle, middle, right, begin(temp));
        copy(begin(temp), end(temp), left);
    }
}

int main()
{
    list<int> dataInputs = { 100, 1000, 10000, 50000, 100000, 300000, 600000, 1000000 };

    for (auto testSize : dataInputs) {
        clock_t start;
        clock_t finish;

        // Define the files used for the tests
        auto inputFileName = "..\\Input Data\\data_input_" + to_string(testSize) + ".txt";
        auto outputFileName = "..\\Output Data\\Parallel implementation C++ Threads\\data_output_" + to_string(testSize) + ".txt";

        ifstream fin(inputFileName);
        ofstream fout(outputFileName);

        vector<int> vector;
        int number;

        // Read the input data
        for (int index = 0; index < testSize; index++) {
            fin >> number;
            vector.push_back(number);
        }

        // Save the start time
        start = clock();

        merge_sort(begin(vector), end(vector));

        // Save the end time of the test
        finish = clock();

        for (auto val : vector) {
            fout << val << " ";
        }

        // Show the timer
        cout << "\nTest on " + to_string(testSize) + " numbers, took " << ((double)finish - (double)start) / CLOCKS_PER_SEC << " seconds.\n";
    }
}