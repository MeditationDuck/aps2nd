#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

struct PPMFileHeader{
    int width;
    int height;
};
const float weightR = 0.2126f;
const float weightG = 0.7152f;
const float weightB = 0.0722f;

int main(int argc, char** argv){

    ifstream ifs(argv[1], ios::binary);
    ofstream ofs("output.ppm", ios::binary);

   
    PPMFileHeader file_header;
    char magic[3];
    ifs.read((char*)&magic, 3);
    ofs.write((char*)&magic, 3);

    string line;
    uint16_t max;
    while(ifs.peek() == '#'){
        getline(ifs, line);
        ofs.write(line.c_str(), line.size());
    }
    ifs >> file_header.width >> file_header.height >> max;
    ifs.get();
    ofs << file_header.width << "\n" << file_header.height << "\n" << max << "\n";
    
    cout << file_header.width << " " << file_header.height << " " << max << endl;
    int pixcel_color_count = file_header.width * file_header.height* 3;
    uint8_t* piccel = new uint8_t[pixcel_color_count];
    uint8_t* res = new uint8_t[pixcel_color_count];
    int counts[5] = {0, 0, 0, 0, 0};

    ifs.read((char*)piccel, pixcel_color_count* sizeof(uint8_t));
    int totalSize = file_header.width * file_header.height * 3;
    std::copy(piccel, piccel + totalSize, res);
    const int TILE_SIZE = 1800;

   
    for (int tileRow = 1; tileRow < file_header.height - 1; tileRow += TILE_SIZE) {
        int maxRow = std::min(tileRow + TILE_SIZE, file_header.height - 1);
        for (int tileCol = 1; tileCol < file_header.width - 1; tileCol += TILE_SIZE) {
            int maxCol = std::min(tileCol + TILE_SIZE, file_header.width - 1);
            for (int i = tileRow; i < maxRow; i++) {
                for (int j = tileCol; j < maxCol; j++) {
                    for (int c = 0; c < 3; c++) {
                        int16_t tmp =
                            -(int16_t)piccel[(j + file_header.width * (i + 1)) * 3 + c]
                            -(int16_t)piccel[(j + file_header.width * (i - 1)) * 3 + c]
                            -(int16_t)piccel[((j - 1) + file_header.width * i) * 3 + c]
                            +(int16_t)5 * (int16_t)piccel[(j + file_header.width * i) * 3 + c]
                            -(int16_t)piccel[((j + 1) + file_header.width * i) * 3 + c];
                        if (tmp > 255) {
                            tmp = 255;
                        } else if (tmp < 0) {
                            tmp = 0;
                        }
                        res[(j + file_header.width * i) * 3 + c] = tmp;
                    }

                    float color = weightR * res[(j + file_header.width * i) * 3] + 
                                weightG * res[(j + file_header.width * i) * 3 + 1] + 
                                weightB * res[(j + file_header.width * i) * 3 + 2];
                    uint8_t count_index = (uint8_t)std::round(color)/51;
                    if(count_index> 4) count_index = 4;

                    counts[count_index]++;
                }
            }
        }
    }
 

    for(int i = 1; i < file_header.height-1; i++){
        int index = (file_header.width* i)*3;
        int index1 = ((file_header.width-1) + file_header.width* i)*3;
        float color = weightR * res[index] + weightG * res[index+ 1] + weightB * res[index+ 2];
         uint8_t count_index = (uint8_t)std::round(color)/51;
                    if(count_index> 4) count_index = 4;

                    counts[count_index]++;
        color = weightR * res[index1] + weightG * res[index1+ 1] + weightB * res[index1+ 2];
        count_index = (uint8_t)std::round(color)/51;
        if(count_index> 4) count_index = 4;

        counts[count_index]++;
    }
    for(int j = 0; j < file_header.width; j++){
        int index = (j)*3 ;
        int index1 = (j + file_header.width* (file_header.height-1))*3;
        float color = weightR * res[index] + weightG * res[index+ 1] + weightB * res[index+ 2];
         uint8_t count_index = (uint8_t)std::round(color)/51;
                    if(count_index> 4) count_index = 4;

                    counts[count_index]++;
        color = weightR * res[index1] + weightG * res[index1+ 1] + weightB * res[index1+ 2];
        count_index = (uint8_t)std::round(color)/51;
                    if(count_index> 4) count_index = 4;

                    counts[count_index]++;
    }
  
    ofstream text("output.txt");
    text << counts[0] << " " << counts[1] << " " << counts[2] << " " << counts[3] << " " << counts[4];
    text.close();

    ofs.write((char*)res, pixcel_color_count* sizeof(uint8_t));
    delete[] piccel;
    delete[] res;
    ifs.close();
    ofs.close();
}