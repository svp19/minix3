#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <atomic>

#define MAX 4096
#define vvi vector<vector<int>>
#define vi vector<int>

using namespace std;

atomic<int> pixels{0};

class Image
{
  public:
    vvi r, g, b, gray, edges;
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

        // Read pixel values
        for(int i=0; i < h; ++i) {
            vi row_r, row_g, row_b;
            for(int j=0; j < w; ++j) {
                fin >> val;
                row_r.push_back(val);
                fin >> val;
                row_g.push_back(val);
                fin >> val;
                row_b.push_back(val);
            }
            r.push_back(row_r);
            g.push_back(row_g);
            b.push_back(row_b);
        }
        fin.close();
    }

    void grayScale() {
        for(int i=0; i < h; ++i) {
            vi row;
            for(int j = 0; j < w; ++j){
                row.push_back(0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j]);
            }
            gray.push_back(row);
        }

        pixels++;
    }


    void detectEdges() {
        int filter[3][3] = {
            {1, 2, 1},
            {2, -13, 2},
            {1, 2, 1}
        };

        new_h = h - 2;
        new_w = w - 2;

        while(pixels < 1);

        for(int i = 0; i < h; ++i){
            vi row;
            for(int j = 0; j < w; ++j){
                int pixel = 0;
                for(int x = 0; x<3; ++x){
                    for(int y = 0; y<3; ++y){
                        if(i + x >= h || j + y >= w)
                            continue;
                        pixel += gray[i+x][j+y] * filter[x][y];
                    }
                }
                row.push_back(max(min(pixel, 255), 0));
            }
            edges.push_back(row);
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
    
    // Transform
    // cout<< "main(): creating gray thread\n";
    thread gray_thread( &Image::grayScale, &image );

    // cout<< "main(): creating edge thread\n";
    thread edge_thread( &Image::detectEdges, &image );

    gray_thread.join();
    edge_thread.join();

    // Write output
    image.writeToFile(argv[2]);
    return 0;
}