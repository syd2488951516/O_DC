#include <iostream>
#include <chrono>
#include <map>
#include "tbb/tbb.h"

using namespace std;

#include "OptimisticLockCoupling/Tree.h"
#include "ROWEX/Tree.h"
#include "ART/Tree.h"



//将key转化为1个8位的字符串
void loadKey(TID tid, Key &key) {
    // Store the key of the tuple into the key vector 将元组的键存储到键向量中
    // Implementation is database specific
    key.setKeyLen(sizeof(tid));
    reinterpret_cast<uint64_t *>(&key[0])[0] = __builtin_bswap64(tid);
}

//毕设：多线程测试
void multiTest(char **argv) {
    //我先插入500个数，并发查剩下的数
    std::cout << "multi threaded:" << std::endl;

    //Lookup
    uint64_t n=1000;
    uint64_t* keys1=new uint64_t[n];
    uint64_t* keys2=new uint64_t[n];

    int COLUMN_SIZE = 100000000,NUM_QUERIES = 1000;
    float QUERY_SELECTIVITY = 1;

    int s = COLUMN_SIZE / 100 * QUERY_SELECTIVITY; //查询的选择度为 1% ，即查询间隔s <--- 选择度*分裂列大小

    for(int i=0;i<COLUMN_SIZE-1;i++)
        rand();

    int WORKLOAD_PATTERN = 1;
    int j = COLUMN_SIZE / NUM_QUERIES;
    int maxLeftQueryVal = COLUMN_SIZE - s; //最大的左查询值
    if (WORKLOAD_PATTERN == 1) {
        for (int i = 0; i < NUM_QUERIES; i++) {
            keys1[i]=rand() % (COLUMN_SIZE - s);
            keys2[i]=keys1[i] + s;
        }
    }

    // cout<<keys1[500]<<endl;
    // cout<<keys1[501]<<endl;

    ART_OLC::Tree tree(loadKey);
    //ART_ROWEX::Tree tree(loadKey);


    // Build tree 看的差不多了 （并发插入）
    // {
    //     n = 0;
    //     auto starttime = std::chrono::system_clock::now();
    //     tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
    //         auto t = tree.getThreadInfo();
    //         for (uint64_t i = range.begin(); i != range.end(); i++) {
    //             Key key1,key2;
    //             loadKey(keys1[i], key1);
    //             tree.insert(key1, keys1[i], t);
    //             loadKey(keys2[i], key1);
    //             tree.insert(key1, keys2[i], t);
    //         }
    //     });
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
    //             std::chrono::system_clock::now() - starttime);
    //     printf("insert,%ld,%f\n", n, (n * 1.0) / duration.count());
    // }


    {

         //Lookup
        tbb::task_scheduler_init init(2);
        n = 1000;
        ART_OLC::N  *first = nullptr;
        ART_OLC::N  *second = nullptr;

        ART_OLC::N  *first2 = nullptr;
        ART_OLC::N  *second2 = nullptr;

 

        auto starttime = std::chrono::system_clock::now();
        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key3;
                Key key4;
            
                loadKey(keys1[i],key3);
                loadKey(keys2[i],key4);

                //cout<<"------"<<keys1[i]<<"--------"<<endl;

                //N* LookupLT(const Key &k, N *&first,N *&second,ThreadInfo &threadInfo) const;

                // tree.LookupLT(key3,first,second,t);
                
                // cout<<keys1[i]<<" "<< ART_OLC::N::getLeaf(first)<<" "<<ART_OLC::N::getLeaf(second)<<endl;
                // //cout<<"------"<<keys2[i]<<"--------"<<endl;
                // tree.LookupLT(key4,first2,second2,t);
                // cout<<keys2[i]<<" "<< ART_OLC::N::getLeaf(first2)<<" "<<ART_OLC::N::getLeaf(second2)<<endl;

                tree.LookupLT(key3,first,second,t,keys1[i]);

                tree.insert(key3, keys1[i], t);
                tree.LookupLT(key4,first2,second2,t,keys2[i]);

                
                //tree.insert(key3, keys1[i], t);
                tree.insert(key4, keys2[i], t);
                
                first = nullptr;
                second = nullptr;

                first2 = nullptr;
                second2 = nullptr;
               
            }
        });
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - starttime);
        printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());

        


    }

    // {
    //     // Lookup 并发查找
    //     auto starttime = std::chrono::system_clock::now();
    //     tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
    //         auto t = tree.getThreadInfo();
    //         for (uint64_t i = range.begin(); i != range.end(); i++) {
    //             Key key;
    //             loadKey(keys[i], key);
    //             auto val = tree.lookup(key, t);
    //             if (val != keys[i]) {
    //                 std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
    //                 throw;
    //             }
    //         }
    //     });
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
    //             std::chrono::system_clock::now() - starttime);
    //     printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());
    // }
     
    delete[] keys1;
    delete[] keys2;
}




