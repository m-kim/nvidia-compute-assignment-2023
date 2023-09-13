///////////////////////////////////////////////////////////////////////////////
// Mark Kim 
// September 11th, 2023
// Input: a file named input.txt
// Output: A subset of homes in the input.txt that maximizes the amount of candy
// without going over the limit set by the parents.
// If there isn't a home/set of homes that can be found, print "Don't go here"
// Compile instructions: g++ -fopenmp --std=c++20 main.cpp
// std::views, which requires C++20. 
// 
// Notes:
// This was run on a Razer Blade 2020 with an Intel Core i7-10750h, 32GB of RAM 
// and an RTX-2060 mobile GPU with 6GB of RAM on Windows 11 v22000.2295 
// using Ubuntu 22.04 for WSL2.
// g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <ranges>
#include <numeric>
#include <omp.h>

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

    if (num_candy < 0){
        std::cout << "Don't go here." << std::endl;
        return 0;
    }
    if (max_homes < 0){
        std::cout << "Don't go here." << std::endl;
        return 0;
    }

    auto candy_count = input | std::views::drop(2);

    if (max_homes != candy_count.size()){
        std::cout << "Don't go here." << std::endl;
        return 0;
    }

    std::array<bool,4> exact_count{false};
    std::array<int,4> home_count{10001};
    std::array<int,4> home_idx{1};
    std::array<int,4> home_sum{0};
    std::array<int,4> cnt{0};
    std::array<int,4> sum{0};
    std::array<int,4> prev_sum{-1};
    
    //special case if number of candy is zero
    if (num_candy == 0){
        auto zero_home = std::find(candy_count.begin(), candy_count.end(), 0);
        if (zero_home != std::end(candy_count)){
            auto dist =  distance(candy_count.begin(), zero_home) + 1;
            std::cout << "Start at home " << dist;
            std::cout << " and go to home " << dist;
            std::cout << " getting 0 pieces of candy" << std::endl;
            return 0;
        }
        else{
            std::cout << "Don't go here." << std::endl;
            return 0;
        }

    }

    omp_set_dynamic(0);
    omp_set_num_threads(4);
    #pragma omp parallel for
    for (int i=0; i<candy_count.size(); i++){
        int tidx = omp_get_thread_num();
        int sum = 0;
        for (int j=i; j<candy_count.size(); j++){
            sum += candy_count[j];
            if (sum == num_candy){
                if (!exact_count[tidx]){
                    exact_count[tidx] = true;
                    home_count[tidx] = 10001;
                    home_idx[tidx] = -1;
                    home_sum[tidx] = 0;
                }
                cnt[tidx]++;
                if (j - i < home_count[tidx]){
                    exact_count[tidx] = true;
                    home_count[tidx] = j - (i);
                    home_idx[tidx] = i;
                    home_sum[tidx] = sum;
                }
                break;
            }
            if (!exact_count[tidx] && (sum < num_candy) && (sum > prev_sum[tidx]))
            {
                prev_sum[tidx] = sum;
                home_count[tidx] = j - (i);
                home_idx[tidx] = i;
                home_sum[tidx] = sum;                
            }
        }
    }
    

    
    for (int i=0; i<4; i++){
        std::cout << exact_count[i] << " " << home_idx[i] << " " << home_count[i] << " " << home_sum[i] << std::endl;
    }

    bool single_exact = std::reduce(exact_count.begin(), exact_count.end()) > 0;
    int index = -1;
    // 1. Is there a perfect sum?
    if (single_exact){
        int min_homes = 1e6;
        // 2. Which is the minimal number of homes?
        for (int i=0; i<4; i++){
            if (exact_count[i]){
                if (min_homes > home_count[i]){
                    min_homes = home_count[i];
                    index = i;
                }
            }
        }    
    }
    else{
        // 3. If there isn't a perfect sum,
        int max_sum = 0;
        int min_homes = 1e6;
        for (int i=0; i<4; i++){
            // 4. which is the maximum sum?
            if (max_sum < home_sum[i] && min_homes > home_count[i]){
                max_sum = home_sum[i];
                min_homes = home_count[i];
                index = i;
            }
        }
    }
    
    if (home_idx[index] >= 0 && home_sum[index] > 0){
        std::cout << "Start at home " << home_idx[index] + 1;
        std::cout << " and go to home " << home_idx[index] + 1 + home_count[index];
        std::cout << " getting " << home_sum[index] << " pieces of candy" << std::endl;
    }
    else
        std::cout << "Don't go here." << std::endl;

    return 0;
    
}