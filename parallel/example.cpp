#include <iostream>
#include <chrono>
#include <map>
#include "tbb/tbb.h"
#include <omp.h>

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

void multiTest2(char **argv) {
    //我先插入500个数，并发查剩下的数
    std::cout << "multi threaded:" << std::endl;

    //Lookup
    uint64_t n=1000;
    uint64_t* keys1=new uint64_t[n];
    uint64_t* keys2=new uint64_t[n];

    uint64_t position;
    cracker_column *cracker_map2;

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

                // if(keys1[i] ==  41518759){
                //     auto val2 = tree.lookup2(key3);

                // }
                // if(keys2[i] ==  41518759){
                //     auto val2 = tree.lookup2(key4);

                // }

               // tree.LookupLT(key3,first,second,t,keys1[i],cracker_map2,COLUMN_SIZE, position);

                 tree.insert(key3, keys1[i], t);
                
                //tree.LookupLT(key4,first2,second2,t,keys2[i],cracker_map2,COLUMN_SIZE, position);

               
                tree.insert(key4, keys2[i], t);

                // tree.LookupLT(key3,first,second,t,keys1[i]);
                // tree.LookupLT(key4,first2,second2,t,keys2[i]);

                
                // tree.insert(key3, keys1[i], t);
                // tree.insert(key4, keys2[i], t);
                
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



//毕设：多线程测试
void multiTest(char **argv) {
    //我先插入500个数，并发查剩下的数
    std::cout << "multi threaded:" << std::endl;

    // int tid = 1;
    // ART_OLC::N *node = reinterpret_cast<ART_OLC::N *>(tid | (static_cast<uint64_t>(1) << 63));
    // cout<<"sds"<<endl;
    // cout<<node->typeVersionLockObsolete<<endl;



    int THREADS = 4;

    //Lookup
    uint64_t n=1000;
    uint64_t* keys1=new uint64_t[n];
    uint64_t* keys2=new uint64_t[n];


    uint64_t COLUMN_SIZE = 100000000,NUM_QUERIES = 1000;
    uint64_t position;
    float QUERY_SELECTIVITY = 1;

    int s = COLUMN_SIZE / 100 * QUERY_SELECTIVITY; //查询的选择度为 1% ，即查询间隔s <--- 选择度*分裂列大小

    for(int i=0;i<COLUMN_SIZE-1;i++)
        rand();
    
    // create queries
    int WORKLOAD_PATTERN = 1;
	//int WORKLOAD_PATTERN = 2;
	//int WORKLOAD_PATTERN = 3;
	
    int j = COLUMN_SIZE / NUM_QUERIES;
    int maxLeftQueryVal = COLUMN_SIZE - s; //最大的左查询值
    if (WORKLOAD_PATTERN == 1) {
        for (int i = 0; i < NUM_QUERIES; i++) {
            keys1[i]=rand() % (COLUMN_SIZE - s);
            keys2[i]=keys1[i] + s;
        }
    }
	else if (WORKLOAD_PATTERN == 2) {
            for (int i = 0; i < NUM_QUERIES; i++) {
                keys1[i]= i * j + 1000001;
                if(i < 991){
                    keys2[i]=keys1[i] + s;
                }else{
                    keys2[i]=99999000 + i;
                }
				//queries[i] = std::make_pair(keys1[i], keys2[i]);
               
            }
    }
	else if (WORKLOAD_PATTERN == 3) {
            vector<int64_t> aux;
            generate_skewed_data(&aux, maxLeftQueryVal,COLUMN_SIZE,NUM_QUERIES);
            for (int i = 0; i < aux.size(); i++) {
                keys1[i]= aux[i];
                keys2[i]= keys1[i] + s;
            }
    } 

    typedef std::pair<int,int> cracker_column;

    IndexEntry *crackercolumn = (IndexEntry *) malloc(COLUMN_SIZE * 2 * sizeof( uint64_t));

    // cracker_column *cracker_map;
    // posix_memalign((void**)&cracker_map, 64, COLUMN_SIZE * sizeof(cracker_column));

    // init cracker column
    std::vector<int> column ;
    for(int i=0;i< COLUMN_SIZE;i++){
        column.push_back(i);
    }
    std::random_shuffle(column.begin(), column.end());
	
    for(int i=0;i<COLUMN_SIZE;i++){
        crackercolumn[i].m_key = column[i];
        crackercolumn[i].m_rowId = i;
    }

    // cracker_column* cracker_map2;
    // posix_memalign((void**)&cracker_map2, 64,COLUMN_SIZE * sizeof(cracker_map2));
    //cracker_map2 = (cracker_column*) malloc(COLUMN_SIZE * sizeof(cracker_column));
    IndexEntry *crackercolumn2 = (IndexEntry *) malloc(COLUMN_SIZE * 2 * sizeof( uint64_t));

	omp_set_dynamic(0);
	omp_set_num_threads(THREADS);

    int piece =COLUMN_SIZE / THREADS;
    int piece_r =COLUMN_SIZE % THREADS;
    // 并行的复制分裂列
    #pragma omp parallel for   
	for(int i = 0; i < THREADS; ++i){
		memcpy(crackercolumn2 + (i * piece), crackercolumn + (i * piece), (i == THREADS - 1 ? piece + piece_r : piece) * sizeof(cracker_column));
	}

    // 并行的分裂
    ART_OLC::Tree tree(loadKey);

    //ART_ROWEX::Tree tree(loadKey);
	
	TotalTime query_times;
    query_times.Initialize(NUM_QUERIES);
	chrono::time_point<chrono::system_clock> start, end;


    
    //设置tbb的线程数
    tbb::task_scheduler_init init(THREADS);
	
	{
         //Lookup
        n = 1000;
        ART_OLC::N  *first = nullptr;
        ART_OLC::N  *second = nullptr;

        ART_OLC::N  *first2 = nullptr;
        ART_OLC::N  *second2 = nullptr;

 
        double sum = 0;
        
        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
				
				start = chrono::system_clock::now();
				
                Key key3;
                Key key4;
            
                loadKey(keys1[i],key3);
                loadKey(keys2[i],key4);
                first = nullptr;
                second = nullptr;
                first2 = nullptr;
                second2 = nullptr;

                //cout<<"------"<<keys1[i]<<"--------"<<endl;

                //N* LookupLT(const Key &k, N *&first,N *&second,ThreadInfo &threadInfo) const;

                // tree.LookupLT(key3,first,second,t);
                
                // cout<<keys1[i]<<" "<< ART_OLC::N::getLeaf(first)<<" "<<ART_OLC::N::getLeaf(second)<<endl;
                // //cout<<"------"<<keys2[i]<<"--------"<<endl;
                // tree.LookupLT(key4,first2,second2,t);
                // cout<<keys2[i]<<" "<< ART_OLC::N::getLeaf(first2)<<" "<<ART_OLC::N::getLeaf(second2)<<endl;

                tree.LookupLT(key3,first,second,t,keys1[i],crackercolumn2,COLUMN_SIZE, position);
                tree.insert(key3, keys1[i], t);
                tree.LookupLT(key4,first2,second2,t,keys2[i],crackercolumn2,COLUMN_SIZE, position);
                tree.insert(key4, keys2[i], t);
				
				end = chrono::system_clock::now();
				
				sum += chrono::duration<double>(end - start).count();
				query_times.scan_time[i] = sum;
                
            }
        });

		
		//cout<<duration.count()/1000<<endl;
        // printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());
		
	     cout<<sum<<endl;
		
	        
		/* for (size_t i = 0; i < NUM_QUERIES; i++) {
			//sum of index_insert_time and lookup_time
			cout << query_times.scan_time[i] <<endl;
		} */


    }

    /* {
         //Lookup
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
                first = nullptr;
                second = nullptr;
                first2 = nullptr;
                second2 = nullptr;

                //cout<<"------"<<keys1[i]<<"--------"<<endl;

                //N* LookupLT(const Key &k, N *&first,N *&second,ThreadInfo &threadInfo) const;

                // tree.LookupLT(key3,first,second,t);
                
                // cout<<keys1[i]<<" "<< ART_OLC::N::getLeaf(first)<<" "<<ART_OLC::N::getLeaf(second)<<endl;
                // //cout<<"------"<<keys2[i]<<"--------"<<endl;
                // tree.LookupLT(key4,first2,second2,t);
                // cout<<keys2[i]<<" "<< ART_OLC::N::getLeaf(first2)<<" "<<ART_OLC::N::getLeaf(second2)<<endl;

                tree.LookupLT(key3,first,second,t,keys1[i],crackercolumn2,COLUMN_SIZE, position);
                tree.insert(key3, keys1[i], t);
                tree.LookupLT(key4,first2,second2,t,keys2[i],crackercolumn2,COLUMN_SIZE, position);
                tree.insert(key4, keys2[i], t);

                
            }
        });
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - starttime);
		
		cout<<duration.count()/1000<<endl;
        printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());


    } */

    delete[] keys1;
    delete[] keys2;



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
     
}




