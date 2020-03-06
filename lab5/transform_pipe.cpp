#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <unistd.h> 

#define MAX 4096
#define vvi vector<vector<int>>
#define vi vector<int>
using namespace std;


class Image
{
  public:
    int** gray;
    int ** r;
    int ** g;
    int ** b;
    int ** edges;
    int h, w, new_h, new_w;
    string file_name;

    Image(){
       h = w = 0;
    }

    Image(string file_name){
        this->file_name = file_name;
        readImage(file_name);
    }

    void readImage(string file_name) {

        // Get file pointer
        ifstream fin(file_name);
        string first_line;
        int val;

        // Read header
        fin >> first_line;
        fin >> this->w;
        fin >> this->h;
        fin >> val;

        // Dynamically allocate all 2D Arrays
        gray = new int*[h];
        edges = new int*[h];
        r = new int*[h];
        g = new int*[h];
        b = new int*[h];
        for(int i=0; i<h; ++i){
            gray[i] = new int[w];
            edges[i] = new int[w];
            r[i] = new int[w];
            g[i] = new int[w];
            b[i] = new int[w];
        }

        // Read pixel values
        for(int i=0; i < h; ++i) {
            for(int j=0; j < w; ++j) {
                fin >> r[i][j] >> g[i][j] >> b[i][j];
            }
        }
        fin.close();
    }

    void grayScale() {
        for(int i=0; i < h; ++i) {
            for(int j = 0; j < w; ++j){
                gray[i][j] = 0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j];
            }
        }
    }


    void detectEdges() {
        int filter[3][3] = {
            {1, 2, 1},
            {2, -13, 2},
            {1, 2, 1}
        };

        new_h = h - 2;
        new_w = w - 2;


        for(int i = 0; i < h; ++i){
            for(int j = 0; j < w; ++j){
                int pixel = 0;
                for(int x = 0; x<3; ++x){
                    for(int y = 0; y<3; ++y){
                        if(i + x >= h || j + y >= w)
                            continue;
                        pixel += gray[i+x][j+y] * filter[x][y];
                    }
                }
                edges[i][j] = max(min(pixel, 255), 0);
            }
        }
        // while(pixels != h);
        writeToFile(file_name);
    }

    void processRow(int row) {

        int filter[3][3] = {
            {1, 2, 1},
            {2, -13, 2},
            {1, 2, 1}
        };

        for(int j=0; j < w; ++j) {
            int pixel = 0;
            for(int x = 0; x<3; ++x){
                for(int y = 0; y<3; ++y){
                    if(row + x >= h || j + y >= w)
                        continue;
                    pixel += gray[row+x][j+y] * filter[x][y];
                }
            }
            edges[row][j] = max(min(pixel, 255), 0);
        }
    }

    void wrapper(int i=0){

        int p[2], pid, nbytes;
        if(pipe(p) < 0){
            cout << "Failed to create pipe\n";
            exit(0);
        }
        // Writer
        if((pid=fork()) > 0){
            char *pixel = new char[w + 1];
            for(int i=0; i < h; ++i) {
                for(int j = 0; j < w; ++j){
                    string row = "";
                    row += (char) (( (int) (0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j])) + 128);
                    strcpy(pixel + j, row.c_str());
                }
                write(p[1], pixel, w *sizeof(char) + 1 );
                // cout << "W: " << ( (int) (*(pixel + 0)) ) + 128 << "i: "<< i << "\n";
            }
        }
        // Reader
        else {
            int row = 0;
            char inbuf[w * sizeof(char) + 1]; 
            while ((nbytes = read(p[0], inbuf, w * sizeof(char) + 1)) > 0){
                for(int j=0; j < w; ++j){
                    gray[row][j] = ( (int) (*(inbuf + j)) ) + 128;
                }
                if(row >= 2 && row < h){
                    processRow(row - 2);
                }
                cout << "Row: " << row << "\n";
                row++;
                if(row >= h)
                    break;
            }

            // Process remaining rows
            cout << "Done";            
            processRow(h - 2);
            processRow(h - 1);
            cout << "Done";

            writeToFile(file_name);
        } 
    }

    void writeToFile(string filename){
        //writeToFile
        ofstream fout(filename);

        fout << "P2\n";
        fout << w << " " << h << "\n";
        fout << 255 << "\n";
        for(int i=0; i < h; ++i){
            for(int j = 0; j < w; ++j){
                fout << edges[i][j] << "\n";
            }
        }
        fout.close();
    }
};

int main(int argc, char** argv) {
    if(argc < 3){
        cout << "usage: ./a.out <path to input file> <path to output file>\n";
        return 0;
    }
    // Read Image
    Image image(argv[1]);
    image.file_name = argv[2];
    
    image.wrapper();

    // Write output
    // image.writeToFile(argv[2]);
    return 0;
}