#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include "struct.h"

using namespace std;

void convolution1(const string& in_file){
    ifstream ifs(in_file, ios::binary);
    ofstream ofs("out.ppm", ios::binary);
    if(!ifs || !ofs){
        if(!ifs){
            ifs.close();
        }
        cout << "file open error" << endl;
        return ;
    }

    
    char magic[3];
    ifs.read((char*)&magic, 3);
    ofs.write((char*)&magic, 3);
    if(!ifs){
        ifs.close();
        ofs.close();
        cout << "magic read error" << endl;
        return;
    }

    if(ofs.bad()){
        ifs.close();
        ofs.close();
        cout << "magic write error" << endl;
        return;
    }

    if(magic[0] != 'P' || magic[1] != '6'){
        ifs.close();
        ofs.close();
        cout << "magic number error" << endl;
        return;
    }

    PPMFileHeader file_header;

    string line;
    uint16_t max;
    while(ifs.peek() == '#'){
        getline(ifs, line);
        ofs.write(line.c_str(), line.size());
    }
    if(!(ifs >> file_header.width >> file_header.height >> max)){
        ifs.close();
        ofs.close();
        cout << "header read error" << endl;
        return;
    }

   

    if(max != 255){
        ifs.close();
        ofs.close();
        cout << "max value error" << endl;
        return;
    }
    ifs.get();// one caracter '\n'

    ofs << file_header.width << "\n" << file_header.height << "\n" << max << "\n";


    int pixcel_color_count = file_header.width * file_header.height* 3;

     std::streampos storedPosition = ifs.tellg();
    
    vector<vector<uint8_t>> col_adjecent(file_header.height, vector<uint8_t>(file_header.width*3));
    vector<vector<uint8_t>> row_adjecent(file_header.width, vector<uint8_t>(file_header.height*3));
    for(int i = 0; i < file_header.height; i++){
        col_adjecent[i].resize(file_header.width*3);
        ifs.read((char*) &col_adjecent[i][0], file_header.width*3);
    }

    for(int i = 0; i < file_header.height; i++){
        for(int j = 0; j < file_header.width; j++){
            for(int c = 0; c < 3; c++){
                row_adjecent[j][i*3+c] = col_adjecent[i][j*3 + c];
            }
        }
    }


    uint8_t* piccel = new uint8_t[pixcel_color_count];
    // int16_t* piccel2 = new int16_t[pixcel_color_count];
    uint8_t* res = new uint8_t[pixcel_color_count];
    ifs.seekg(storedPosition);
    ifs.read((char*)piccel, pixcel_color_count* sizeof(uint8_t));
    if(!ifs){
        ifs.close();
        ofs.close();
        cout << "piccel read error" << endl;
        return;
    }
    // multiply by 5
     for(int i = 1; i < file_header.height-1; i++){
         for(int j = 1; j < file_header.width-1; j++){
             for(int c = 0; c < 3; c++){
                int16_t tmp = 0;
                for(int row = -1; row <= 1; row++){
                    for(int col = -1; col <= 1; col++){
                        if(row != 0 && col != 0){
                            continue;
                        }
                        uint mult = (row == 0 && col == 0) ? 5 : -1;
                        tmp += mult * (int16_t)col_adjecent[i+row][(j+col)*3 + c];
                    }
                }
                if(tmp > 255) tmp = 255;
                else if(tmp < 0) tmp = 0;

                res[(j + file_header.width* i)*3 + c] = tmp;
            }
        }
    }
    // first row
    for(int i = 0; i < file_header.height; i++){
        for(int c = 0; c< 3; c++){
            int index = (0 + file_header.width* i)*3 + c;
            res[index] = row_adjecent[0][i*3 + c];
        }
    }
    // last row
    for(int i = 0; i < file_header.height; i++){
        for(int c = 0; c< 3; c++){
            int index = ((file_header.width-1) + file_header.width* i)*3 + c;
            res[index] = row_adjecent[(file_header.width-1)][i*3 + c];
        }
    }
    // first col
    for(int j = 0; j < file_header.width; j++){
        for(int c = 0; c< 3; c++){
            int index = (j + file_header.width* 0)*3 + c;
            res[index] = col_adjecent[0][j*3 + c];
        }
    }
    // last col
    for(int j = 0; j < file_header.width; j++){
        for(int c = 0; c< 3; c++){
            int index = (j + file_header.width* (file_header.height-1))*3 + c;
            res[index] = col_adjecent[(file_header.height-1)][j*3 + c];
        }
    }
    

    int counts[5] = {0, 0, 0, 0, 0};

    for(int i = 0; i < pixcel_color_count/3; i++){

        double color = 0.2126*res[i*3] + 0.7152*res[i*3+1] + 0.0722*res[i*3+2];

        int roundedColor = static_cast<int>(std::round(color));

        if(roundedColor <= 50){
            counts[0]++;
        }
        else if(roundedColor <= 101){
            counts[1]++;
        }
        else if(roundedColor <= 152){
            counts[2]++;
        }
        else if(roundedColor <= 203){
            counts[3]++;
        }
        else{
            counts[4]++;
        }
    }
    ofstream text("out.txt");
    text << counts[0] << " " << counts[1] << " " << counts[2] << " " << counts[3] << " " << counts[4];
   
    text.close();

    ofs.write((char*)res, pixcel_color_count* sizeof(uint8_t));
    delete[] piccel;
    // delete[] piccel2;
    delete[] res;
    ifs.close();
    ofs.close();

}