#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <thread>
#include <shared_mutex>

#include <time.h>

#include <list>

using namespace std;

//数组
vector<int> data_(10000);
vector<int> dataCopy(10000);

class Worker{
public:

    void compute(int id) {
        cout << "线程ID：" << this_thread::get_id()  << "开始工作"<< endl;
        for(int time = 0; time < 10000; time++) {
            i = rand() % 10000;
            j = rand() % 10000;
            while(j - i >= 0 && j - i <= 2) {
                rand() % 10000;
            }
            mutex_.lock_shared();
            a[0] = data_[i];
            a[1] = data_[(i + 1) % 10000];
            a[2] = data_[(i + 2) % 10000];
            mutex_.unlock_shared();
            a[0] = a[0] + a[1] + a[2];
            mutex_.lock(); //互斥锁
            data_[j] = a[0];
            mutex_.unlock();
            cout << "id : " << id << ": "<< a[0] << ": "<< a[1] << ": "<< a[2]  << " 工作次数" <<  time << endl;
        }
    }

private:
    int i, j;
    int a[3]; //需要的数据
    shared_mutex mutex_;
};


int main() {
    //数据初始化
    cout << "线程ID：" << this_thread::get_id()  << "开始工作"<< endl;
    for(int i = 0; i < 10000; i++) {
        data_[i] = rand() % 100;
    }
    //复制数据，方便查看区别
    dataCopy = data_;
    Worker W1;
    Worker W2;
    //开启线程
    thread homework1(&Worker::compute, &W1, 1);
    thread homework2(&Worker::compute, &W2, 2);

    homework1.join();
    homework2.join();
    //对比数据
    for(int i = 0; i < 10000; i++) {
        cout <<"i = " << i << ";  原来数据：" << dataCopy[i] << ";  当前数据" << data_[i] << endl;
    }
    return 0;
}