//毕设：单线程测试
/*
void singleTest(char **argv){
    {
        ART_unsynchronized::Tree tree(loadKey);//开始就为tree 申请了一个N256的节点
        //Lookup
        uint64_t n=1000;
        uint64_t* keys1=new uint64_t[n];
        uint64_t* keys2=new uint64_t[n];

        int COLUMN_SIZE = 100000000,NUM_QUERIES = 1000;
        float QUERY_SELECTIVITY = 1;

        int s = COLUMN_SIZE / 100 * QUERY_SELECTIVITY; //查询的选择度为 1% ，即查询间隔s <--- 选择度*分裂列大小

        for(int i=0;i<COLUMN_SIZE-1;i++)
            rand();

        int WORKLOAD_PATTERN = 1;
        int j = COLUMN_SIZE / NUM_QUERIES;
        int maxLeftQueryVal = COLUMN_SIZE - s; //最大的左查询值
        if (WORKLOAD_PATTERN == 1) {
            for (int i = 0; i < NUM_QUERIES; i++) {
                keys1[i]=rand() % (COLUMN_SIZE - s);
                keys2[i]=keys1[i] + s;
            }
        }
        
        Key key3;
        Key key4;

         for(uint64_t i=0;i<1000;i++){
            cout<<"---------第"<<i+1<<"次--------------"<<endl;
            cout<<"----------lookupLT---------"<<endl;
            cout<<"range: "<<keys1[i]<<" "<<keys2[i]<<endl;

        
            loadKey(keys1[i],key3);
            for(int i=0;i<8;i++)
                cout<<+(key3[i])<<" ";
            cout<<endl;
            loadKey(keys2[i],key4);
            for(int i=0;i<8;i++)
                cout<<+(key4[i])<<" ";
            cout<<endl;

            cout<<"----------first Lookup--------"<<endl;
            //start_insert = chrono::system_clock::now();
            tree.LookupLT(key3);
            cout<<"----------second Lookup-------"<<endl;
            tree.LookupLT(key4);
            //end_insert = chrono::system_clock::now();
            //cout<<chrono::duration<double>(end_insert - start_insert).count()<<endl;
            //cout<<endl;

            cout<<"----------first insert----------"<<endl;
            tree.insert(key3, keys1[i]);
            cout<<"----------second insert--------"<<endl;
            tree.insert(key4, keys2[i]);
            cout<<endl;

       }

        // uint64_t c = 45570486;
        // Key key2;
        // cout<<"--------syddsb--------"<<endl;
        // loadKey(c, key2);
        // auto val2 = tree.lookup2(key2);
        // if (val2 != 45570486) {
        //         std::cout << "wrong key read: " << val2 << " expected:" << 45570486 << std::endl;
        //         throw;
        // }
    }
    
}
*/


void exchange(IndexEntry *&c, int64_t x1, int64_t x2) {
    IndexEntry tmp = *(c + x1);
    *(c + x1) = *(c + x2);
    *(c + x2) = tmp;
}

int crackInTwoItemWise(IndexEntry *&c, int64_t posL, int64_t posH, int64_t med) {
    int x1 = posL, x2 = posH;
    while (x1 <= x2) {
        if (c[x1] < med)
            x1++;
        else {
            while (x2 >= x1 && (c[x2] >= med))
                x2--;
            if (x1 < x2) {
                exchange(c, x1, x2);
                x1++;
                x2--;
            }
        }
    }
    if (x1 < x2)
        printf("Not all elements were inspected!");
    x1--;
    if (x1 < 0)
        x1 = 0;
    return x1;
}

IntPair crackInThreeItemWise(IndexEntry *&c, int64_t posL, int64_t posH, int64_t low, int64_t high) {
    int x1 = posL, x2 = posH;
    while (x2 > x1 && c[x2] >= high)
        x2--;
    int x3 = x2;
    while (x3 > x1 && c[x3] >= low) {
        if (c[x3] >= high) {
            exchange(c, x2, x3);
            x2--;
        }
        x3--;
    }
    while (x1 < x3) {
        if (c[x1] < low)
            x1++;
        else {
            exchange(c, x1, x3);
            while (x3 > x1 && c[x3] >= low) {
                if (c[x3] >= high) {
                    exchange(c, x2, x3);
                    x2--;
                }
                x3--;
            }
        }
    }
    IntPair p = (IntPair) malloc(sizeof(struct int_pair));
    p->first = x3;
    p->second = x2;
    return p;
}


