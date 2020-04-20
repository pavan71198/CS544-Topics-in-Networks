#include <bits/stdc++.h>

using namespace std;

class InqInputPort {
    public:
        queue <pair <int, int>> packets;
        InqInputPort(int switchPortCount, int bufferSize, float packetGenProb){
            this->bufferSize = bufferSize;
            this->packetGenProb = packetGenProb;
            this->switchPortCount = switchPortCount;
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            this->packetRandGen = default_random_engine(seed);
            seed = chrono::system_clock::now().time_since_epoch().count();
            this->outputPortRandGen = default_random_engine(seed);
            this->packetDist = uniform_real_distribution <float> (0.0, 1.0);
            this->outputPortDist = uniform_int_distribution <int> (0,switchPortCount-1);
        }
        void generatePacket(){
            if (packetDist(packetRandGen)<packetGenProb){
                if (packets.size()<bufferSize){
                    pair <int,int> newPacket;
                    newPacket.first = curTimeSlot;
                    newPacket.second = outputPortDist(outputPortRandGen);
                    packets.push(newPacket);
                }
            }
            curTimeSlot++;
        }
    private:
        int curTimeSlot = 0;
        int bufferSize;
        float packetGenProb;
        int switchPortCount;
        default_random_engine packetRandGen;
        default_random_engine outputPortRandGen;
        uniform_real_distribution <float> packetDist;
        uniform_int_distribution <int> outputPortDist;
};

class KouqInputPort {
    public:
        pair <int, int> packet;
        KouqInputPort(int switchPortCount, float packetGenProb){
            this->packetGenProb = packetGenProb;
            this->switchPortCount = switchPortCount;
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            this->packetRandGen = default_random_engine(seed);
            seed = chrono::system_clock::now().time_since_epoch().count();
            this->outputPortRandGen = default_random_engine(seed);
            this->packetDist = uniform_real_distribution <float> (0.0, 1.0);
            this->outputPortDist = uniform_int_distribution <int> (0,switchPortCount-1);
        }
        void generatePacket(){
            if (packetDist(packetRandGen)<packetGenProb){
                packet.first = curTimeSlot;
                packet.second = outputPortDist(outputPortRandGen);
            }
            else{
                packet.second = -1;
            }
            curTimeSlot++;
        }
    private:
        int curTimeSlot = 0;
        float packetGenProb;
        int switchPortCount;
        default_random_engine packetRandGen;
        default_random_engine outputPortRandGen;
        uniform_real_distribution <float> packetDist;
        uniform_int_distribution <int> outputPortDist;
};

void scheduleInq (vector<InqInputPort> &inputPorts, vector<int> &delays, int curTimeSlot, int switchPortCount){
    vector<vector<int>> outputPackets (switchPortCount, vector<int>(0));
    for (int i=0; i<switchPortCount; i++){
        if (!inputPorts[i].packets.empty()){
            outputPackets[inputPorts[i].packets.front().second].push_back(i);
        }
    }
    for (int i=0; i<switchPortCount; i++){
        if (outputPackets[i].size()>=1){
            int randPacketIdx = rand()%outputPackets[i].size();
            delays.push_back(curTimeSlot-inputPorts[outputPackets[i][randPacketIdx]].packets.front().first);
            inputPorts[outputPackets[i][randPacketIdx]].packets.pop();
        }
    }
}

void scheduleKouq (vector<KouqInputPort> &inputPorts, vector<queue<int>> &outputQs, int &kouqDrops, int switchPortCount, int bufferSize, int knockout){
    vector<vector<int>> outputPackets (switchPortCount, vector<int>(0));
    for (int i=0; i<switchPortCount; i++){
        if (inputPorts[i].packet.second!=-1){
            outputPackets[inputPorts[i].packet.second].push_back(inputPorts[i].packet.first);
        }
    }
    for (int i=0; i<switchPortCount; i++){
        if (outputPackets[i].size()>knockout){
            kouqDrops++;
            random_shuffle(outputPackets[i].begin(), outputPackets[i].end());
            for (int j=0; j<knockout; j++){
                if (outputQs[i].size()>=bufferSize){
                    break;
                }
                outputQs[i].push(outputPackets[i][j]);
            }
        }
        else{
            random_shuffle(outputPackets[i].begin(), outputPackets[i].end());
            for (int j=0; j<outputPackets[i].size(); j++){
                if (outputQs[i].size()>=bufferSize){
                    break;
                }
                outputQs[i].push(outputPackets[i][j]);
            }
        }
    }
}