//毕设：单线程测试
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
                //cout<<val <<" "<<keys[i]<<endl;
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

    // {
    //     //test insert 的 parent 是否正确
    //     uint64_t c = 1;
    //     Key key2;
    //     cout<<"--------syddsb--------"<<endl;
    //     loadKey(c, key2);
    //     auto val2 = tree.lookup2(key2);
    //     //cout<<"val2 = "<<val2<<endl;
    //     if (val2 != 1) {
    //             std::cout << "wrong key read: " << val2 << " expected:" <<1 << std::endl;
    //             throw;
    //     }


    // }
     
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

            // for(int i=0;i<key.getKeyLen();i++){
            //     cout<<int(key[i])<<" ";
            // }
            // cout<<endl;

            tree.insert(key, keys[i]);//插入的是转化后的key以及正常的value
            //cout<<endl;//插入一个旧换行输出
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
            //cout<<val<<endl;
            if (val != keys[i]) {
                std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
                throw;
            }
        }

        uint64_t c = 125;
        Key key2;
        cout<<"--------syddsb--------"<<endl;
        loadKey(c, key2);
        auto val2 = tree.lookup2(key2);
        cout<<val2<<endl;
        if (val2 != 125) {
                std::cout << "wrong key read: " << val2 << " expected:" <<125 << std::endl;
                throw;
        }
        // cout<<"sbsb"<<endl;

    }

    

    // {
    //     auto starttime = std::chrono::system_clock::now();

    //     for (uint64_t i = 0; i != n; i++) {
    //         Key key;
    //         loadKey(keys[i], key);
    //         tree.remove(key, keys[i]);
    //     }
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
    //             std::chrono::system_clock::now() - starttime);
    //     printf("remove,%ld,%f\n", n, (n * 1.0) / duration.count());
    // }
    delete[] keys;

    cout<<"sbsb"<<endl;

    //std::cout << std::endl;
}





//单线程测试我写的算法

int main(int argc, char **argv) {
    
    //singleTest(argv);
     multiTest(argv);
    //multiTest2(argv);
   
   
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




