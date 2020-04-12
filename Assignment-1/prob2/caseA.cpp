#include <bits/stdc++.h>

using namespace std;

class Officer{
    public:
        Officer(float mu){
            serviceRate = exponential_distribution<float> (mu);
        }
        queue <int> q;
        int totalPeopleWaiting=0;
        void service(int &totalPeopleServed, float &totalServiceTime, float &totalWaitTime){
            float curServiceRate = serviceRate(generator);
            serving += curServiceRate;
            float curServingTime = 0;
            while (!q.empty()){
                if (serving>=1){
                    int arrivalTime = q.front();
                    totalPeopleServed++;
                    totalServiceTime += 1/curServiceRate;
                    totalWaitTime += curTime-arrivalTime+curServingTime;
                    curServingTime += 1/curServiceRate;
                    serving--;
                    totalPeopleWaiting += q.size()-1;
                    q.pop();
                }
                else{
                    break;
                }
            }
            curTime++;
        }
    private:
        int curTime=0;
        float serving = 0;
        default_random_engine generator;
        exponential_distribution<float> serviceRate;
};

int main(){
    float lambda, mu;
    cout << "Enter Lambda: ";
    cin >> lambda;
    cout << "Enter Mu: ";
    cin >> mu;
    default_random_engine generator;
    poisson_distribution<int> arrival(lambda);
    Officer officers[3]={Officer(mu), Officer(mu), Officer(mu)};
    uniform_int_distribution<int> assign(0,2);
    float totalServiceTime=0;
    int totalPeopleServed=0;
    float totalWaitTime=0;
    for (int t=0; t<1000; t++){
        int N = arrival(generator);
        for (int n=0; n<N; n++){
            int officer_num = assign(generator);
            officers[officer_num].q.push(t);
        }
        for (int i=0; i<3; i++){
            officers[i].service(totalPeopleServed, totalServiceTime, totalWaitTime);
        }
    }
    cout << totalPeopleServed << endl;
    cout << totalServiceTime << endl;
    cout << totalWaitTime << endl;
    for (int i=0; i<3; i++){
        cout << officers[i].totalPeopleWaiting << endl;
    }
    return 0;
}