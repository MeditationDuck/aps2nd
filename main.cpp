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
    ofstream ofs("out.txt", ios::binary);
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


    // char magic_header[3];
    ifs.read((char*)&file_header, 3);
    if(!ifs){
        ifs.close();
        ofs.close();
        cout << "magic read error" << endl;
        return;
    }

    ofs.write((char*)&file_header, 3);
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
    }
    if(!(ifs >> file_header.width >> file_header.height >> max)){
        ifs.close();
        ofs.close();
        cout << "header read error" << endl;
        return;
    }
    cout <<"Header:" << endl;
    cout << "Width: " <<file_header.width << endl;
    cout << "Height: " << file_header.height << endl;
    cout << "Max: " << max << endl;

    if(max != 255){
        ifs.close();
        ofs.close();
        cout << "max value error" << endl;
        return;
    }

    ifs.get();// one caracter '\n'

    file_size -= sizeof(PPMFileHeader);

    PPMPiccel* piccel = new PPMPiccel[file_size];
    // PPMPiccel* piccel2 = new PPMPiccel[file_size];
    ifs.read((char*)piccel, file_size);
    if(!ifs){
        ifs.close();
        ofs.close();
        cout << "piccel read error" << endl;
        return;
    }

    for(int i = 0; i < file_header.width; i++){
        for(int j = 0; j < file_header.height; j++){
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
                cout << setfill('0') << setw(3) << (int)piccel[(j + file_header.width* i)*3 + c].r << " ";
            }
        }
        cout << endl;
    }

    ifs.close();
    ofs.close();
}

int main(){
    convolution("test-10x8/test.ppm");
}


// int main(int argc, char** argv){
//     if( argc < 2 ){
//         cout << "invalid argument" << endl;
//         return 1;
//     }
//     string in_file = argv[1];
//     convolution(in_file);
// }