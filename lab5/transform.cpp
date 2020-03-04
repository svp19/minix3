#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#define MAX 2048

using namespace std;

class Image
{
  public:
    vector<vector<int>> r, g, b;
    int w, h;
    string file_name;

    void readImage(string file_name) {

        this->file_name = file_name;
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
            vector<int> row_r, row_g, row_b;
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

    void grayScale(){
        vector<vector<int>> gray; 
        for(int i=0; i < h; ++i){
            vector<int> row;
            for(int j = 0; j < w; ++j){
                row.push_back(0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j]);
            }
            gray.push_back(row);
        }

        cout << "Wrting";
        //writeToFile
        ofstream fout("output.ppm");
        fout << "P2\n";
        fout << w << " " << h << "\n";
        fout << 255 << "\n";
        for(int i=0; i < h; ++i){
            for(int j = 0; j < w; ++j){
                fout << gray[i][j] << "\n";
            }
        }
        fout.close();
    }
};

int main(int argc, char** argv) {

    // Read Image
    Image img;
    img.readImage("mussorie.ppm");
    img.grayScale();
    return 0;
}