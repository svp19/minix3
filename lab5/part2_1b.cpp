#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <semaphore.h>

#define MAX 4096
#define vvi vector<vector<int>>
#define vi vector<int>
using namespace std;

sem_t semaphore;

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
            if(i > 2 && i < h){
                sem_post(&semaphore);
            }
        }
    }


    void detectEdges() {
        int filter[3][3] = {
            {1, 2, 1},
            {2, -13, 2},
            {1, 2, 1}
        };

        for(int i = 0; i < h; ++i){
            if(i < h - 4)
                sem_wait(&semaphore);
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
    
    sem_init(&semaphore, 0, 0);
    // Transform
    // cout<< "main(): creating gray thread\n";
    thread gray_thread( &Image::grayScale, &image );

    // cout<< "main(): creating edge thread\n";
    thread edge_thread( &Image::detectEdges, &image );

    gray_thread.join();
    edge_thread.join();

    sem_destroy(&semaphore);
    // Write output
    // image.writeToFile(argv[2]);
    return 0;
}