void transmitKouq(vector<queue<int>> &outputQs, int curTimeSlot, vector<int> &delays, int switchPortCount){
    for (int i=0; i<switchPortCount; i++){
        if (!outputQs[i].empty()){
            delays.push_back(curTimeSlot-outputQs[i].front());
            outputQs[i].pop();
        }
    }
}

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
    // cout << "switchPortCount : " << switchPortCount << endl;
    // cout << "bufferSize : " << bufferSize << endl;
    // cout << "packetGenProb : " << packetGenProb << endl;
    // cout << "queueType : " << queueType << endl;
    // cout << "knockout : " << knockout << endl;
    // cout << "maxTimeSlots : " << maxTimeSlots << endl;
    // cout << "outputFile : " << outputFile << endl;
    if (queueType == 0){
        vector <InqInputPort> inputPorts (switchPortCount, InqInputPort (switchPortCount, bufferSize, packetGenProb));
        vector <int> delays;
        for (int i=0; i<maxTimeSlots; i++){
            for (int j=0; j<switchPortCount; j++){
                inputPorts[j].generatePacket();
            }
            scheduleInq(inputPorts, delays, i, switchPortCount);
        }
        float delaySum, delaySqSum;
        for (int i=0; i<delays.size(); i++){
            delaySum += delays[i];
        }
        float averageDelay = delaySum/delays.size();
        for (int i=0; i<delays.size(); i++){
            delaySqSum += (delays[i]-averageDelay)*(delays[i]-averageDelay);
        }
        float sdDelay = delaySqSum/delays.size();
        sdDelay = sqrt(sdDelay);
        float linkUtil = (float)delays.size()/(maxTimeSlots*switchPortCount);
        cout << "Avg. Delay: " << averageDelay << endl;
        cout << "SD Delay: " << sdDelay << endl;
        cout << "Link Utilization: " << linkUtil <<endl;
    }
    else if (queueType == 1){
        vector <KouqInputPort> inputPorts (switchPortCount, KouqInputPort (switchPortCount, packetGenProb));
        vector <int> delays;
        queue<int> outQ;
        vector <queue <int>> outputQs(switchPortCount, outQ);
        int kouqDrops=0;
        for (int i=0; i<maxTimeSlots; i++){
            for (int j=0; j<switchPortCount; j++){
                inputPorts[j].generatePacket();
            }
            scheduleKouq(inputPorts, outputQs, kouqDrops, switchPortCount, bufferSize, knockout);
            transmitKouq(outputQs, i, delays, switchPortCount);
        }
        float delaySum, delaySqSum;
        for (int i=0; i<delays.size(); i++){
            delaySum += delays[i];
        }
        float averageDelay = delaySum/delays.size();
        for (int i=0; i<delays.size(); i++){
            delaySqSum += (delays[i]-averageDelay)*(delays[i]-averageDelay);
        }
        float sdDelay = delaySqSum/delays.size();
        sdDelay = sqrt(sdDelay);
        float linkUtil = (float)delays.size()/(maxTimeSlots*switchPortCount);
        float kouqDropProb = (float)kouqDrops/(maxTimeSlots*switchPortCount);
        cout << "Avg. Delay: " << averageDelay << endl;
        cout << "SD Delay: " << sdDelay << endl;
        cout << "Link Utilization: " << linkUtil <<endl;
        cout << "KOUQ Drop Probability: " << kouqDropProb <<endl;
    }

    return 0;
}