int64_t scanQuery(IndexEntry *c, uint64_t from, uint64_t to) {
    uint64_t sum = 0;
    for (uint64_t i = from; i <= to; i++) {
        sum += c[i].m_key;
    }

    return sum;
}

//偏斜负载function1
int64_t zipf(double alpha, int64_t n) {
    static int first = true;      // Static first time flag
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value = 0.0;      // Computed exponential value to be returned
    int64_t i;                     // Loop counter

    // Compute normalization constant on first call only
    if (first == true) {
        for (i = 1; i <= n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    // Pull a uniform random number (0 < z < 1)
    do {
        z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    } while ((z == 0) || (z == 1));

    // Map z to the value
    sum_prob = 0;
    for (i = 1; i <= n; i++) {
        sum_prob = sum_prob + c / pow((double) i, alpha);
        if (sum_prob >= z) {
            zipf_value = i;
            break;
        }
    }

    // Assert that zipf_value is between 1 and N
    assert((zipf_value >= 1) && (zipf_value <= n));

    return zipf_value;
}

//偏斜负载function2
void generate_skewed_data(vector<int64_t> *data, int64_t maxLeftQueryVal, int64_t COLUMN_SIZE, int64_t NUM_QUERIES) {
    // the focus should be in the center of the dataset
    int64_t hotspot = COLUMN_SIZE / 2;

    // compute zipf distribution
    typedef map<int64_t, int64_t> result_t;
    typedef result_t::iterator result_iterator_t;
    float ZIPF_ALPHA = 2.0;

    result_t result;
    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        int64_t nextValue = zipf(ZIPF_ALPHA, COLUMN_SIZE);
        result_iterator_t it = result.find(nextValue);
        if (it != result.end()) {
            ++it->second;
        } else {
            result.insert(make_pair(nextValue, 1));
        }
    }

    int64_t zoneSize = hotspot / result.size();

    int64_t zone = 0;
    for (result_iterator_t it = result.begin(); it != result.end(); ++it) {
        for (int i = 0; i < it->second; ++i) {
            int64_t direction = rand() % 2 == 0 ? 1 : -1;
            int64_t zoneBegin = hotspot + (zone * zoneSize * direction);
            int64_t zoneEnd = zoneBegin + (zoneSize * direction);
            if (direction == -1) {
                int64_t tmp = zoneBegin;
                zoneBegin = zoneEnd;
                zoneEnd = tmp;
            }
            int64_t predicate = rand() % (zoneEnd - zoneBegin + 1) + zoneBegin;
            while (predicate > maxLeftQueryVal) {
                direction = rand() % 2 == 0 ? 1 : -1;
                zoneBegin = hotspot + (zone * zoneSize * direction);
                zoneEnd = zoneBegin + (zoneSize * direction);
                if (direction == -1) {
                    int64_t tmp = zoneBegin;
                    zoneBegin = zoneEnd;
                    zoneEnd = tmp;
                }
                predicate = rand() % (zoneEnd - zoneBegin + 1) + zoneBegin;
            }
            data->push_back(predicate);
        }
        ++zone;
    }
    random_shuffle(data->begin(), data->end());

}





//毕设：单线程测试2
void singleTest2(char **argv){
    {
        ART_unsynchronized::Tree tree(loadKey);//开始就为tree 申请了一个N256的节点
        //Lookup



        uint64_t n=1000;
        uint64_t* keys1=new uint64_t[n];
        uint64_t* keys2=new uint64_t[n];

        uint64_t COLUMN_SIZE = 100000000,NUM_QUERIES = 1000;
        float QUERY_SELECTIVITY = 1;

        int s = COLUMN_SIZE / 100 * QUERY_SELECTIVITY; //查询的选择度为 1% ，即查询间隔s <--- 选择度*分裂列大小

        /* for(int i=0;i<COLUMN_SIZE-1;i++)
            rand(); */
		
		
		TotalTime query_times;
		query_times.Initialize(NUM_QUERIES);
        chrono::time_point<chrono::system_clock> start, end;
			
		typedef std::pair<int,int> cracker_column;


        IndexEntry *crackercolumn = (IndexEntry *) malloc(COLUMN_SIZE * 2 * sizeof( uint64_t));

        // init cracker column
        std::vector<int> column ;
        for(int i=0;i< COLUMN_SIZE;i++){
            column.push_back(i);
        }
        std::random_shuffle(column.begin(), column.end());
        
		start = chrono::system_clock::now();
        for(int i=0;i<COLUMN_SIZE;i++){
            crackercolumn[i].m_key = column[i];
            crackercolumn[i].m_rowId = i;
        }
		//cout<<"crack column:"<<column[1]<<" "<<column[2]<<endl;
		end = chrono::system_clock::now();
        query_times.swap_time[0] += chrono::duration<double>(end - start).count();

        int WORKLOAD_PATTERN = 1;
		//int WORKLOAD_PATTERN = 2;
		//int WORKLOAD_PATTERN = 3;
        int j = COLUMN_SIZE / NUM_QUERIES;
        int maxLeftQueryVal = COLUMN_SIZE - s; //最大的左查询值
		
        // if (WORKLOAD_PATTERN == 1) {
            // for (int i = 0; i < NUM_QUERIES; i++) {
                // keys1[i]=rand() % (COLUMN_SIZE - s);
                // keys2[i]=keys1[i] + s;
            // }
        // }
		
		if (WORKLOAD_PATTERN == 1) {
            for (int i = 0; i < NUM_QUERIES; i++) {
                keys1[i]=rand() % (COLUMN_SIZE - s);
                keys2[i]=keys1[i] + s;
            }
        }else if (WORKLOAD_PATTERN == 2) {
            for (int i = 0; i < NUM_QUERIES; i++) {
                keys1[i]= i * j +1;
                keys2[i]=keys1[i] + s;
            }
        }else if (WORKLOAD_PATTERN == 3) {
            vector<int64_t> aux;
            generate_skewed_data(&aux, maxLeftQueryVal,COLUMN_SIZE,NUM_QUERIES);
            for (int i = 0; i < aux.size(); i++) {
                keys1[i]= aux[i];
                keys2[i]= keys1[i] + s;
            }
       } 
		
		

       

        
        // ART_unsynchronized::N *first = nullptr;
        // ART_unsynchronized::N *second = nullptr;
        // ART_unsynchronized::N *first2 = nullptr;
        // ART_unsynchronized::N *second2 = nullptr;

        IntPair p1, p2;




        Key key3;
        Key key4;

        
        

        for(uint64_t i=0;i<NUM_QUERIES;i++){
            //cout<<"---------第"<<i+1<<"次--------------"<<endl;
            //cout<<"----------lookupLT---------"<<endl;
            //cout<<"range: "<<keys1[i]<<" "<<keys2[i]<<endl;

        
            loadKey(keys1[i],key3);
            loadKey(keys2[i],key4);

        
            start = chrono::system_clock::now();
            p1 = tree.LookupLT(key3,COLUMN_SIZE-1);
            //cout<<"----------second Lookup-------"<<endl;
            p2 = tree.LookupLT(key4,COLUMN_SIZE-1);

            end = chrono::system_clock::now();
            query_times.lookup_time[i] += chrono::duration<double>(end - start).count();
            //end_insert = chrono::system_clock::now();
            //cout<<chrono::duration<double>(end_insert - start_insert).count()<<endl;
            //cout<<endl;


            IntPair pivot_pair = NULL;
			start = chrono::system_clock::now();
            if (p1->first == p2->first && p1->second == p2->second) {
                pivot_pair = crackInThreeItemWise(crackercolumn, p1->first, p1->second, keys1[i],keys2[i]);
            } else {
                // crack in two
                pivot_pair = (IntPair) malloc(sizeof(struct int_pair));
                pivot_pair->first = crackInTwoItemWise(crackercolumn, p1->first, p1->second, keys1[i]);
                pivot_pair->second = crackInTwoItemWise(crackercolumn, pivot_pair->first, p2->second, keys2[i]);
            } 
			end = chrono::system_clock::now();
            query_times.swap_time[i]+= chrono::duration<double>(end - start).count();
			


            start = chrono::system_clock::now();
            //cout<<"----------first insert----------"<<endl;
            tree.insert(key3, keys1[i]);
            //cout<<"----------second insert--------"<<endl;
            tree.insert(key4, keys2[i]);
            end = chrono::system_clock::now();
            query_times.index_insert_time[i]+= chrono::duration<double>(end - start).count();

            start = chrono::system_clock::now();
            uint64_t val = tree.lookup(key3);
            uint64_t val2 = tree.lookup(key4);
            end = chrono::system_clock::now();
            query_times.lookup_time[i] += chrono::duration<double>(end - start).count();
			
			
            start = chrono::system_clock::now();
            int64_t sum = scanQuery(crackercolumn, val, val2-1);
            end = chrono::system_clock::now();
            query_times.scan_time[i] += chrono::duration<double>(end - start).count();



           // cout<<endl;

       }

      /*   //使用索引的时间
		for (size_t i = 0; i < NUM_QUERIES; i++) {
			//sum of index_insert_time and lookup_time
			cout << query_times.index_insert_time[i]  + query_times.lookup_time[i]  <<endl;
		} */
		
		
		/* //每次分裂的花费pre time
		for (size_t i = 0; i < NUM_QUERIES; i++) {
			//sum of index_insert_time and lookup_time
			cout << query_times.index_insert_time[i]  + query_times.lookup_time[i] 
					 + query_times.swap_time[i] + query_times.scan_time[i]<<endl;
		} */
		
		/* //测试第一次为什么比较块
		for (size_t i = 0; i < NUM_QUERIES; i++) {
			//sum of index_insert_time and lookup_time
			cout << query_times.index_insert_time[i]<<","<<query_times.lookup_time[i]<<","
				 << query_times.swap_time[i]<<","<<query_times.scan_time[i]<<endl;

		} */
		
	   //测试总的时间
		double sum = 0;
		for (size_t i = 0; i < NUM_QUERIES; i++) {
			//sum of index_insert_time and lookup_time
			sum += query_times.index_insert_time[i]  + query_times.lookup_time[i]  
					 + query_times.swap_time[i]+ query_times.scan_time[i];
		}
		cout<<sum<<endl;
		
		
		
		
		

        // uint64_t c = 45570486;
        // Key key2;
        // cout<<"--------syddsb--------"<<endl;
        // loadKey(c, key2);
        // auto val2 = tree.lookup2(key2);
        // if (val2 != 45570486) {
        //         std::cout << "wrong key read: " << val2 << " expected:" << 45570486 << std::endl;
        //         throw;
        // }
    }
    
}




void singlethreaded(char **argv) {
    std::cout << "single threaded:" << std::endl;

    uint64_t n = std::atoll(argv[1]);
    uint64_t *keys = new uint64_t[n];

    // Generate keys
    for (uint64_t i = 0; i < n; i++)
        // dense, sorted
        keys[i] = i + 1;
    if (atoi(argv[2]) == 1)
        // dense, random
        std::random_shuffle(keys, keys + n);
    if (atoi(argv[2]) == 2)
        // "pseudo-sparse" (the most-significant leaf bit gets lost)
        for (uint64_t i = 0; i < n; i++)
            keys[i] = (static_cast<uint64_t>(rand()) << 32) | static_cast<uint64_t>(rand());

    printf("operation,n,ops/s\n");
    ART_unsynchronized::Tree tree(loadKey);//开始就为tree 申请了一个N256的节点

    // Build tree
    {
        auto starttime = std::chrono::system_clock::now();
        for (uint64_t i = 0; i != n; i++) {
            Key key;
            loadKey(keys[i], key);

            for(int i=0;i<key.getKeyLen();i++){
                cout<<int(key[i])<<" ";
            }
            cout<<endl;

            tree.insert(key, keys[i]);//插入的是转化后的key以及正常的value
            cout<<endl;//插入一个旧换行输出
        }
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - starttime);
        printf("insert,%ld,%f\n", n, (n * 1.0) / duration.count());
    }

    {
        // Lookup
        auto starttime = std::chrono::system_clock::now();
        for (uint64_t i = 0; i != n; i++) {
            Key key;
            loadKey(keys[i], key);
            auto val = tree.lookup(key);
            if (val != keys[i]) {
                std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
                throw;
            }
        }

        uint64_t c = 257;
        Key key2;
        cout<<"--------syddsb--------"<<endl;
        loadKey(c, key2);
        auto val2 = tree.lookup2(key2);
        if (val2 != 257) {
                std::cout << "wrong key read: " << val2 << " expected:" << 257 << std::endl;
                throw;
        }

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now() - starttime);
        printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());
    }

    

    {
        auto starttime = std::chrono::system_clock::now();

        for (uint64_t i = 0; i != n; i++) {
            Key key;
            loadKey(keys[i], key);
            tree.remove(key, keys[i]);
        }
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - starttime);
        printf("remove,%ld,%f\n", n, (n * 1.0) / duration.count());
    }
    delete[] keys;

    std::cout << std::endl;
}



