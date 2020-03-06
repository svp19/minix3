#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <string>

int fd1; 

// FIFO file path 

#define MAX 4096
#define vvi vector<vector<int>>
#define vi vector<int>
#define SNAME_READ "/readimage18"
#define SNAME_WRITE "/writeimage18"

using namespace std;

string myfifo = "myfifo5"; 
sem_t *write_sem;
sem_t *read_sem;


class Image
{
  public:
    int ** edges;
    int ** gray;
    int h, w, complete_rows;
    string file_name;

    Image(){
       h = w = 0;
    }

    void allocateMatrix() {
        // Dynamically allocate the 2D Array
        edges = new int*[h];
        gray = new int*[h];
        for(int i=0; i<h; ++i){
            edges[i] = new int[w];
            gray[i] = new int[w];
        }

        // Init completed rows
        complete_rows = 0;
    }

    void getDimensions() {
        // key_t my_key = ftok("shmfile", 65);
        // int shmid = shmget(my_key, sizeof(char), 0666|IPC_CREAT);
        // char *pixel = (char *) shmat(shmid,(void*)0,0);

        // sem_wait(write_sem);
        // h = atoi(pixel);
        // sem_post(read_sem);

        // sem_wait(write_sem);
        // w = atoi(pixel);
        // sem_post(read_sem);

        // shmdt(pixel);
        // shmctl(shmid,IPC_RMID,NULL);
        h = 1878;
        w = 1877;
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

    void detectEdges() {

        char *pixel = new char[w * h * sizeof(char) + 1];;
        fd1 = open(myfifo.c_str(),O_RDONLY); 
        cout << "Hello\n";
        for(int i=0; i<3; ++i){
            read(fd1, pixel, w * h + 1);
            for(int j=0; j < 3; ++j){
                gray[i][j] = ( (int) (*(pixel + i*w + j)) ) + 128;
                cout << gray[i][j] << " ";
            }
            // cout << "Read" << i << "\n";
            if(i >= 2 && i < h){
                processRow(i - 2);
            }
            close(fd1);
        }

        // Process remaining rows
        processRow(h - 2);
        processRow(h - 1);

        writeToFile(file_name);
    }

    void writeToFile(string filename){
        //writeToFile
        ofstream fout(filename);
        int filter[3][3] = {
            {1, 2, 1},
            {2, -13, 2},
            {1, 2, 1}
        };

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
    if(argc < 2){
        cout << "usage: ./a.out <path to output file>\n";
        return 0;
    }
    
    read_sem = sem_open(SNAME_READ, 0);
    write_sem = sem_open(SNAME_WRITE, 0);

    mkfifo(myfifo.c_str(), 0666);
    // if(mknod(myfifo.c_str(), S_IFIFO | 0666, 0) < 0){
    //     perror(myfifo.c_str());
    //     exit(1);
    // }
    
    Image image;

    // Get file name
    image.file_name = argv[2];
    image.getDimensions();
    image.allocateMatrix();
    
    // Transform
    image.detectEdges();
    cout<< "Image width: " << image.w << " and Image height: " << image.h << endl;

    sem_destroy(write_sem);
    sem_destroy(read_sem);

    return 0;
}