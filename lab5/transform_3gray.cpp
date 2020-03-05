#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX 4096
#define vvi vector<vector<int>>
#define vi vector<int>
#define SNAME_READ "/readimage"
#define SNAME_WRITE "/writeimage"

using namespace std;

sem_t *write_sem;
sem_t *read_sem;

class Image
{
  public:
    int ** r;
    int ** g;
    int ** b;
    int h, w;
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
        r = new int*[h];
        g = new int*[h];
        b = new int*[h];
        for(int i=0; i<h; ++i){
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
        key_t my_key = ftok("shmfile", 65); // ftok function is used to generate unique key
        int shmid = shmget(my_key, sizeof(int), 0666|IPC_CREAT); // shmget returns an ide in shmid
        int *pixel = (int*) shmat(shmid,(void*)0,0); // shmat to join to shared memory

        for(int i=0; i < 10; ++i) {
            for(int j = 0; j < 10; ++j){
                sem_wait(read_sem);
                *pixel = 0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j];
                cout << *pixel << "\n";
                sem_post(write_sem);
            }
        }

        shmdt(pixel);
    }
};

int main(int argc, char** argv) {
    if(argc < 3){
        cout << "usage: ./a.out <path to input file> <path to output file>\n";
        return 0;
    }

    write_sem = sem_open(SNAME_WRITE, O_CREAT, 0644, 0);  // Init value 0
    read_sem = sem_open(SNAME_READ, O_CREAT, 0644, 1); // Init value 1    
    

    // Read Image
    Image image(argv[1]);
    image.file_name = argv[2];
    
    // Transform
    image.grayScale();
    
    //detach from shared memory
    

    return 0;
}