///////////////////////////////////////////////////////////////////////////////
// Mark Kim 
// September 11th, 2023
// Input: a file named input.txt
// Output: A subset of homes in the input.txt that maximizes the amount of candy
// without going over the limit set by the parents.
// If there isn't a home/set of homes that can be found, print "Don't go here"
// Compile instructions: g++ --std=c++20 main.cpp
// std::views, which requires C++20.
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <ranges>

std::vector<int> split(std::string &str)
{
    auto const re = std::regex{R"(\n)"};
    auto const vec = std::vector<std::string>(
        std::sregex_token_iterator{begin(str), end(str), re, -1},
        std::sregex_token_iterator{}
    );

    std::vector<int> out(vec.size());
    for (int i=0; i<out.size(); i++){
        out[i] = std::stoi(vec[i]);
    }
    

    return out;
}

int main(int argv, char **argc)
{
    auto path = std::filesystem::path("input.txt");
    std::ifstream file(path);
	if (!file.is_open()) {
        std::cerr << "Could not open input.txt" << std::endl;
		return 0;
	}

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	std::string s_input(size, ' ');
	file.seekg(0);
	file.read(s_input.data(), size);
    file.close();
    auto input = split(s_input);

    const int max_homes = input[0];
    const int num_candy = input[1];

    auto candy_count = input | std::views::drop(2);
    bool exact_count = false;
    int home_count = 10001;
    int home_idx = -1;
    int home_sum = 0;
    int cnt = 0;
    int sum = 0;
    int prev_sum = -1;

    for (int i=0; i<candy_count.size(); i++){
        sum = 0;
        for (int j=i; j<candy_count.size(); j++){
            sum += candy_count[j];
            if (sum == num_candy){
                if (!exact_count){
                    exact_count = true;
                    home_count = 10001;
                    home_idx = -1;
                    home_sum = 0;
                }
                cnt++;
                if (j - i < home_count){
                    exact_count = true;
                    home_count = j - (i);
                    home_idx = i;
                    home_sum = sum;
                }
                break;
            }
            if (!exact_count && (sum < num_candy) && (sum > prev_sum))
            {
                prev_sum = sum;
                home_count = j - (i);
                home_idx = i;
                home_sum = sum;                
            }
        }
    }
    if (home_idx >= 0){
        std::cout << "Start at home " << home_idx + 1;
        std::cout << " and go to home " << home_idx + 1 + home_count;
        std::cout << " getting " << home_sum << " pieces of candy" << std::endl;
    }
    else
        std::cout << "Don't go here." << std::endl;

    return cnt;
    
}