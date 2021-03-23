#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>

using namespace std;

vector<int> merge(vector<int> left, vector<int> right){

    vector<int> result;

    while (!left.empty() || !right.empty()) {

        if (!left.empty() && !right.empty()) {
            if (left.front() <= right.front()) {
                result.push_back(left.front());
                left.erase(left.begin());
            }
            else {
                result.push_back(right.front());
                right.erase(right.begin());
            }
        }
        else if (!left.empty()) {
            for (auto val : left) {
                result.push_back(val);
            }

            break;
        }
        else if (!right.empty()) {
            for (auto val : right) {
                result.push_back(val);
            }

            break;
        }
    }

    return result;
}

vector<int> mergeSort(vector<int> initial)
{
    vector<int> left;
    vector<int> right;
    vector<int> result;

    if (initial.size() <= 1)
        return initial;
    
    int middle = (initial.size() + 1) / 2;

    for (int i = 0; i < middle; i++) {
        left.push_back(initial[i]);
    }

    for (int i = middle; i < initial.size(); i++) {
        right.push_back(initial[i]);
    }

    left = mergeSort(left);
    right = mergeSort(right);
    result = merge(left, right);

    return result;
}

int main()
{
    list<int> dataInputs = { 100, 1000, 10000, 50000, 100000, 300000, 600000, 1000000 };

    for (auto testSize : dataInputs) {

        clock_t start;
        clock_t finish;

        // Define the files used for the tests
        auto inputFileName = "..\\Input Data\\data_input_" + to_string(testSize) + ".txt";
        auto outputFileName = "..\\Output Data\\Sequential implementation\\data_output_" + to_string(testSize) + ".txt";
        
        ifstream fin(inputFileName);
        ofstream fout(outputFileName);
        
        vector<int> vector;
        int number;

        // Read the input data
        for (int index = 0; index < testSize; index++) {
            fin >> number;
            vector.push_back(number);
        }

        // Record start time of test
        start = clock();
        
        vector = mergeSort(vector);
        
        // Record end time of test
        finish = clock();
        
        // Store the output
        for (auto val : vector) {
            fout << val << " ";
        }

        cout << "Test on " + to_string(testSize) + " numbers, took " << ((double)finish - (double)start) / CLOCKS_PER_SEC << " seconds.\n";
    }
}