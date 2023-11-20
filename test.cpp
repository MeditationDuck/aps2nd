#include "main.h"
#include <iostream>
#include <iomanip>
#include<openssl/evp.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fstream>
#include<vector>
#include <sstream> 
#include <string>
#include <chrono>
#include "conv1.h"
#include "decrease_for.h"
#include "decrease_for2.h"
#include "decrease_for3.h"
#include "decrease_for4.h"
#include "decrease_for5.h"

using namespace std;

std::string calculateMD5(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::ostringstream oss;

    if (!file) {
        oss << "Cannot open file: " << filename;
        return oss.str();
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        oss << "Failed to create MD5 context";
        return oss.str();
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), nullptr) != 1) {
        oss << "Failed to initialize MD5 digest";
        EVP_MD_CTX_free(mdctx);
        return oss.str();
    }

    std::vector<unsigned char> buffer(1024, 0);
    while (file.good()) {
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        if (EVP_DigestUpdate(mdctx, buffer.data(), file.gcount()) != 1) {
            oss << "Failed to update MD5 digest";
            EVP_MD_CTX_free(mdctx);
            return oss.str();
        }
    }

    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, md_value, &md_len) != 1) {
        oss << "Failed to finalize MD5 digest";
        EVP_MD_CTX_free(mdctx);
        return oss.str();
    }

    EVP_MD_CTX_free(mdctx);

    for (unsigned int i = 0; i < md_len; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)md_value[i];
    }

    return oss.str();
}




bool testfile(const string& filename, string expected_contents){
    ifstream ifs(filename);
    if(!ifs){
        cout << "file open error" << endl;
        return false;
    }
    string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    if(contents != expected_contents){
        cout << "file contents error" << endl;
        return false;
    }
    return true;
}

int main(){


    // convolution("test-10x8/test.ppm");

    // see_file("out.ppm");

    auto start = std::chrono::high_resolution_clock::now();
    convolution("vit_small.ppm");
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Convolution took " << duration.count() << " ms" << std::endl;
   

    start = std::chrono::high_resolution_clock::now();
    convolution1("vit_small.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Convolution took " << duration.count() << " ms" << std::endl;

  

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for("vit_small.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Convolution took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for2("vit_small.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for2 took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for3("vit_small.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for3 took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for4("vit_small.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for4 took " << duration.count() << " ms" << std::endl;


    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for5("vit_small.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for5 took " << duration.count() << " ms" << std::endl;



    bool output_result = true;

    string hash = calculateMD5("out.ppm");
    if(hash == "32554ccd9b09af5b660a17b05350959b"){
        // cout << "Hash is correct!!!!!!!!!" << endl;
    }
    else{
        cout << "Hash is incorrect!!!!!!!" << endl;
        output_result = false;
    }
    if(testfile("out.txt", "24432 16307 15192 54597 9472")){
        // cout << "File is correct!!!!!!!!!" << endl;
    }
    else{
        cout << "Text is incorrect!!!!!!!" << endl;
        output_result = false;
    }




    cout << "BIGFILE" << endl << endl;


    // start = std::chrono::high_resolution_clock::now();
    // convolution("vit_normal.ppm");
    // stop = std::chrono::high_resolution_clock::now();

    // duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    // std::cout << "Convolution took " << duration.count() << " ms" << std::endl;
   

    // start = std::chrono::high_resolution_clock::now();
    // convolution1("vit_normal.ppm");
    // stop = std::chrono::high_resolution_clock::now();

    // duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    // std::cout << "Convolution took " << duration.count() << " ms" << std::endl;

  

    // start = std::chrono::high_resolution_clock::now();
    // convolution_dec_for("vit_normal.ppm");
    // stop = std::chrono::high_resolution_clock::now();

    // duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    // std::cout << "Convolution took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for2("vit_normal.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for2 took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for3("vit_normal.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for3 took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for4("vit_normal.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for4 took " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    convolution_dec_for5("vit_normal.ppm");
    stop = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "convolution_dec_for5 took " << duration.count() << " ms" << std::endl;
 







    if(output_result){
        // cout << "All tests passed!" << endl;
    }
    else{
        cout << "Some tests failed!" << endl;
        cout << "FAILED FAILED FAILED FAILED" << endl; 
    }


    return 0;
}