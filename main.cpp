#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <thread>
#include <shared_mutex>
#include <chrono>
#include <list>
#include <time.h>

using namespace std;

//复制的数据，便于对比前后变化
vector<int> dataCopy(100000);
// 1000 * 100

class Data{
public:
    //初始化函数。并设定part id
    Data(int id):part(id) {
        for(int i = 0; i < 100; i++) {
            data[i] = rand() % 10;
            //复制数据，方便查看区别
            dataCopy[100 * part + i] = data[i];
        }
    }

    //根据下标返回所需要的数据
    int GetData(int index) {
        return data[index];
    }

    //根据下标和数值，修改数据
    void SetData(int number, int index) {
        data[index] = number;
    }

    //展示数据
    void ShowData(){
        for(int i = 0; i < 100; i++) {
            cout << "id = " << this->part << "=======" <<  "原先的数据：index " << part * 100 + i << "    Data: " << dataCopy[part * 100 + i] << ";  现在数据为：" << data[i] << endl;
        }
    }

    int GetPart() {
        return part;
    }

private:
    int data[100];
    int part;
};

//哈希表：方便根据id定位所需要的part
unordered_map<int, Data*> map;
//vector<Data *> map;

class Worker{
public:
    void compute(int id) {
        cout << "线程ID：" << this_thread::get_id()  << "开始工作"<< endl;
        for(int time = 0; time < 100000; time++) {
            i = rand() % 100000;
            while (i >= 99998) {
                i = rand() % 100000;
            }
            // i / 1000
            // i % 100 余数
            mutex_.lock();
            a[0] = map[i / 100]->GetData(i % 100);
            a[1] = map[(i + 1) / 100]->GetData((i + 1) % 100);
            a[2] = map[(i + 2) / 100]->GetData((i + 2) % 100);
            mutex_.unlock();
            j = rand() % 100000;
            while(j - i >= 0 && j - i <= 2) {
                j = rand() % 100000;
            }
            a[0] = a[0] + a[1] + a[2];
            auto now = chrono::steady_clock::now();

            if(mutex_.try_lock_until(now + 1s)) {
                Data* target = map[j / 100];
                target->SetData(a[0], (j) % 100);
                mutex_.unlock();
            } else {
                cout << "此次加锁失败" << endl;
                this_thread::sleep_for(chrono::milliseconds(1000) );
                i--;
                continue;
            }
            /*
            mutex_.lock();
            a[0] = a[0] + a[1] + a[2];
            Data* target = map[j / 100];
            target->SetData(a[0], j % 100);
            mutex_.unlock();
            */
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
        map.insert(pair<int, Data *>(i, new Data (i)));
    }
    for(int i = 0; i < 1000; i++) {
        cout << "i = " << i << "==="<<map[i]->GetPart() << endl;
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


    for (int i = 0; i < 100000; i++) {
        cout << "i = " << i << "===" << "原先数据" << dataCopy[i] << "；    现在的数据" << map[(int)(i / 100)]->GetData(i % 100) << ";    属于的part : " << map[i / 100]->GetPart() <<endl;
    }

    return 0;
}
