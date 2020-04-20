#include <bits/stdc++.h>
#include <fstream>

using namespace std;

class InqInputPort {
    public:
        queue <pair <int, int>> packets; //queue for storing packets to be processed
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
            /*
            *    generating packets based on packet genration probability
            */
            if (packetDist(packetRandGen)<packetGenProb){
                /*
                *   adding packets to the input buffer if buffer isn't full
                */
                if (packets.size()<bufferSize){
                    pair <int,int> newPacket;
                    newPacket.first = curTimeSlot;
                    newPacket.second = outputPortDist(outputPortRandGen); //selecting a random output port with uniform probability
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
            int randOutputPort = outputPortDist(outputPortRandGen);
            /*
            *    generating packets based on packet genration probability
            */
            if (packetDist(packetRandGen)<packetGenProb){
                packet.first = curTimeSlot;
                packet.second = randOutputPort; //selecting a random output port with uniform probability
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

class IslipInputPort {
    public:
        vector<queue <int>> packets;
        IslipInputPort(int switchPortCount, int bufferSize, float packetGenProb){
            this->bufferSize = bufferSize;
            this->packetGenProb = packetGenProb;
            this->switchPortCount = switchPortCount;
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            this->packetRandGen = default_random_engine(seed);
            seed = chrono::system_clock::now().time_since_epoch().count();
            this->outputPortRandGen = default_random_engine(seed);
            this->packetDist = uniform_real_distribution <float> (0.0, 1.0);
            this->outputPortDist = uniform_int_distribution <int> (0,switchPortCount-1);
            queue<int> inQ;
            this->packets = vector<queue <int>> (switchPortCount, inQ); //N queues, 1 for each of the output port
        }
        void generatePacket(){
            /*
            *    generating packets based on packet genration probability
            */
            if (packetDist(packetRandGen)<packetGenProb){
                int randOutputPort = outputPortDist(outputPortRandGen); //selecting a random output port with uniform probability
                /*
                *   adding packets to the input buffer if buffer isn't full
                */
                if (packets[randOutputPort].size()<bufferSize){
                    packets[randOutputPort].push(curTimeSlot);
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
            /*
            *   selecting a random packet if there are conflicts
            */
            delays.push_back(curTimeSlot-inputPorts[outputPackets[i][randPacketIdx]].packets.front().first);
            /*
            *   transmitting a packet just after getting it out of input queue
            */
            inputPorts[outputPackets[i][randPacketIdx]].packets.pop();
        }
    }
}

void scheduleKouq (vector<KouqInputPort> &inputPorts, vector<queue<int>> &outputQs, int &kouqDrops, int switchPortCount, int bufferSize, float knockout){
    vector<vector<int>> outputPackets (switchPortCount, vector<int>(0));
    for (int i=0; i<switchPortCount; i++){
        if (inputPorts[i].packet.second!=-1){
            outputPackets[inputPorts[i].packet.second].push_back(inputPorts[i].packet.first);
        }
    }
    for (int i=0; i<switchPortCount; i++){
        /*
        *   selecting K packets out of total corresponding to the output port and sending them to 
        *   output queue and dropping the rest if no. of packets > K
        */
        if (outputPackets[i].size()>knockout){
            kouqDrops++;
            for (int j=0; j<knockout; j++){
                if (outputQs[i].size()>=bufferSize){
                    break;
                }
                outputQs[i].push(outputPackets[i][j]);
            }
        }
        /*
        *   sending all the packets corresponding to the output port to the output queue if no. of packets <= K
        */
        else{
            for (int j=0; j<outputPackets[i].size(); j++){
                if (outputQs[i].size()>=bufferSize){
                    break;
                }
                outputQs[i].push(outputPackets[i][j]);
            }
        }
    }
}

void transmitKouq(vector<queue<int>> &outputQs, vector<int> &delays, int curTimeSlot, int switchPortCount){
    for (int i=0; i<switchPortCount; i++){
        if (!outputQs[i].empty()){
            /*
            *   transmitting 1 packet from each output's queue in a time slot.
            */
            delays.push_back(curTimeSlot-outputQs[i].front());
            outputQs[i].pop();
        }
    }
}

void scheduleIslip(vector<IslipInputPort> &inputPorts, vector<vector<bool>> &requests, vector<int> &delays, int curTimeSlot, int switchPortCount){
    bool requestsEmpty = true;
    /*
    *   checking for pre-existing requests to output ports from each input port
    */
    for (int i=0; i<switchPortCount; i++){
        for (int j=0; j<switchPortCount; j++){
            if (requests[i][j]!=false){
                requestsEmpty = false;
            }
        }
    }
    /*
    *   if there are no requests, getting requests to each output ports from each input port
    */
    if (requestsEmpty){
        for (int i=0; i<switchPortCount; i++){
            for (int j=0; j<switchPortCount; j++){
                if (!inputPorts[j].packets[i].empty()){
                    requests[i][j]=true;
                }
            }
        }
    }
    /*
    *   running one round of ISLIP on the requests
    */
    vector<bool> occupiedInputPorts(switchPortCount, false);
    vector<bool> occupiedOutputPorts(switchPortCount, false);
    vector<int> accepted(switchPortCount, -1);
    for (int k=0; k<switchPortCount; k++){
        bool serving = false;
        for (int i=0; i<switchPortCount; i++){
            for (int j=0; j<switchPortCount; j++){
                /*
                *   granting phase of checking whether the input and output ports are free and
                *   accepting phase of accepting the top request with 
                */
                if ((requests[i][j]) && (!occupiedInputPorts[j]) && (!occupiedOutputPorts[i])){
                    accepted[i]=j;
                    occupiedInputPorts[j]=true;
                    occupiedOutputPorts[i]=true;
                    serving = true; // marking whether a request is served in the round if not then there are no requests
                }
            }
        }
        if (!serving){
            break; // if no request is served end the ISLIP algo rounds
        }
    }
    // for (int i=0; i<switchPortCount; i++){
    //     cout << i << ": ";
    //     for (int j=0; j<switchPortCount; j++){
    //         if (requests[i][j]){
    //             cout << j << " ";
    //         }
    //     }
    //     cout << endl;
    // }
    for (int i=0; i<switchPortCount; i++){
        if (accepted[i] != -1){
            /*
            *   serving the accepted requests in this round of ISLIP
            */
            delays.push_back(curTimeSlot - inputPorts[accepted[i]].packets[i].front());
            inputPorts[accepted[i]].packets[i].pop();
            requests[i][accepted[i]]=false;
            // cout << i << " " << accepted[i] << endl;
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
    bool knockoutSet = false;
    int maxTimeSlots = 10000;
    char outputFile[100] = "routing-out.txt";
    // for (int i=0; i<argc; i++){
    //     cout << argv[i] << endl;
    // }
    for (int i=1; i<argc-1; i+=2){
        string option(argv[i]);
        /*
        * reading various parameters from command line arguments
        */
        if (option == "-N"){
            if (sscanf(argv[i+1],"%d",&switchPortCount) != 1){
                cout << argv[i] << " " << argv[i+1] << endl;
                cout << "Wrong input given (1)" << endl;
                return 1;
            }
            if (!knockoutSet){
                knockout = 0.6*switchPortCount; //changing K with respect to N
            }
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
            knockoutSet = true;
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
    ofstream outfile;
    outfile.open(outputFile, ios_base::app);
    if (queueType == 0){
        vector <InqInputPort> inputPorts (switchPortCount, InqInputPort (switchPortCount, bufferSize, packetGenProb));
        vector <int> delays;
        
        for (int i=0; i<maxTimeSlots; i++){
            /*
            *   generating packets at each input port in a time slot
            */
            for (int j=0; j<switchPortCount; j++){
                inputPorts[j].generatePacket();
            }
            /*
            *   scheduling and transmitting packets in a time slot
            */
            scheduleInq(inputPorts, delays, i, switchPortCount);
        }
        /* 
        *   Avg. Delay, Standard Deviation of Delay and Link Utilization calculations
        */
        float delaySum = 0, delaySqSum = 0;
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
        /*
        * output results to file
        */
        outfile << switchPortCount << "\t";
        outfile << packetGenProb << "\t";
        outfile << "INQ" << "\t";
        outfile << averageDelay << "\t";
        outfile << sdDelay << "\t";
        outfile << linkUtil << endl;
    }
    else if (queueType == 1){
        vector <KouqInputPort> inputPorts (switchPortCount, KouqInputPort (switchPortCount, packetGenProb));
        vector <int> delays;
        queue<int> outQ;
        /*
        *   creating output queues for each output port
        */
        vector <queue <int>> outputQs(switchPortCount, outQ);
        int kouqDrops=0;
        for (int i=0; i<maxTimeSlots; i++){
            /*
            *   generating packets at each input port in a time slot
            */
            for (int j=0; j<switchPortCount; j++){
                inputPorts[j].generatePacket();
            }
            /*
            *   scheduling and transmitting packets in a time slot
            */
            scheduleKouq(inputPorts, outputQs, kouqDrops, switchPortCount, bufferSize, knockout);
            transmitKouq(outputQs, delays, i, switchPortCount);
        }
        /* 
        *   Avg. Delay, Standard Deviation of Delay, KOUQ Drop Probability and Link Utilization calculations
        */
        float delaySum = 0, delaySqSum = 0;
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
        /*
        * output results to file
        */
        outfile << switchPortCount << "\t";
        outfile << packetGenProb << "\t";
        outfile << "KOUQ" << "\t";
        outfile << averageDelay << "\t";
        outfile << sdDelay << "\t";
        outfile << linkUtil << "\t";
        outfile << kouqDropProb << endl;
    }
    else if (queueType == 2){
        vector <IslipInputPort> inputPorts(switchPortCount, IslipInputPort(switchPortCount, bufferSize, packetGenProb));
        vector<vector<bool>> requests(switchPortCount, vector<bool>(switchPortCount, false));
        vector<int> delays;
        for (int i=0; i<maxTimeSlots; i++){
            /*
            *   generating packets at each input port in a time slot
            */
            for (int j=0; j<switchPortCount; j++){
                inputPorts[j].generatePacket();
            }
            /*
            *   scheduling and transmitting packets in a time slot
            */
            scheduleIslip(inputPorts, requests, delays, i, switchPortCount);
        }
        /* 
        *   Avg. Delay, Standard Deviation of Delay and Link Utilization calculations
        */
        float delaySum = 0, delaySqSum = 0;
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
        /*
        * output results to file
        */
        cout << "Avg. Delay: " << averageDelay << endl;
        cout << "SD Delay: " << sdDelay << endl;
        cout << "Link Utilization: " << linkUtil <<endl;
        outfile << switchPortCount << "\t";
        outfile << packetGenProb << "\t";
        outfile << "ISLIP" << "\t";
        outfile << averageDelay << "\t";
        outfile << sdDelay << "\t";
        outfile << linkUtil << endl;
    }
    outfile.close();

    return 0;
}