#include <assert.h>
#include <algorithm>
#include <iostream>
#include "Tree.h"
#include "../Epoche.cpp"
#include "N.cpp"


using namespace std;


namespace ART_unsynchronized {
    
    //直接给root初始化了一个N256节点
    Tree::Tree(LoadKeyFunction loadKey) : root(new N256(nullptr, 0)), loadKey(loadKey) {
    }

    Tree::~Tree() {
        N::deleteChildren(root);
        N::deleteNode(root);
    }

    TID Tree::lookup(const Key &k) const {
        N *node = nullptr;
        N *nextNode = root;
        uint32_t level = 0;
        bool optimisticPrefixMatch = false;

        while (true) {
            node = nextNode;
            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch:
                    return 0;
                case CheckPrefixResult::OptimisticMatch://目前没有用上
                    optimisticPrefixMatch = true;
                    // fallthrough
                case CheckPrefixResult::Match:
                    if (k.getKeyLen() <= level) {
                        return 0;
                    }
                    nextNode = N::getChild(k[level], node);

                    if (nextNode == nullptr) {
                        return 0;
                    }
                    if (N::isLeaf(nextNode)) {
                        TID tid = N::getLeaf(nextNode);
                        if (level < k.getKeyLen() - 1 || optimisticPrefixMatch) {//这一步是干嘛？？？
                            return checkKey(tid, k);
                        }
                        return tid;
                    }
                    level++;
            }
        }
    }


    N* findFirst(N* n,uint8_t keyByte,bool ss,uint32_t &level,const Key &k) {
        // Find the next child for the keyByte
        //cout<<"KeyByte = "<<+(keyByte)<<endl;
        switch (n->getType()) {
            case NTypes::N4: {
               // cout<<"findFirst Node 4"<<endl;
                auto node = static_cast<N4 *>(n);
                if(node->keys[0] == keyByte ){
                    // cout<<"yy"<<endl;
                    if(ss){
                        level = level-node->getPrefixLength()-1;
                        return findFirst(node->parent,k[level],ss,level,k);
                    }else{
                        //cout<<"dsd"<<endl;
                        return N::minimum(node->children[1]);
                    }
                   //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
                }
                unsigned i,j=-1;
                for(i=0;(i<node->count)&&(node->keys[i]<keyByte);i++)
                        j = i;
                if(ss)
                    return N::maximum(node->children[j]);//上层左边最大
                else
                    return  N::minimum(node->children[i+1]);//上层右边最小
                break;
            }
            case  NTypes::N16: {
               // cout<<"findFirst Node 16"<<endl;
                unsigned i,j=-1;
                auto node = static_cast<N16 *>(n);
                // cout<<+keyByte<<endl;
                // cout<<node->count<<endl;
                // for(i=0;i<node->count;i++)
                //    cout<<+(node->key[i]-128)<<" ";
                // cout<<endl;
                //先判断边界
                //cout<<"node->flipSign(node->keys[0]) = "<<+(node->flipSign(node->keys[0]))<<endl;
                if(node->flipSign(node->keys[0]) == keyByte ){
                    // cout<<"yy"<<endl;
                    if(ss){
                        level = level-node->getPrefixLength()-1;
                        return findFirst(node->parent,k[level],ss,level,k);
                    }else{
                        //cout<<"dsd"<<endl;
                        return N::minimum(node->children[1]);
                    }
                   //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
                }
                for(i=0;(i<node->count)&&(node->flipSign(node->keys[i])<keyByte);i++)// 第12次的时候刚好到达了边界
                        j = i;
                if(ss)
                    return N::maximum(node->children[j]);//上层左边最大
                else
                    return  N::minimum(node->children[i+1]);//上层右边最小
                
                // __m128i cmp=_mm_cmpeq_epi8(_mm_set1_epi8(flipSign(keyByte)),_mm_loadu_si128(reinterpret_cast<__m128i*>(node->key)));
                // unsigned bitfield=_mm_movemask_epi8(cmp)&((1<<node->count)-1);
                // if (bitfield)
                //    return &node->child[ctz(bitfield)]; else
                //    return &nullNode;
            }
            case NTypes::N48: {
                //cout<<"findFirst Node 48"<<endl;
                auto node = static_cast<N48 *>(n);
                if(node->minl == keyByte ){  //同样需要加右边界判断（没有完成）
                    // cout<<"yy"<<endl;
                    if(ss){
                        level = level-node->getPrefixLength()-1;
                        return findFirst(node->parent,k[level],ss,level,k);
                    }else{
                        cout<<"N48 bound not complete"<<endl;
                        ///return N::minimum(node->children[1]);
                    }
                   //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
                }
                unsigned pos1,pos2;
                pos1=pos2=keyByte;
                //cout<<pos2<<endl;
                //cout<<+key[depth-1]<<endl;
                if(ss){
                    //cout<<pos1<<endl;
                    while(node->childIndex[--pos1] == node->emptyMarker);
                    //cout<<pos1<<endl;
                    return N::maximum(node->children[node->childIndex[pos1]]);
                }else{
                    //cout<<pos2<<endl;
                    while(node->childIndex[++pos2] == node->emptyMarker);
                    return  N::minimum(node->children[node->childIndex[pos2]]);
                } 
                break;
            }
            case NTypes::N256: {  //需要测试
                //cout<<"find N256"<<endl;
                auto node = static_cast<N256 *>(n);
                unsigned pos1,pos2;
                // if(node->children[0] && node->parent == NULL){ //先随便写了一个判断
                //     return NULL;
                // }
                if(node->minl == keyByte){//最左边界判断
                   if(ss){
                       if(node->parent == NULL){
                            return NULL;
                        }else{
                            level = level-node->getPrefixLength()-1;
                            return findFirst(node->parent,k[level],ss,level,k);
                        }
                   }else{//右边第一个的最小值
                       pos2=keyByte;
                       while(!node->children[++pos2] && pos2<256);
                       return  pos2 >255 ? NULL:N::minimum(node->children[pos2]);
                   }
                    
                }
                if(node->maxr == keyByte){//最右边界判断  //也需要加一个判断
                   if(ss){
                       pos1=keyByte;
                       while(!node->children[--pos1]);
                       return N::maximum(node->children[pos1]);

                   }else{
                       if(node->parent == NULL){
                        return NULL;
                    }else{
                        level = level-node->getPrefixLength()-1;
                        return findFirst(node->parent,k[level],ss,level,k);
                    }
                   } 
                }
                pos1=pos2=keyByte;
                //cout<<pos1<<endl;
                if(ss){
                    while(!node->children[--pos1]);
                    //cout<<"pos1="<<pos1<<endl;
                    return N::maximum(node->children[pos1]);
                }else{
                    while(!node->children[++pos2] && pos2<256);
                       
                    return  pos2 >255 ? NULL:N::minimum(node->children[pos2]);
                } 
                
                // return &(node->child[keyByte]);
                break;
            }
        }
        throw; // Unreachable
    }

    void findBoundary(N *node,const Key &k,uint32_t level,N *&first,N *&second){
        unsigned pos;
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                //cout<<"n->count:"<<n->count<<endl;
                for(pos=0;(pos<n->count)&&(n->keys[pos]<k[level]);pos++);//相比原来的判断的level发生来了改变
                //cout<<pos<<endl;
                if(pos == 0){
                        //cout<<"node 2-4-1"<<endl;
                        second = N::minimum(n->children[pos]);//当前层的最小值
                        if(n->parent != NULL){
                           level = level- n->getPrefixLength()-1;
                           first = findFirst(n->parent,k[level],true,level,k);//上层左边节点最大值
                        }
                        
                        //first = findFirst(nodeParent[0],key[nodeParent[0]->prefixLength],true);//上层左边节点最大值
                }else if(pos == n->count){
                            //cout<<"node 2-4-2"<<endl;
                            first = N::maximum(n->children[pos-1]);//当前层节点的最大值
                            //cout<<"first:"<< N::getLeaf(first)<<endl;
                            if(n->parent != NULL){
                                //cout<<level<<endl;
                                level = level- n->getPrefixLength()-1;
                                second = findFirst(n->parent,k[level],false,level,k);//上层右边节点最小值
                            }
                            //second = findFirst(nodeParent[0],key[nodeParent[0]->prefixLength],false);//上层右边节点最小值
                        } else{
                            //cout<<"node 2-4-3"<<endl;
                            first = N::maximum(n->children[pos-1]);//左边最大
                            second = N::minimum(n->children[pos]);//右边最小
                        }
                break;
                

               
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                //cout<<"not N16"<<endl;
                for(pos=0;(pos<n->count)&&n->flipSign(n->keys[pos])<k[level];pos++);
                //cout<<"pos="<<pos<<endl;
                // cout<<"n->count:"<<n->count<<endl;
                if(pos == 0){ //应该有问题 判断不全面
                        //cout<<"node 2-16-1"<<endl;
                        second = N::minimum(n->children[pos]);
                        //if(judge(nodeParent[0],key[depth-2]))
                        if(n->parent){
                            level = level- n->getPrefixLength()-1;
                            first = findFirst(n->parent,k[level],true,level,k);//上层左边最大
                        }
                }else if(pos == n->count){
                            //cout<<"node 2-16-2"<<endl;
                            //cout<<"sn sn 2"<<endl;
                            first = N::maximum(n->children[pos-1]);
                            if(n->parent){
                              level = level- n->getPrefixLength()-1;
                              second = findFirst(n->parent,k[level],false,level,k);//上层右边节点最小值
                            }
                            // else{
                            //     cout<<"yy is sd"<<endl;
                            // }  
                        }else{
                            //cout<<"dsds"<<endl;
                           // cout<<"node 2-16-3"<<endl;
                            first = N::maximum(n->children[pos-1]);
                            second = N::minimum(n->children[pos]);
                        }
                break;
              
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
               // cout<<"not N48"<<endl;
                //cout<<"depth="<<depth<<endl;
               //cout<<"Node 48"<<endl;

              // cout<<"min = "<<+(n->minl)<<" max ="<<+(n->maxr)<<endl;
               
               unsigned pos1,pos2,pos3=0,pos4=255;
               //cout<<+key[depth-1]<<endl;
               while(n->childIndex[pos3] == n->emptyMarker)
                   pos3++;//Node48中的最小值
               //cout<<"pos3="<<pos3<<endl;
               while(n->childIndex[pos4] == n->emptyMarker)
                   pos4--;//Node48中的最大值

               //cout<<"pos3 = "<<pos3<<" pos4 ="<<pos4<<endl;

               if(n->maxr < k[level]){ //右边界
                  first = N::maximum(n->children[n->childIndex[n->maxr]]);
                  if(n->parent){
                      level = level- n->getPrefixLength()-1;
                      second = findFirst(n->parent,k[level],false,level,k);//上层节点的最小值
                  }
               }else if(k[level] < n->minl){ //左边界
                        //cout<<"sn sn syd"<<endl;
                        second  = N::minimum(n->children[n->childIndex[n->minl]]);
                        //需要加判断 有可能要插入的值比当前最小值还小
                        //cout<<"sn sn syd 1"<<endl;
                        //cout<<depth<<endl;
                        //cout<<+key[depth-2]<<endl;
                        //if(judge(nodeParent[0],key[depth-2]))
                        level = level- n->getPrefixLength()-1;
                        first = findFirst(n->parent,k[level],true,level,k);//上层左边最大
                  }
                  else{
                      pos1=pos2=k[level];
                     //cout<<+key[depth-1]<<endl;
                     while(n->childIndex[--pos1] == n->emptyMarker);
                     while(n->childIndex[++pos2] == n->emptyMarker);
                     first = N::maximum(n->children[n->childIndex[pos1]]);
                     second = N::minimum(n->children[n->childIndex[pos2]]);
               }
               break;
              
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                //cout<<"not N256"<<endl;
                if(n->count == 0){//第一次的两次查找 索引为空的时候
                    return;
                }
                unsigned pos1,pos2,pos3=0,pos4=255;
                // while(!(n->children[++pos3]));
                // while(!(n->children[--pos4]));

                while(!(n->children[pos3++]));//如果边界有值的话会计算出错
                while(!(n->children[pos4--]));
                
                //cout<<"min = "<<+(n->minl)<<" max ="<<+(n->maxr)<<endl;
                //cout<<"pos3 = "<<pos3<<" pos4 ="<<pos4<<endl;
                //cout<<pos3<<endl;
                //cout<<pos4<<endl;
                if(n->maxr < k[level]){
                    //cout<<"sn sn sn N256 右"<<endl;
                    first = N::maximum(n->children[n->maxr]);
                    level = level- n->getPrefixLength()-1;
                    second = findFirst(n->parent,k[level],false,level,k);//上层节点的最小值
                }else if(k[level] < n->minl){
                            //cout<<"sn sn sn N256 左"<<endl;
                            second  = N::minimum(n->children[n->minl]);
                            //if(judge(nodeParent[0],key[depth-2]))
                            if(n->parent){
                                level = level- n->getPrefixLength()-1;
                                first = findFirst(n->parent,k[level],true,level,k);//上层左边最大
                            }    
                        }
                        else{
                            // cout<<depth<<endl;
                            // cout<<+key[depth-1]<<endl;
                            pos1=pos2=k[level];
                            while(!(n->children[--pos1]));
                            //cout<<pos1<<endl;
                            while(!n->children[++pos2]);
                            //cout<<pos2<<endl;
                            first = N::maximum(n->children[pos1]);
                            second = N::minimum(n->children[pos2]);
                        }
                break;
                
            }
        }

    }

    /* END */
    IntPair createOffsetPair(N *&first, N *&second, ElementType limit) {
        IntPair op = (IntPair) malloc(sizeof(struct int_pair));
        if (first && second) {
            op->first = N::getLeaf(first);
            op->second = N::getLeaf(second)-1;
        } else if (first) {
            op->first = N::getLeaf(first);
            op->second = limit;
        } else if (second) {
            op->first = 0;
            op->second = N::getLeaf(second)-1;
        } else {
            op->first = 0;
            op->second = limit;
        }
        return op;
    }


     //查找的代码 单线程下的查找代码
     IntPair Tree::LookupLT(const Key &k, uint64_t limit) const{
        N *first = nullptr;
        N *second = nullptr;

        N *node = nullptr;
        N *nextNode = root;
        uint32_t level = 0;
        bool optimisticPrefixMatch = false;

        while (true) {
            node = nextNode;
            //  if(node != nullptr ){
            //     cout<<"node is note null"<<endl;
            //     cout<<node->getPrefixLength()<<endl;
            //     cout<<"node->count: "<<+(node->count)<<endl;
            //     const uint8_t *c = node->getPrefix();
            //     for(int i=0;i<node->getPrefixLength();i++){
            //         cout<<+(c[i])<<" ";
            //     }
            //     if(node->getPrefixLength()>0){
            //          cout<<endl;
            //     }
            //     if(node->getType() == NTypes::N256){
            //         cout<<"N256"<<endl;
            //     }else if(node->getType() == NTypes::N48){
            //         cout<<"N48"<<endl;
            //     }else if(node->getType() == NTypes::N16){
            //         cout<<"N16"<<endl;
            //     }else {
            //         cout<<"N4"<<endl;
            //     }

            // //     if(node->parent != nullptr){
            // //         if(node->parent->getType() == NTypes::N256){
            // //             cout<<"N256"<<endl;
            // //         }else if(node->parent->getType() == NTypes::N48){
            // //             cout<<"N48"<<endl;
            // //         }else if(node->parent->getType() == NTypes::N16){
            // //             cout<<"N16"<<endl;
            // //         }else {
            // //             cout<<"N4"<<endl;
            // //         }
            // //    }
            // }
            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch://前缀不匹配的情况
                    {
                    PCCompareResults prefixResult;
                    //cout<<"NoMatch level ="<<level<<endl;
                    auto newLevel = level- node->getPrefixLength();
                    prefixResult = checkPrefixCompare(node, k, newLevel, loadKey);//其实可以直接使用这个进行下降？？？？
                    switch (prefixResult) {
                        case PCCompareResults::Bigger://内部节点比较大 //注意first有可能在上一层中查找
                            //cout<<"bigger"<<endl;
                            second = N::minimum(node);
                            //cout<<level<<endl;
                            level = level- node->getPrefixLength();
                            //cout<<level<<endl;
                            first = findFirst(node->parent,k[level],true,level,k);//上层左边最大
                            break;
                        case PCCompareResults::Equal: {
                            break;
                        }
                        case PCCompareResults::Smaller://内部节点小于key的时候就是small (需要完善 没写)
                            cout<<"Smaller"<<endl;
                            break;
                    }

                    //cout<<"first:"<< N::getLeaf(first)<<endl;
                    //cout<<"second:"<< N::getLeaf(second)<<endl;
                    // if(N::getLeaf(first) >  N::getLeaf(second)){
                    //     cout<<"sb"<<endl;
                    // }
                    return createOffsetPair(first, second, limit);
                    //return NULL;
                    }
                case CheckPrefixResult::OptimisticMatch://目前没有用上
                    optimisticPrefixMatch = true;
                    // fallthrough
                case CheckPrefixResult::Match:
                    if (k.getKeyLen() <= level) {
                        return NULL;
                    }
                    nextNode = N::getChild(k[level], node);
                    
                    //cout<<"level= "<<level<<endl;
                    if (nextNode == nullptr) {
                       // cout<<"nextNode is NULL"<<endl;
                        // 当查找到空节点的时候根据父亲节点的不同去进行查找
                        findBoundary(node,k,level,first,second);
                       // cout<<"first:"<< N::getLeaf(first)<<endl;
                        //cout<<"second:"<< N::getLeaf(second)<<endl;
                        // if(N::getLeaf(first) >  N::getLeaf(second)){
                        //   cout<<"sb"<<endl;
                        // }

                        return createOffsetPair(first, second, limit);
                        //return NULL;
                    }
                    if (N::isLeaf(nextNode)) {  //目前还没有增加边界条件的判断
                        TID tid = N::getLeaf(nextNode);
                        if (level < k.getKeyLen() - 1 || optimisticPrefixMatch) {//路径压缩的情况 需要比较当前key和叶子节点的key
                            //return checkKey(tid, k);//改成要返回一个节点
                            Key kt;
                            this->loadKey(tid,kt);
                            int res = std::memcmp(&k[0], &kt[0], k.getKeyLen());;
                            if(res == 0){
                                return {0};
                                //return nextNode;
                            }else if(res < 0){//叶子节点的值比较大 (当然这里也应该比较前缀)
                                  second = nextNode;
                                  //level = level- node->getPrefixLength();
                                  //cout<<"yy tx1"<<endl;
                                  first = findFirst(node,k[level],true,level,k);//上层左边最大
                                  //cout<<"bound not right"<<endl;
                            }else{
                                 first = nextNode;
                                 if(node){
                                     //cout<<"yy tx2"<<endl;
                                     //level = level- node->getPrefixLength();
                                     second = findFirst(node,k[level],false,level,k);//上层节点的最小值
                                 }
                                    

                            }
                            //cout<<"first:"<< N::getLeaf(first)<<endl;
                            //cout<<"second:"<< N::getLeaf(second)<<endl;
                            // if(N::getLeaf(first) >  N::getLeaf(second)){
                            //     cout<<"sb"<<endl;
                            // }
                            return createOffsetPair(first, second, limit);
                            //return NULL;
                        }
                        //return nextNode;
                    }
                    level++;
            }
        
       }
    }
    
    // test parent case
    TID Tree::lookup2(const Key &k) const {
        N *node = nullptr;
        N *nextNode = root;
        uint32_t level = 0;
        bool optimisticPrefixMatch = false;

        while (true) {
            node = nextNode;
            if(node != nullptr ){
                cout<<"node is note null"<<endl;
                //cout<<+(node->getCount())<<endl;
                if(node->getType() == NTypes::N256){
                        cout<<"N256"<<endl;
                    }else if(node->getType() == NTypes::N48){
                        cout<<"N48"<<endl;
                    }else if(node->getType() == NTypes::N16){
                        cout<<"N16"<<endl;
                    }else {
                        cout<<"N4"<<endl;
                    }
                if(node->parent != nullptr){
                    if(node->parent->getType() == NTypes::N256){
                        cout<<"N256"<<endl;
                    }else if(node->parent->getType() == NTypes::N48){
                        cout<<"N48"<<endl;
                    }else if(node->parent->getType() == NTypes::N16){
                        cout<<"N16"<<endl;
                    }else {
                        cout<<"N4"<<endl;
                    }
                }

            }
           
            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch:
                    return 0;
                case CheckPrefixResult::OptimisticMatch://目前没有用上
                    optimisticPrefixMatch = true;
                    // fallthrough
                case CheckPrefixResult::Match:
                    if (k.getKeyLen() <= level) {
                        return 0;
                    }
                    nextNode = N::getChild(k[level], node);

                    if (nextNode == nullptr) {
                        return 0;
                    }
                    if (N::isLeaf(nextNode)) {
                        TID tid = N::getLeaf(nextNode);
                        if (level < k.getKeyLen() - 1 || optimisticPrefixMatch) {//这一步是干嘛？？？
                            return checkKey(tid, k);
                        }
                        return tid;
                    }
                    level++;
            }
        }
    }



  

    TID Tree::checkKey(const TID tid, const Key &k) const {
        Key kt;
        this->loadKey(tid, kt);
        if (k == kt) {
            return tid;
        }
        return 0;
    }

    void Tree::insert(const Key &k, TID tid) {
        N *node = nullptr;
        N *nextNode = root;
        //if(nextNode->getType() == NTypes::N256)
        //   cout<<"True"<<endl;
        N *parentNode = nullptr;
        uint8_t parentKey, nodeKey = 0;
        uint32_t level = 0;

        while (true) {
            parentNode = node;//父亲节点（父亲节点开始为空）
            parentKey = nodeKey;//父亲节点的key
            node = nextNode;//当前的节点（需要进行比较的节点）

            uint32_t nextLevel = level;

            uint8_t nonMatchingKey;//主要在不匹配的时候记录不匹配的前缀
            Prefix remainingPrefix;//记录在不匹配的时候还有多少前缀进行匹配
            switch (checkPrefixPessimistic(node, k, nextLevel, nonMatchingKey, remainingPrefix,
                                                           this->loadKey)) { // increases level
                //：情况1：当已经插入了255个节点，当插入key为256的时候，会出现不匹配的情况执行下面不匹配的代码
                case CheckPrefixPessimisticResult::NoMatch: {  
                    //cout<<"No Match"<<endl;
                    assert(nextLevel < k.getKeyLen()); //prevent duplicate key
                    // 1) Create new node which will be parent of node, Set common prefix, level to this node
                    //cout<<"nextLevel= "<<nextLevel<<" level="<<level<<endl;
                    //nextLevel:为不匹配的位置 Level为当前节点前缀匹配的位置
                    auto newNode = new N4(node->getPrefix(), nextLevel - level);// 第256个节点 nextLevel=6,level=1

                    // 2)  add node and (tid, *k) as children
                    newNode->insert(k[nextLevel], N::setLeaf(tid));
                    newNode->insert(nonMatchingKey, node);

                    // 3) update parentNode to point to the new node 更新父节点的指针指向当前节点
                    N::change(parentNode, parentKey, newNode);

                    // 4) update prefix of node 更新节点的前缀 
                    node->setPrefix(remainingPrefix,
                                    node->getPrefixLength() - ((nextLevel - level) + 1));
                    
                    newNode->parent = node->parent;// add newNode 's parent pointer 需要修改2处
                    node->parent = newNode;
                    return;
                }
                case CheckPrefixPessimisticResult::Match:{
                       //cout<<"first"<<endl;
                       break;
                }
                
                    
            }
            assert(nextLevel < k.getKeyLen()); //prevent duplicate key
            level = nextLevel;//新的level
            nodeKey = k[level];
            //cout<<"nodeKey="<<int(nodeKey)<<" level="<<level<<endl;
            nextNode = N::getChild(nodeKey, node);

            if (nextNode == nullptr) {
                //cout<<"insert 1"<<endl;
                N::insertA(node, parentNode, parentKey, nodeKey, N::setLeaf(tid));
                return;
            }
            if (N::isLeaf(nextNode)) {//那个位置是叶子节点 情况1：由1个节点变成2个节点可以执行到这里
                Key key;
                loadKey(N::getLeaf(nextNode), key);//因为实验的value和key一样，根据value 得到key

                level++;//下降一层
                assert(level < key.getKeyLen()); //prevent inserting when prefix of key exists already
                uint32_t prefixLength = 0;

                while (key[level + prefixLength] == k[level + prefixLength]) {
                    prefixLength++;//两个节点的最长公共前缀的长度
                }
                //cout<<"prefixLength="<< prefixLength<<endl;

                auto n4 = new N4(&k[level], prefixLength);//为了区分2个节点需要建立内部节点 
                n4->insert(k[level + prefixLength], N::setLeaf(tid));
                n4->insert(key[level + prefixLength], nextNode);
                N::change(node, k[level - 1], n4);//把这个n4节点插入到跟节点（N256节点）
				//cout<<"sdsd"<<endl;
                
                n4->parent = node;// add newNode 's parent pointer

                return;
            }

            level++;
        }
    }
    
    //在查找的时候用的前缀匹配
    inline typename Tree::CheckPrefixResult Tree::checkPrefix(N *n, const Key &k, uint32_t &level) {
        if (k.getKeyLen() <= level + n->getPrefixLength()) {//一种前缀不匹配的情况
            return CheckPrefixResult::NoMatch;
        }
        if (n->hasPrefix()) {
            for (uint32_t i = 0; i < std::min(n->getPrefixLength(), maxStoredPrefixLength); ++i) {//仍然发生了前缀不匹配的情况
                if (n->getPrefix()[i] != k[level]) {
                    return CheckPrefixResult::NoMatch;
                }
                ++level;
            }
            if (n->getPrefixLength() > maxStoredPrefixLength) {
                level += n->getPrefixLength() - maxStoredPrefixLength;
                return CheckPrefixResult::OptimisticMatch;
            }
        }
        return CheckPrefixResult::Match;//N256都会返回Match
    }
    //在插入的时候用的前缀匹配
    //判断是否匹配，匹配的话返回Match以及当前的level 
               // 不匹配的话返回nonMachingKey 以及 nonMatchingPrefix 和 当前的level
    typename Tree::CheckPrefixPessimisticResult Tree::checkPrefixPessimistic(N *n, const Key &k, uint32_t &level,
                                                                        uint8_t &nonMatchingKey,
                                                                        Prefix &nonMatchingPrefix,
                                                                        LoadKeyFunction loadKey) {
        if (n->hasPrefix()) {
            uint32_t prevLevel = level;
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {//基本上没有用
                    loadKey(N::getAnyChildTid(n), kt);
                }
                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];//取对应的每一位前缀去匹配
                if (curKey != k[level]) {//应该是不匹配的时候要分裂(发生了不匹配的时候：主要求nonMarchingKey nonMarchingPrefix)
                    nonMatchingKey = curKey;
                    //cout<<n->getPrefixLength()<<" "<< maxStoredPrefixLength<<endl;
                    if (n->getPrefixLength() > maxStoredPrefixLength) {//这里也基本都不会执行到
                        if (i < maxStoredPrefixLength) {
                            loadKey(N::getAnyChildTid(n), kt);
                        }
                        //cout<<"测试是否可以执行到这里"<<endl;
                        for (uint32_t j = 0; j < std::min((n->getPrefixLength() - (level - prevLevel) - 1),
                                                          maxStoredPrefixLength); ++j) {
                            nonMatchingPrefix[j] = kt[level + j + 1];
                        }
                    } else {
                        //cout<<"n->getPrefixLength() - i - 1 = "<<n->getPrefixLength() - i - 1<<endl;
                        for (uint32_t j = 0; j < n->getPrefixLength() - i - 1; ++j) {
                            nonMatchingPrefix[j] = n->getPrefix()[i + j + 1];
                        }
                    }
                    return CheckPrefixPessimisticResult::NoMatch;
                }
                ++level;//相等就++level
            }
        }
        return CheckPrefixPessimisticResult::Match;//前缀相等(匹配)
    }
    
    //在范围查询中用的前缀匹配
    typename Tree::PCCompareResults Tree::checkPrefixCompare(N *n, const Key &k, uint32_t &level,
                                                        LoadKeyFunction loadKey) {
        if (n->hasPrefix()) {
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    loadKey(N::getAnyChildTid(n), kt);
                }
                uint8_t kLevel = (k.getKeyLen() > level) ? k[level] : 0;// //start 第level上的key

                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];// 当前内部节点 第level上的key
                //cout<<"curKey= "<<+(curKey)<<" kLevel=  "<<+(kLevel)<<endl;
                if (curKey < kLevel) {
                    return PCCompareResults::Smaller;
                } else if (curKey > kLevel) {
                    return PCCompareResults::Bigger;
                }
                ++level;
            }
        }
        return PCCompareResults::Equal;
    }

    typename Tree::PCEqualsResults Tree::checkPrefixEquals(N *n, uint32_t &level, const Key &start, const Key &end,
                                                      LoadKeyFunction loadKey) {
        if (n->hasPrefix()) {
            bool endMatches = true;
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    loadKey(N::getAnyChildTid(n), kt);
                }
                uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 0;

                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];
                if (curKey > startLevel && curKey < endLevel) {
                    return PCEqualsResults::Contained;
                } else if (curKey < startLevel || curKey > endLevel) {
                    return PCEqualsResults::NoMatch;
                } else if (curKey != endLevel) {
                    endMatches = false;
                }
                ++level;
            }
            if (!endMatches) {
                return PCEqualsResults::StartMatch;
            }
        }
        return PCEqualsResults::BothMatch;
    }

     void Tree::remove(const Key &k, TID tid) {
        N *node = nullptr;
        N *nextNode = root;
        N *parentNode = nullptr;
        uint8_t parentKey, nodeKey = 0;
        uint32_t level = 0;
        //bool optimisticPrefixMatch = false;

        while (true) {
            parentNode = node;
            parentKey = nodeKey;
            node = nextNode;

            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch:
                    return;
                case CheckPrefixResult::OptimisticMatch:
                    // fallthrough
                case CheckPrefixResult::Match: {
                    nodeKey = k[level];
                    nextNode = N::getChild(nodeKey, node);

                    if (nextNode == nullptr) {
                        return;
                    }
                    if (N::isLeaf(nextNode)) {
                        if (N::getLeaf(nextNode) != tid) {
                            return;
                        }
                        assert(parentNode == nullptr || node->getCount() != 1);
                        if (node->getCount() == 2 && node != root) {
                            // 1. check remaining entries
                            N *secondNodeN;
                            uint8_t secondNodeK;
                            std::tie(secondNodeN, secondNodeK) = N::getSecondChild(node, nodeKey);
                            if (N::isLeaf(secondNodeN)) {

                                //N::remove(node, k[level]); not necessary
                                N::change(parentNode, parentKey, secondNodeN);

                                delete node;
                            } else {
                                //N::remove(node, k[level]); not necessary
                                N::change(parentNode, parentKey, secondNodeN);
                                secondNodeN->addPrefixBefore(node, secondNodeK);

                                delete node;
                            }
                        } else {
                            N::removeA(node, k[level], parentNode, parentKey);
                        }
                        return;
                    }
                    level++;
                }
            }
        }
    }

    //原来的代码李以前是注释部分

     /*
      bool Tree::lookupRange(const Key &start, const Key &end, Key &continueKey, TID result[],
                                std::size_t resultSize, std::size_t &resultsFound) const {
        return false;
        for (uint32_t i = 0; i < std::min(start.getKeyLen(), end.getKeyLen()); ++i) {
            if (start[i] > end[i]) {
                resultsFound = 0;
                return false;
            } else if (start[i] < end[i]) {
                break;
            }
        }
        TID toContinue = 0;
        std::function<void(const N *)> copy = [&result, &resultSize, &resultsFound, &toContinue, &copy](const N *node) {
            if (N::isLeaf(node)) {
                if (resultsFound == resultSize) {
                    toContinue = N::getLeaf(node);
                    return;
                }
                result[resultsFound] = N::getLeaf(node);
                resultsFound++;
            } else {
                std::tuple<uint8_t, N *> children[256];
                uint32_t childrenCount = 0;
                N::getChildren(node, 0u, 255u, children, childrenCount);
                for (uint32_t i = 0; i < childrenCount; ++i) {
                    const N *n = std::get<1>(children[i]);
                    copy(n);
                    if (toContinue != 0) {
                        break;
                    }
                }
            }
        };
        std::function<void(const N *, uint32_t)> findStart = [&copy, &start, &findStart, &toContinue, &restart, this](
                const N *node, uint32_t level) {
            if (N::isLeaf(node)) {
                copy(node);
                return;
            }

            uint64_t v;
            PCCompareResults prefixResult;
            do {
                v = node->startReading();
                prefixResult = checkPrefixCompare(node, start, level, loadKey);
            } while (!node->stopReading(v));
            switch (prefixResult) {
                case PCCompareResults::Bigger:
                    copy(node);
                    break;
                case PCCompareResults::Equal: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    N::getChildren(node, startLevel, 255, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        const N *n = std::get<1>(children[i]);
                        if (k == startLevel) {
                            findStart(n, level + 1);
                        } else if (k > startLevel) {
                            copy(n);
                        }
                        if (toContinue != 0 || restart) {
                            break;
                        }
                    }
                    break;
                }
                case PCCompareResults::SkippedLevel:
                    restart = true;
                    break;
                case PCCompareResults::Smaller:
                    break;
            }
        };
        std::function<void(const N *, uint32_t)> findEnd = [&copy, &end, &toContinue, &restart, &findEnd, this](
                const N *node, uint32_t level) {
            if (N::isLeaf(node)) {
                return;
            }
            uint64_t v;
            PCCompareResults prefixResult;
            do {
                v = node->startReading();
                prefixResult = checkPrefixCompare(node, end, level, loadKey);
            } while (!node->stopReading(v));

            switch (prefixResult) {
                case PCCompareResults::Smaller:
                    copy(node);
                    break;
                case PCCompareResults::Equal: {
                    uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    N::getChildren(node, 0, endLevel, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        const N *n = std::get<1>(children[i]);
                        if (k == endLevel) {
                            findEnd(n, level + 1);
                        } else if (k < endLevel) {
                            copy(n);
                        }
                        if (toContinue != 0 || restart) {
                            break;
                        }
                    }
                    break;
                }
                case PCCompareResults::Bigger:
                    break;
                case PCCompareResults::SkippedLevel:
                    restart = true;
                    break;
            }
        };

        restart:
        restart = false;
        resultsFound = 0;

        uint32_t level = 0;
        N *node = nullptr;
        N *nextNode = root;

        while (true) {
            node = nextNode;
            uint64_t v;
            PCEqualsResults prefixResult;
            do {
                v = node->startReading();
                prefixResult = checkPrefixEquals(node, level, start, end, loadKey);
            } while (!node->stopReading(v));
            switch (prefixResult) {
                case PCEqualsResults::SkippedLevel:
                    goto restart;
                case PCEqualsResults::NoMatch: {
                    return false;
                }
                case PCEqualsResults::Contained: {
                    copy(node);
                    break;
                }
                case PCEqualsResults::StartMatch: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    N::getChildren(node, startLevel, 255, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        const N *n = std::get<1>(children[i]);
                        if (k == startLevel) {
                            findStart(n, level + 1);
                        } else if (k > startLevel) {
                            copy(n);
                        }
                        if (restart) {
                            goto restart;
                        }
                        if (toContinue) {
                            break;
                        }
                    }
                    break;
                }
                case PCEqualsResults::BothMatch: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;
                    if (startLevel != endLevel) {
                        std::tuple<uint8_t, N *> children[256];
                        uint32_t childrenCount = 0;
                        N::getChildren(node, startLevel, endLevel, children, childrenCount);
                        for (uint32_t i = 0; i < childrenCount; ++i) {
                            const uint8_t k = std::get<0>(children[i]);
                            const N *n = std::get<1>(children[i]);
                            if (k == startLevel) {
                                findStart(n, level + 1);
                            } else if (k > startLevel && k < endLevel) {
                                copy(n);
                            } else if (k == endLevel) {
                                findEnd(n, level + 1);
                            }
                            if (restart) {
                                goto restart;
                            }
                            if (toContinue) {
                                break;
                            }
                        }
                    } else {
                        nextNode = N::getChild(startLevel, node);
                        if (!node->stopReading(v)) {
                            goto restart;
                        }
                        level++;
                        continue;
                    }
                    break;
                }
            }
            break;
        }
        if (toContinue != 0) {
            loadKey(toContinue, continueKey);
            return true;
        } else {
            return false;
        }
    }
    */

}