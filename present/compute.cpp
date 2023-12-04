#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>

// Constants
const double HT_L1 = 1.25; // ns
const double HT_L2 = 20.3; // ns
const double HT_RAM = 144; // ns
const double Cost_Min = 2.0;
const double Cost_Max = 9.0;

void remove_commas(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), ','), str.end());
}

double parse_double(const std::string& line, const std::string& start_delimiter, const std::string& end_delimiter = " ") {
    auto start = line.find(start_delimiter);
    if (start == std::string::npos) return 0.0;
    start += start_delimiter.length();
    auto end = line.find(end_delimiter, start);
    std::string number = line.substr(start, end - start);
    remove_commas(number);
    return std::stod(number);
}

// Function to calculate efficiency points
double calculate_efficiency_points(double I_refs, double D_refs, double MR_L1i, double MR_L1d, double MR_L2i, double MR_L2d) {
    // Calculate AMAT for instruction and data cache
    double AMAT_i = HT_L1 + MR_L1i * (HT_L2 + MR_L2i * HT_RAM);
    double AMAT_d = HT_L1 + MR_L1d * (HT_L2 + MR_L2d * HT_RAM);

    // Compute cost
    double cost = AMAT_i * I_refs + AMAT_d * D_refs;
    std::cout << AMAT_i << " " << I_refs  << " " << AMAT_d << " " << D_refs << std::endl;

    // Output the calculated cost for debugging
    cost = cost / 1'000'000'000;
    std::cout << "Calculated Cost: " << cost << std::endl;

    // Calculate efficiency points
    double pts_efficiency = 10 * (Cost_Max - cost) / (Cost_Max - Cost_Min);
    // std::cout << pts_efficiency << std::endl;

    // Truncate to 0 if negative and to 10 if exceeding 10
    // pts_efficiency = std::max(0.0, std::min(10.0, pts_efficiency));

    // Round up to nearest 0.25
    // pts_efficiency = std::ceil(pts_efficiency * 4) / 4;

    return pts_efficiency;
}

int main() {
    std::ifstream file("res.txt");
    std::string line;
    double I_refs = 0, D_refs = 0, I1_misses = 0, D1_misses = 0, LLi_misses = 0, LLd_misses = 0;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("I   refs:") != std::string::npos) {
                I_refs = parse_double(line, "I   refs:      ");
            } else if (line.find("D   refs:") != std::string::npos) {
                D_refs = parse_double(line, "D   refs:        ", "  (");
            } else if (line.find("I1  misses:") != std::string::npos) {
                I1_misses = parse_double(line, "I1  misses:            ");
            } else if (line.find("D1  misses:") != std::string::npos) {
                D1_misses = parse_double(line, "D1  misses:        ");
            } else if (line.find("LLi misses:") != std::string::npos) {
                LLi_misses = parse_double(line, "LLi misses:            ");
            } else if (line.find("LLd misses:") != std::string::npos) {
                LLd_misses = parse_double(line, "LLd misses:        ");
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open file res.txt" << std::endl;
        return 1;
    }

    // Check the parsed data
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "I_refs: " << I_refs << ", D_refs: " << D_refs << std::endl;
    std::cout << "I1_misses: " << I1_misses << ", D1_misses: " << D1_misses << std::endl;
    std::cout << "LLi_misses: " << LLi_misses << ", LLd_misses: " << LLd_misses << std::endl;

    double MR_L1i = I1_misses / I_refs;
    double MR_L1d = D1_misses / D_refs;
    double MR_L2i = LLi_misses / I_refs;
    double MR_L2d = LLd_misses / D_refs;

    // Check the calculated miss rates
    std::cout << "MR_L1i: " << MR_L1i << ", MR_L1d: " << MR_L1d << std::endl;
    std::cout << "MR_L2i: " << MR_L2i << ", MR_L2d: " << MR_L2d << std::endl;

    double pts_efficiency = calculate_efficiency_points(I_refs, D_refs, MR_L1i, MR_L1d, MR_L2i, MR_L2d);

    std::cout << "Efficiency Points: " << pts_efficiency << std::endl;
    std::cout << "Thus " << pts_efficiency + 5 << std::endl;

    return 0;
}
