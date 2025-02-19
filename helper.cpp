#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;

double getBrightness(char c){
    int imgSize = 32;  // Image size for rendering
    cv::Mat img(imgSize, imgSize, CV_8UC1, cv::Scalar(255));  // White background

    if (img.empty()) {
        std::cerr << "Error: Failed to create image!" << std::endl;
        return -1.0;
    }

    // Render character using OpenCV's built-in font
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;  // Default OpenCV font
    double fontScale = 1.0;  // Adjust size as needed
    int thickness = 2;  // Thickness of the character
    int baseline = 0;
    
    // Compute text size and position
    cv::Size textSize = cv::getTextSize(std::string(1, c), fontFace, fontScale, thickness, &baseline);
    cv::Point textOrg((imgSize - textSize.width) / 2, (imgSize + textSize.height) / 2);

    // Draw character
    cv::putText(img, std::string(1, c), textOrg, fontFace, fontScale, cv::Scalar(0), thickness);

    // Compute average brightness
    double brightness = cv::mean(img)[0] / 255.0;  // Normalize [0,1]
    return (brightness);
}

int main(){
    ofstream lookupTable;
    string avalableChars = "~`8EXxTGgFfQZz50@#$%^&*(.,?</\\-+=: ";
    vector<pair<char,double>> brightMap;
    double bright;

    for(char c : avalableChars){
        bright = getBrightness(c);
        brightMap.push_back({c,bright});
    }

    sort(brightMap.begin(),brightMap.end(),[](const auto& a, const auto& b) { return a.second < b.second; });

    /*
    for (const auto& pair : brightMap) {
        cout << pair.first << " - " << pair.second << endl;
    }
    //*/

    lookupTable.open("lookup.txt", ifstream::out | ifstream::trunc);
    if(lookupTable.is_open() == 1){
        for (const auto& pair : brightMap) {
            lookupTable << pair.first << " " << pair.second << endl;
        }
    } else {
        cerr << "error opening file" << endl;
    }

    lookupTable.close();
    return(0);
}