#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "struct.h"

using namespace std;


void convolution(const string& in_file){
    ifstream ifs(in_file, ios::binary);
    ofstream ofs("out.ppm", ios::binary);
    if(!ifs || !ofs){
        if(!ifs){
            ifs.close();
        }
        return ;
    }

    PPMFileHeader file_header;

    char magic[3];

    ifs.read((char*)&magic, 3);
    ofs.write((char*)&magic, 3);
    if(!ifs){
        ifs.close();
        ofs.close();
        return;
    }

    if(ofs.bad()){
        ifs.close();
        ofs.close();
        return;
    }

    if(magic[0] != 'P' || magic[1] != '6'){
        ifs.close();
        ofs.close();
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
                piccel2[index] = (int16_t)piccel[index] * (int16_t)5;
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
            int index = (file_header.width* i)*3 + c;
            piccel2[index] = piccel[index];
            int index1 = ((file_header.width-1) + file_header.width* i)*3 + c;
            piccel2[index1] = piccel[index1];
        }
    }
    for(int j = 0; j < file_header.width; j++){
        for(int c = 0; c< 3; c++){
            int index = (j)*3 + c;
            piccel2[index] = piccel[index];
            int index2 = (j + file_header.width* (file_header.height-1))*3 + c;
            piccel2[index2] = piccel[index2];
        }
    }
    
    int counts[5] = {0, 0, 0, 0, 0};

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

        float color = 0.2126*res[i*3] + 0.7152*res[i*3+1] + 0.0722*res[i*3+2];
        uint8_t roundedColor = (uint8_t)std::round(color);
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
    delete[] piccel2;
    delete[] res;
    ifs.close();
    ofs.close();

}

void see_file(const string& in_file)  {
    ifstream ifs(in_file, ios::binary);
    if(!ifs){
        if(!ifs){
            ifs.close();
        }
        cout << "file open error" << endl;
        return ;
    }

    PPMFileHeader file_header;
    char magic[3];

    ifs.read((char*)&magic, 3);
    if(!ifs){
        ifs.close();
        cout << "magic read error" << endl;
        return;
    }

    if(magic[0] != 'P' || magic[1] != '6'){
        ifs.close();
        cout << "magic number error" << endl;
        return;
    }

    string line;
    uint16_t max;
    while(ifs.peek() == '#'){
        getline(ifs, line);
    }
    if(!(ifs >> file_header.width >> file_header.height >> max)){
        ifs.close();
        cout << "header read error" << endl;
        return;
    }

    if(max != 255){
        ifs.close();
        cout << "max value error" << endl;
        return;
    }
    ifs.get();// one caracter '\n'

    cout <<"Header:" << endl;
    cout << "Width: " <<file_header.width << endl;
    cout << "Height: " << file_header.height << endl;
    cout << "Max: " << max << endl;

    int file_size = file_header.width * file_header.height* 3;

    uint8_t* piccel = new uint8_t[file_size];
    ifs.read((char*)piccel, file_size);
    if(!ifs){
        ifs.close();
        cout << "piccel read error" << endl;
        return;
    }

    for(int i = 0; i < file_header.height; i++){
        for(int j = 0; j < file_header.width; j++){
            for(int c = 0; c< 3; c++){
                if(c == 0){
                    cout << "R: ";
                }
                else if(c == 1){
                    cout << "G: ";
                }
                else if(c == 2){
                    cout << "B: ";
                }
                cout << setfill('0') << setw(4) << (int)piccel[(j + file_header.width* i)*3 + c] << " ";
            }
        }
        cout << endl;
    }
    ifs.close();
}


// int main(){

//     see_file("test-10x8/test.ppm");

//     convolution("test-10x8/test.ppm");
//     see_file("out.ppm");


//     cout << "reference " << endl;

//     see_file("test-10x8/output.ppm");
// }


// int main(int argc, char** argv){
//     if( argc < 2 ){
//         cout << "invalid argument" << endl;
//         return 1;
//     }
//     string in_file = argv[1];
//     convolution(in_file);
// }