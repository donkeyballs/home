#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <thread>
#include <shared_mutex>
#include <chrono>
#include <list>

using namespace std;

vector<int> dataCopy(100000);

class Data{
public:
    Data(int id):part(id) {
        for(int i = 0; i < 100; i++) {
            data[i] = rand() % 10;
            //复制数据，方便查看区别
            dataCopy[100 * part + i] = data[i];
        }
    }

    int GetData(int index) {
        return data[index];
    }

    void SetData(int number, int index) {
        data[index] = number;
    }

    void ShowData(){
        for(int i = 0; i < 100; i++) {
            cout << "原先的数据：index " << part * 100 + i << "    Data: " << dataCopy[part * 100 + i] << ";  现在数据为：" << data[i] << endl;
        }
    }


private:
    int data[100];
    int part;
};

//数组


unordered_map<int, Data*> map;

class Worker{
public:
    void compute(int id) {
        cout << "线程ID：" << this_thread::get_id()  << "开始工作"<< endl;
        for(int time = 0; time < 100000; time++) {
            i = rand() % 100000;
            j = rand() % 100000;
            while(j - i >= 0 && j - i <= 2) {
                j = rand() % 100000;
            }
            // i / 100
            // i % 100 余数
            mutex_.lock_shared();
            a[0] = map[i / 1000]->GetData(i % 100);
            a[1] = map[(i + 1) / 1000]->GetData((i + 1) % 100);
            a[2] = map[(i + 2) / 1000]->GetData((i + 2) % 100);
            mutex_.unlock_shared();
            a[0] = a[0] + a[1] + a[2];
            Data* target = map[j / 1000];
            if(mutex_.try_lock_for(100ms)) {
                target->SetData(a[0], j);
                mutex_.unlock();
            } else {
                cout << "此次加锁失败" << endl;
                i--;
                continue;
            }
            cout << "id : " << id << "    数据："<< a[0] << ": "<< a[1] << ": "<< a[2]  << "   ；工作次数" <<  time + 1 << endl;
        }
    }

private:
    int i, j;
    int a[3]; //需要的数据
    shared_timed_mutex mutex_;

};


int main() {
    //数据初始化
    cout << "线程ID：" << this_thread::get_id()  << "开始工作"<< endl;
    //用哈希表映射所需要的数据块

    for(int i = 0; i < 1000; i++) {
        Data *temp = new Data(i);
        map[i] = temp;
    }


    Worker W1;
    Worker W2;
    //开启线程
    thread homework1(&Worker::compute, &W1, 1);
    thread homework2(&Worker::compute, &W2, 2);
    homework1.join();
    homework2.join();
    //对比数据
    string t;
    cout << "任意输入查看前后数据" << endl;
    cin >> t;
    for(int i = 0; i < 1000; i++) {
        map[i]->ShowData();
    }

    return 0;
}
