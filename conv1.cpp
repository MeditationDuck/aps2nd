#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
// #include "conv1.h"

using namespace std;

struct PPMFileHeader{
    char magic[3];
    int width;
    int height;
    int max;
};

struct PPMHeader{
    int width;
    int height;
};

struct PPMPiccel{
    uint8_t r;
};


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

    ifs.seekg(0, std::ifstream::end);
	long file_size = ifs.tellg();
	ifs.seekg(0,std::ifstream::beg);

    PPMFileHeader file_header;

    ifs.read((char*)&file_header, 3);
    ofs.write((char*)&file_header, 3);
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

    if(file_header.magic[0] != 'P' || file_header.magic[1] != '6'){
        ifs.close();
        ofs.close();
        cout << "magic number error" << endl;
        // cout << file_header.magic[0] << " " << file_header.magic[1] << endl;
        return;
    }

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

    // cout <<"Header:" << endl;
    // cout << "Width: " <<file_header.width << endl;
    // cout << "Height: " << file_header.height << endl;
    // cout << "Max: " << max << endl;

    file_size -= ifs.tellg();

    int pixcel_color_count = file_header.width * file_header.height* 3;
    


    uint8_t* piccel = new uint8_t[pixcel_color_count];
    int16_t* piccel2 = new int16_t[pixcel_color_count];
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
                int index = (j + file_header.width* i)*3 + c;
                int16_t tmp = (int16_t)piccel[index] * (int16_t)5;
                piccel2[(j + file_header.width* i)*3 + c] = tmp;
            }
        }
    }

    // multiply by -1  from above
    for(int i = 1; i < file_header.height-1; i++){
        for(int j = 1; j < file_header.width-1; j++){
            for(int c = 0; c< 3; c++){
                int index = (j + file_header.width* (i-1))*3 + c;
                piccel2[(j + file_header.width* i)*3 + c] -= (int16_t)piccel[index];
            }
        }
    }
    // from below
    for(int i = 1; i < file_header.height-1; i++){
        for(int j = 1; j < file_header.width-1; j++){
            for(int c = 0; c< 3; c++){
                int index = (j + file_header.width* (i+1))*3 + c;
                piccel2[(j + file_header.width* i)*3 + c] -= (int16_t)piccel[index];
            }
        }
    }
    // from left
    for(int i = 1; i < file_header.height-1; i++){
        for(int j = 1; j < file_header.width-1; j++){
            for(int c = 0; c< 3; c++){
                int index = ((j+1) + file_header.width* (i))*3 + c;
                piccel2[(j + file_header.width* i)*3 + c] -= (int16_t)piccel[index];
            }
        }
    }
    //frm right
    for(int i = 1; i < file_header.height-1; i++){
        for(int j = 1; j < file_header.width-1; j++){
            for(int c = 0; c< 3; c++){
                int index = ((j-1) + file_header.width* (i))*3 + c;
                piccel2[(j + file_header.width* i)*3 + c] -= (int16_t)piccel[index];
            }
        } 
    }

    for(int i = 0; i < file_header.height; i++){
        for(int c = 0; c< 3; c++){
            int index = (0 + file_header.width* i)*3 + c;
            piccel2[index] = piccel[index];
            // cout << (uint16_t)piccel[index] << endl;
        }
    }
    for(int i = 0; i < file_header.height; i++){
        for(int c = 0; c< 3; c++){
            int index = ((file_header.width-1) + file_header.width* i)*3 + c;
            piccel2[index] = piccel[index];
        }
    }
    for(int j = 0; j < file_header.width; j++){
        for(int c = 0; c< 3; c++){
            int index = (j + file_header.width* 0)*3 + c;
            piccel2[index] = piccel[index];
        }
    }
    for(int j = 0; j < file_header.width; j++){
        for(int c = 0; c< 3; c++){
            int index = (j + file_header.width* (file_header.height-1))*3 + c;
            piccel2[index] = piccel[index];
        }
    }
    
    int count_50 = 0;
    int count_101 = 0;
    int count_152 = 0;
    int count_203 = 0;
    int count_255 = 0;

    uint8_t* res = new uint8_t[pixcel_color_count];
    for(int i = 0; i < pixcel_color_count/3; i++){
        for(int c = 0; c < 3; c++){
             if(piccel2[i*3 + c] > 255){
                piccel2[i*3 + c] = 255;
            }
            else if(piccel2[i*3 + c] < 0){
                piccel2[i*3 + c] = 0;
            }
            res[i*3 + c] = piccel2[i*3 + c];
        }

        double color = 0.2126*res[i*3] + 0.7152*res[i*3+1] + 0.0722*res[i*3+2];

        int roundedColor = static_cast<int>(std::round(color));

        if(roundedColor <= 50){
            count_50++;
        }
        else if(roundedColor <= 101){
            count_101++;
        }
        else if(roundedColor <= 152){
            count_152++;
        }
        else if(roundedColor <= 203){
            count_203++;
        }
        else{
            count_255++;
        }
    }
    ofstream text("out.txt");
    text << count_50 << " " << count_101 << " " << count_152 << " " << count_203 << " " << count_255;
    text.close();

    ofs.write((char*)res, pixcel_color_count* sizeof(uint8_t));
    delete[] piccel;
    delete[] piccel2;
    delete[] res;
    ifs.close();
    ofs.close();

}