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
#define SNAME_READ "/readimage16"
#define SNAME_WRITE "/writeimage16"

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

    void sendDimensions() {
        key_t my_key = ftok("shmfile", 65);
        int shmid = shmget(my_key, sizeof(char), 0666|IPC_CREAT);
        char *pixel = (char *) shmat(shmid,(void*)0,0);
        string h_str = to_string(h), w_str = to_string(w);

        sem_wait(read_sem);
        strcpy(pixel, h_str.c_str());
        sem_post(write_sem);

        sem_wait(read_sem);
        strcpy(pixel, w_str.c_str());
        sem_post(write_sem);

        shmdt(pixel);
    }

    // Add 128 to int while casting to char (char) (i + 128)
    // Add 128 to the casted int from a char ((int) x) + 128
    void grayScale() {
        key_t my_key = ftok("shmfile_image", 65); // ftok function is used to generate unique key
        int shmid = shmget(my_key, w * h * sizeof(char) + 1, 0666|IPC_CREAT); // shmget returns an ide in shmid
        char *pixel = (char *) shmat(shmid,(void*)0,0); // shmat to join to shared memory

        for(int i=0; i < h; ++i) {
            // sem_wait(read_sem);
            for(int j = 0; j < w; ++j){
                string row = "";
                row += (char) (( (int) (0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j])) + 128);
                strcpy(pixel + i*w + j, row.c_str());
                // cout<< 0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j]<< "\n";
            }

            // cout<< endl << row << " " << row.size() <<endl;
            sem_post(write_sem);
        }

        shmdt(pixel);
    }
};

int main(int argc, char** argv) {
    if(argc < 2){
        cout << "usage: ./a.out <path to input file>\n";
        return 0;
    }

    write_sem = sem_open(SNAME_WRITE, O_CREAT, 0644, 0);  // Init value 0
    read_sem = sem_open(SNAME_READ, O_CREAT, 0644, 1); // Init value 1
    // cout<< sem_get<<endl;
    // cout<< write_sem<<endl;

    // Read Image
    Image image(argv[1]);
    image.file_name = argv[2];
    image.sendDimensions();
    
    // Transform
    image.grayScale();
    
    //detach from shared memory

    return 0;
}