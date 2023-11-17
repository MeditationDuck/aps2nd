#include <iostream>
#include <fstream>
#include <iomanip>

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

void convolution(const string& in_file){
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
        cout << file_header.magic[0] << " " << file_header.magic[1] << endl;
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

    ofs << file_header.width << " " << file_header.height << "\n" << max << "\n";

    cout <<"Header:" << endl;
    cout << "Width: " <<file_header.width << endl;
    cout << "Height: " << file_header.height << endl;
    cout << "Max: " << max << endl;

    file_size -= ifs.tellg();



    uint8_t* piccel = new uint8_t[file_size];
    int16_t* piccel2 = new int16_t[file_size];
    ifs.read((char*)piccel, file_size);
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
    

    uint8_t* res = new uint8_t[file_size];
    for(int i = 0; i < file_size; i++){
        if(piccel2[i] > 255){
            piccel2[i] = 255;
        }
        else if(piccel2[i] < 0){
            piccel2[i] = 0;
        }
        res[i] = piccel2[i];
    }

    ofs.write((char*)res, file_size);

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

    ifs.seekg(0, std::ifstream::end);
	long file_size = ifs.tellg();
	ifs.seekg(0,std::ifstream::beg);

    PPMFileHeader file_header;

    ifs.read((char*)&file_header, 3);
    if(!ifs){
        ifs.close();
        cout << "magic read error" << endl;
        return;
    }

    if(file_header.magic[0] != 'P' || file_header.magic[1] != '6'){
        ifs.close();
        cout << "magic number error" << endl;
        cout << file_header.magic[0] << " " << file_header.magic[1] << endl;
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

    file_size -= ifs.tellg();

    PPMPiccel* piccel = new PPMPiccel[file_size];
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
                cout << setfill('0') << setw(4) << (int)piccel[(j + file_header.width* i)*3 + c].r << " ";
            }
        }
        cout << endl;
    }
    ifs.close();
}


int main(){

    see_file("test-10x8/test.ppm");

    convolution("test-10x8/test.ppm");
    see_file("out.ppm");


    cout << "reference " << endl;

    see_file("test-10x8/output.ppm");
}


// int main(int argc, char** argv){
//     if( argc < 2 ){
//         cout << "invalid argument" << endl;
//         return 1;
//     }
//     string in_file = argv[1];
//     convolution(in_file);
// }