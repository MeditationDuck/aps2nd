#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

// int main(int argc, char** argv){
//     if( argc < 2 ){
//         cout << "invalid argument" << endl;
//         return 1;
//     }
//     string in_file = argv[1];
//     convolution(in_file);
// }
struct PPMFileHeader{
    int width;
    int height;
};


int main(int argc, char** argv){


    ifstream ifs(argv[1], ios::binary);
    ofstream ofs("output.ppm", ios::binary);
    if(!ifs || !ofs){
        if(!ifs){
            ifs.close();
        }
        return 1;
    }

    PPMFileHeader file_header;

    char magic[3];

    ifs.read((char*)&magic, 3);
    ofs.write((char*)&magic, 3);
    if(!ifs){
        ifs.close();
        ofs.close();
        return 1;
    }

    if(ofs.bad()){
        ifs.close();
        ofs.close();
        return 1;
    }

    if(magic[0] != 'P' || magic[1] != '6'){
        ifs.close();
        ofs.close();
        return 1;
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
        return 1;
    }
    if(max != 255){
        ifs.close();
        ofs.close();
        cout << "max value error" << endl;
        return 1; 
    }
    ifs.get();
    ofs << file_header.width << "\n" << file_header.height << "\n" << max << "\n";
    

    int pixcel_color_count = file_header.width * file_header.height* 3;
    uint8_t* piccel = new uint8_t[pixcel_color_count];
    uint8_t* res = new uint8_t[pixcel_color_count];
    int counts[5] = {0, 0, 0, 0, 0};

    ifs.read((char*)piccel, pixcel_color_count* sizeof(uint8_t));

    int totalSize = file_header.width * file_header.height * 3;
    std::copy(piccel, piccel + totalSize, res);
    if(!ifs){
        ifs.close();
        ofs.close();
        cout << "piccel read error" << endl;
        return 1;
    }


    for(int i = 1; i < file_header.height-1; i++){
        for(int j = 1; j < file_header.width-1; j++){
            for(int c = 0; c < 3; c++){
                int index = (j + file_header.width* i)*3 + c;
                int16_t tmp = 
                    -(int16_t)piccel[((j-1) + file_header.width* (i))*3 + c]
                    +(int16_t)5* (int16_t)piccel[index] 
                    -(int16_t)piccel[((j+1) + file_header.width* (i))*3 + c]
                    -(int16_t)piccel[(j + file_header.width* (i-1))*3 + c]
                    -(int16_t)piccel[(j + file_header.width* (i+1))*3 + c];
                if(tmp > 255){
                    tmp = 255;
                }
                else if(tmp < 0){
                    tmp = 0;
                }
                res[index] = tmp;
            }
        }
    }
 

    // for(int i = 0; i < file_header.height; i++){
    //     for(int c = 0; c< 3; c++){
    //         int index = (file_header.width* i)*3 + c;
    //         res[index] = piccel[index];
    //         int index1 = ((file_header.width-1) + file_header.width* i)*3 + c;
    //         res[index1] = piccel[index1];
    //     }
    // }
    // for(int j = 0; j < file_header.width; j++){
    //     for(int c = 0; c< 3; c++){
    //         int index = (j)*3 + c;
    //         res[index] = piccel[index];
    //         int index2 = (j + file_header.width* (file_header.height-1))*3 + c;
    //         res[index2] = piccel[index2];
    //     }
    // }
  
    for(int i = 0; i < pixcel_color_count/3; i++){
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
    ofstream text("output.txt");
    text << counts[0] << " " << counts[1] << " " << counts[2] << " " << counts[3] << " " << counts[4];
    text.close();

    ofs.write((char*)res, pixcel_color_count* sizeof(uint8_t));
    delete[] piccel;
    delete[] res;
    ifs.close();
    ofs.close();
    return 0;
}