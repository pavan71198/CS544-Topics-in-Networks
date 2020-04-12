#include <bits/stdc++.h>

using namespace std;

int main(int argc, char **argv){
    int switchPortCount = 8;
    int bufferSize = 4;
    float packetGenProb = 0.5;
    int queueType = 0;
    /*
        queueType
        0 - INQ
        1 - KOUQ
        2 - ISLIP
    */
    float knockout = 0.6*switchPortCount;
    int maxTimeSlots = 10000;
    char outputFile[100] = "routing-out.txt";
    // for (int i=0; i<argc; i++){
    //     cout << argv[i] << endl;
    // }
    for (int i=1; i<argc-1; i+=2){
        string option(argv[i]);
        if (option == "-N"){
            if (sscanf(argv[i+1],"%d",&switchPortCount) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (1)" << endl;
                return 1;
            }
            knockout = 0.6*switchPortCount;
        }
        else if (option == "-B"){
            if (sscanf(argv[i+1],"%d",&bufferSize) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (2)" << endl;
                return 1;
            }
        }
        else if (option == "-p"){
            if (sscanf(argv[i+1],"%f",&packetGenProb) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (3)" << endl;
                return 1;
            }
        }
        else if (option == "-queue"){
            char queueTypeArr[6];
            sscanf(argv[i+1],"%s",&queueTypeArr);
            string queueTypeStr(queueTypeArr);
            if (queueTypeStr == "INQ"){
                queueType = 0;
            }
            else if (queueTypeStr == "KOUQ"){
                queueType = 1;
            }
            else if (queueTypeStr == "ISLIP"){
                queueType = 2;
            }
            else{
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (4)" << endl;
                return 1;
            }
        }
        else if (option == "-K"){
            if (sscanf(argv[i+1],"%f",&knockout) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (5)" << endl;
                return 1;
            }
        }
        else if (option == "-out"){
            if (sscanf(argv[i+1],"%s",&outputFile) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (6)" << endl;
                return 1;
            }
        }
        else if (option == "-T"){
            if (sscanf(argv[i+1],"%d",&maxTimeSlots) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (7)" << endl;
                return 1;
            }
        }
        else {
            cout << option << endl;
            cout << "Wrong input given (8)" << endl;
            return 1;
        }
    }
    cout << "switchPortCount : " << switchPortCount << endl;
    cout << "bufferSize : " << bufferSize << endl;
    cout << "packetGenProb : " << packetGenProb << endl;
    cout << "queueType : " << queueType << endl;
    cout << "knockout : " << knockout << endl;
    cout << "maxTimeSlots : " << maxTimeSlots << endl;
    cout << "outputFile : " << outputFile << endl;
    return 0;
}