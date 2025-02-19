#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define elif else if
#define SIZE 256

struct threadInfo{
    vector<char>& outputASCII;
    vector<pair<char,double>> lookupTable;
    cv::Mat image;
    int colNum;

    threadInfo() : outputASCII(*(new vector<char>)) {}

    threadInfo(vector<char>& output , vector<pair<char,double>> table , cv::Mat img , int num) : outputASCII(output) , lookupTable(table) , image(img) , colNum(num){}
};


char lookup(vector<pair<char,double>> table , double brightness){
    int i = 0;
    int closeIndex;
    double closest = 100;

    for (i = 0; i < table.size(); i ++){
        if (abs(brightness - table[i].second) < closest){
            closest = abs(brightness - table[i].second);
            closeIndex = i;
        }
    }

    //cout << "DEBUG: " << brightness << " mapped to char " << table[closeIndex].first << " with bright of " << table[closeIndex].second << endl;
    return(table[closeIndex].first);
}

void* threadFunc(void* data){
    threadInfo* input = static_cast<threadInfo*>(data);
    double brightness;
    int i;

    for(i = 0; i < input->image.rows; i++){
        brightness = 255 - input->image.at<uchar>(i, input->colNum);
        brightness = input->lookupTable[0].second + (1-input->lookupTable[0].second) * (brightness/255);
        input->outputASCII[i] = lookup(input->lookupTable,brightness);
    }

    return(NULL);
}

int main(){
    int i;
    int j;
    double aspectRatio;
    cv::Mat image;
    ifstream lookupFile;
    ofstream outputFile;
    pair<int,int> canvasSize = {SIZE,0};
    vector<pair<char,double>> lookupTable;
    string temp;
    pthread_t thread;




    lookupFile.open("lookup.txt");
    if(lookupFile.is_open() == 1){
        while (getline(lookupFile, temp)) {
            lookupTable.push_back({temp[0] , stod(temp.substr(2))});
        }
        lookupFile.close();
    } else {
        cerr << "cant open lookup table";
        exit(0);
    }


    if(filesystem::exists("inputIMG.png")){
        image = imread("inputIMG.png" , IMREAD_GRAYSCALE);
    } elif (filesystem::exists("inputIMG.jpg")){
        image = imread("inputIMG.jpg" , IMREAD_GRAYSCALE);
    } else {
        cerr << "cant find inputIMG, please ensure inputIMG.png or .jpg is in the file with the .exe file" << endl;
        exit(0);
    }

    aspectRatio = static_cast<double>(image.rows) / static_cast<double>(image.cols);
    //cout << "DEBUG: aspect ratio - " << aspectRatio << endl;
    canvasSize.second = canvasSize.first * aspectRatio;

    resize(image, image, Size(canvasSize.first, canvasSize.second), INTER_LINEAR);
    vector<vector<char>> outputASCII(canvasSize.first, vector<char>(canvasSize.second, ' '));




    auto start = chrono::high_resolution_clock::now();
    //cout << "DEBUG: creating threads" << endl;

    for(i = 0; i < image.cols; i ++){
        threadInfo* holder = new threadInfo(outputASCII[i] , lookupTable , image , image.cols - i);
        pthread_create(&thread , NULL , threadFunc , holder);
    }

    //cout << "DEBUG: threads created, waiting for threads to finish" << endl;

    for (i = 0; i < image.cols; i++) {
        pthread_join(thread, NULL);
    }
    //cout << "DEBUG: threads finished" << endl;




    outputFile.open("ascii.txt");

    for (j = 0; j < outputASCII[0].size(); j++) {
        temp = "";
        for (i = outputASCII.size() - 1; i >= 0; i--) {
            temp += outputASCII[i][j];
        }
        outputFile << temp << "\n";
    }
    imwrite("debugIMG.png",image);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Created ASCII image in " << duration.count() << " milliseconds using " << thread << " threads." << endl;
    return(0);
}