#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX 4096
#define vvi vector<vector<int>>
#define vi vector<int>
#define SNAME_READ "/readimage8"
#define SNAME_WRITE "/writeimage8"

using namespace std;

sem_t *write_sem;
sem_t *read_sem;


class Image
{
  public:
    int ** edges;
    int h, w;
    string file_name;

    Image(){
       h = w = 0;
    }

    void allocateMatrix() {
        // Dynamically allocate the 2D Array
        edges = new int*[h];
        for(int i=0; i<h; ++i){
            edges[i] = new int[w];
        }
    }

    void getDimensions() {
        key_t my_key = ftok("shmfile", 65);
        int shmid = shmget(my_key, sizeof(char), 0666|IPC_CREAT);
        char *pixel = (char *) shmat(shmid,(void*)0,0);

        sem_wait(write_sem);
        h = atoi(pixel);
        sem_post(read_sem);

        sem_wait(write_sem);
        w = atoi(pixel);
        sem_post(read_sem);

        shmdt(pixel);
        shmctl(shmid,IPC_RMID,NULL);
    }

    void detectEdges() {
        int filter[3][3] = {
            {1, 2, 1},
            {2, -13, 2},
            {1, 2, 1}
        };

        key_t my_key = ftok("shmfile_image", 65); // ftok function is used to generate unique key
        int shmid = shmget(my_key, sizeof(w * sizeof(char)), 0666|IPC_CREAT); // shmget returns an ide in shmid

        char *pixel = (char *) shmat(shmid,(void*)0,0); // shmat to join to shared memory
        
        for(int i=0; i<h; ++i){
            sem_wait(write_sem);

            for(int j=0; j < w; ++j){
                // cout << ( (int) (*(pixel + j))) + 128 << "\n";
                edges[i][j] = ( (int) (*(pixel + j)) ) + 128;
            }

            sem_post(read_sem);
        }
        shmdt(pixel);
        shmctl(shmid,IPC_RMID,NULL); // destroy the shared memory

        // new_h = h - 2;
        // new_w = w - 2;

        // for(int i = 0; i < h; ++i){
        //     for(int j = 0; j < w; ++j){
        //         int pixel = 0;
        //         for(int x = 0; x<3; ++x){
        //             for(int y = 0; y<3; ++y){
        //                 if(i + x >= h || j + y >= w)
        //                     continue;
        //                 pixel += gray[i+x][j+y] * filter[x][y];
        //             }
        //         }
        //         edges[i][j] = max(min(pixel, 255), 0);
        //     }
        // }

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
    if(argc < 2){
        cout << "usage: ./a.out <path to output file>\n";
        return 0;
    }
    
    read_sem = sem_open(SNAME_READ, 0);
    write_sem = sem_open(SNAME_WRITE, 0);

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