void multithreaded(char **argv) {
    std::cout << "multi threaded:" << std::endl;

    uint64_t n = std::atoll(argv[1]);
    uint64_t *keys = new uint64_t[n];

    // Generate keys
    for (uint64_t i = 0; i < n; i++)
        // dense, sorted
        keys[i] = i + 1;
    if (atoi(argv[2]) == 1)
        // dense, random
        std::random_shuffle(keys, keys + n);
    if (atoi(argv[2]) == 2)
        // "pseudo-sparse" (the most-significant leaf bit gets lost)
        for (uint64_t i = 0; i < n; i++)
            keys[i] = (static_cast<uint64_t>(rand()) << 32) | static_cast<uint64_t>(rand());

    printf("operation,n,ops/s\n");
    ART_OLC::Tree tree(loadKey);
    //ART_ROWEX::Tree tree(loadKey);


    // Build tree 看的差不多了 （并发插入）
    {
        auto starttime = std::chrono::system_clock::now();
        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key;
                loadKey(keys[i], key);
                tree.insert(key, keys[i], t);
            }
        });
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - starttime);
        printf("insert,%ld,%f\n", n, (n * 1.0) / duration.count());
    }

    {
        // Lookup 并发查找
        auto starttime = std::chrono::system_clock::now();
        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key;
                loadKey(keys[i], key);
                auto val = tree.lookup(key, t);
                if (val != keys[i]) {
                    std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
                    throw;
                }
            }
        });
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - starttime);
        printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());
    }

    {
        //test insert 的 parent 是否正确
        uint64_t c = 1;
        Key key2;
        cout<<"--------syddsb--------"<<endl;
        loadKey(c, key2);
        auto val2 = tree.lookup2(key2);
        if (val2 != 1) {
                std::cout << "wrong key read: " << val2 << " expected:" << 1 << std::endl;
                throw;
        }


    }
     
    //
    // {
    //     //Range
    //     auto starttime = std::chrono::system_clock::now();
    //     tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n-100), [&](const tbb::blocked_range<uint64_t> &range) {
    //         auto t = tree.getThreadInfo();
    //         for (uint64_t i = range.begin(); i != range.end(); i++) {
    //             Key keystart;
    //             Key keyend;
    //             Key continueKey;
            
    //             loadKey(keys[i], keystart);
    //             loadKey(keys[i]+100, keyend);
    //             loadKey(0, continueKey);//?
    //             //查询
    //             uint64_t *result = new uint64_t[100];
    //             std::size_t resultSize = 100;//参数
    //             std::size_t resultsFound  = 0;
                
    //             //感觉有2个参数没有用
    //             bool flag = tree.lookupRange(keystart,keyend,continueKey,result,resultSize,resultsFound,t);
    //             cout<<keys[i]<<" "<<resultsFound<<endl;
    //             for(int i=0;i<resultsFound;i++){
    //                 cout<<result[i]<<" ";
    //             }
    //             cout<<endl;
    //             //cout<<endl;
               
    //         }
    //     });
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
    //             std::chrono::system_clock::now() - starttime);
    //     printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());

    // }
       
    // {
    //     //Remove 并发删除
    //     auto starttime = std::chrono::system_clock::now();

    //     tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
    //         auto t = tree.getThreadInfo();
    //         for (uint64_t i = range.begin(); i != range.end(); i++) {
    //             Key key;
    //             loadKey(keys[i], key);
    //             tree.remove(key, keys[i], t);
    //         }
    //     });
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
    //             std::chrono::system_clock::now() - starttime);
    //     printf("remove,%ld,%f\n", n, (n * 1.0) / duration.count());
    // }

    delete[] keys;
}

//单线程测试我写的算法

int main(int argc, char **argv) {
    
    singleTest2(argv);
    //multiTest(argv);
   
    return 0;
}


// 原 main 函数

// int main(int argc, char **argv) {

//     if (argc != 3) {
//         printf("usage: %s n 0|1|2\nn: number of keys\n0: sorted keys\n1: dense keys\n2: sparse keys\n", argv[0]);
//         return 1;
//     }

//     //singleTest(argv);
   
//     //singlethreaded(argv);

//     multithreaded(argv);

//     return 0;
// }