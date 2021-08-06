#include <assert.h>
#include <algorithm>
#include<functional>
#include "Tree.h"
#include "N.cpp"
#include "../Epoche.cpp"
#include "../Key.h"

using namespace std;




namespace ART_OLC {

    Tree::Tree(LoadKeyFunction loadKey) : root(new N256( nullptr, 0)), loadKey(loadKey) {
    }

    Tree::~Tree() {
        N::deleteChildren(root);
        N::deleteNode(root);
    }

    ThreadInfo Tree::getThreadInfo() {
        return ThreadInfo(this->epoche);
    }

    

    
    // N* Tree::findFirst(N* n,uint8_t keyByte,bool ss,uint32_t &level,const Key &k) {
    //     // Find the next child for the keyByte
    //     //cout<<"KeyByte = "<<+(keyByte)<<endl;
    //     switch (n->getType()) {
    //         case NTypes::N4: {
    //            // cout<<"findFirst Node 4"<<endl;
    //             auto node = static_cast<N4 *>(n);
    //             if(node->keys[0] == keyByte ){//等于边界的情况
    //                 // cout<<"yy"<<endl;
    //                 if(ss){
    //                     level = level-node->getPrefixLength()-1;
    //                     return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
    //                 }else{
    //                     //cout<<"dsd"<<endl;
    //                     return N::minimum(node->children[1]);//右边最小
    //                 }
    //                //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
    //             }
    //             unsigned i,j=-1;
    //             for(i=0;(i<node->count)&&(node->keys[i]<keyByte);i++)
    //                     j = i;
    //             if(ss)
    //                 return N::maximum(node->children[j]);//上层左边最大
    //             else
    //                 return  N::minimum(node->children[i+1]);//上层右边最小
    //             break;
    //         }
    //         case  NTypes::N16: {
    //            // cout<<"findFirst Node 16"<<endl;
    //             unsigned i,j=-1;
    //             auto node = static_cast<N16 *>(n);
    //             //先判断边界
    //             //cout<<"node->flipSign(node->keys[0]) = "<<+(node->flipSign(node->keys[0]))<<endl;
    //             if(node->flipSign(node->keys[0]) == keyByte ){
    //                 // cout<<"yy"<<endl;
    //                 if(ss){
    //                     level = level-node->getPrefixLength()-1;
    //                     return findFirst(node->parent,k[level],ss,level,k);
    //                 }else{
    //                     //cout<<"dsd"<<endl;
    //                     return N::minimum(node->children[1]);
    //                 }
    //                //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
    //             }
    //             for(i=0;(i<node->count)&&(node->flipSign(node->keys[i])<keyByte);i++)// 第12次的时候刚好到达了边界
    //                     j = i;
    //             if(ss)
    //                 return N::maximum(node->children[j]);//上层左边最大
    //             else
    //                 return  N::minimum(node->children[i+1]);//上层右边最小
                
    //             // __m128i cmp=_mm_cmpeq_epi8(_mm_set1_epi8(flipSign(keyByte)),_mm_loadu_si128(reinterpret_cast<__m128i*>(node->key)));
    //             // unsigned bitfield=_mm_movemask_epi8(cmp)&((1<<node->count)-1);
    //             // if (bitfield)
    //             //    return &node->child[ctz(bitfield)]; else
    //             //    return &nullNode;
    //         }
    //         case NTypes::N48: {
    //             //cout<<"findFirst Node 48"<<endl;
    //             auto node = static_cast<N48 *>(n);
    //             if(node->minl == keyByte ){  //同样需要加右边界判断（没有完成）
    //                 // cout<<"yy"<<endl;
    //                 if(ss){
    //                     level = level-node->getPrefixLength()-1;
    //                     return findFirst(node->parent,k[level],ss,level,k);
    //                 }else{
    //                     cout<<"N48 bound not complete"<<endl;
    //                     ///return N::minimum(node->children[1]);
    //                 }
    //                //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
    //             }
    //             unsigned pos1,pos2;
    //             pos1=pos2=keyByte;
    //             //cout<<pos2<<endl;
    //             //cout<<+key[depth-1]<<endl;
    //             if(ss){
    //                 //cout<<pos1<<endl;
    //                 while(node->childIndex[--pos1] == node->emptyMarker);
    //                 //cout<<pos1<<endl;
    //                 return N::maximum(node->children[node->childIndex[pos1]]);
    //             }else{
    //                 //cout<<pos2<<endl;
    //                 while(node->childIndex[++pos2] == node->emptyMarker);
    //                 return  N::minimum(node->children[node->childIndex[pos2]]);
    //             } 
    //             break;
    //         }
    //         case NTypes::N256: {  //需要测试
    //             //cout<<"find N256"<<endl;
    //             auto node = static_cast<N256 *>(n);
    //             unsigned pos1,pos2;
    //             // if(node->children[0] && node->parent == NULL){ //先随便写了一个判断
    //             //     return NULL;
    //             // }
    //             if(node->minl == keyByte){//最左边界判断
    //                if(ss){
    //                    if(node->parent == NULL){
    //                         return NULL;
    //                     }else{
    //                         level = level-node->getPrefixLength()-1;
    //                         return findFirst(node->parent,k[level],ss,level,k);
    //                     }
    //                }else{//右边第一个的最小值
    //                    pos2=keyByte;
    //                    while(!node->children[++pos2] && pos2<256);
    //                    return  pos2 >255 ? NULL:N::minimum(node->children[pos2]);
    //                }
                    
    //             }
    //             if(node->maxr == keyByte){//最右边界判断  //也需要加一个判断
    //                if(ss){
    //                    pos1=keyByte;
    //                    while(!node->children[--pos1]);
    //                    return N::maximum(node->children[pos1]);

    //                }else{
    //                    if(node->parent == NULL){
    //                     return NULL;
    //                 }else{
    //                     level = level-node->getPrefixLength()-1;
    //                     return findFirst(node->parent,k[level],ss,level,k);
    //                 }
    //                } 
    //             }
    //             pos1=pos2=keyByte;
    //             //cout<<pos1<<endl;
    //             if(ss){
    //                 while(!node->children[--pos1]);
    //                 //cout<<"pos1="<<pos1<<endl;
    //                 return N::maximum(node->children[pos1]);
    //             }else{
    //                 while(!node->children[++pos2] && pos2<256);
                       
    //                 return  pos2 >255 ? NULL:N::minimum(node->children[pos2]);
    //             } 
    //             // return &(node->child[keyByte]);
    //             break;
    //         }
    //     }
    //     throw; // Unreachable
    // }


    //tree 的 minimum 方法
    N* Tree::minimum(N* node, bool &needRestart, uint64_t v, ThreadInfo &threadEpocheInfo){//按照对node节点加锁来写
         N *parentNode = nullptr;//需要保留父亲节点
         while(true){
           
            parentNode = node;
            node = N::getMinChild(node);

            if(!node){ //其实这里必不可能为空
                 return NULL;
            }

            if (N::isLeaf(node)){
                 parentNode->readUnlockOrRestart(v, needRestart);//父亲节点不为空的话，先对父亲节点解锁（验证）
                 if (needRestart) return NULL;
                 return node;
            }

            uint64_t nv = node->readLockOrRestart(needRestart);//读取下一个节点的版本号
            if (needRestart) return NULL;

            parentNode->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
            if (needRestart) return NULL;
            v = nv;
         }
    }

    //tree 的 maximum 方法
    N* Tree::maximum(N* node, bool &needRestart, uint64_t v, ThreadInfo &threadEpocheInfo){//按照对node节点加锁来写
         N *parentNode = nullptr;//需要保留父亲节点
         while(true){

            parentNode = node;
            node = N::getMaxChild(node);
 
            if(!node){  //其实这里必不可能为空
                 return NULL;
             }

            if (N::isLeaf(node)){
                 parentNode->readUnlockOrRestart(v, needRestart);//父亲节点不为空的话，先对父亲节点解锁（验证）
                 if (needRestart) return NULL;// 最好的情况下这里什么都返回
                 return node;
            }

            uint64_t nv = node->readLockOrRestart(needRestart);//读取下一个节点的版本号
            if (needRestart) return NULL;

            parentNode->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
            if (needRestart) return NULL;
            v = nv;
         }
    }

    
    //多线程的findFirst
    N* Tree::findFirst(N* n,uint8_t keyByte,bool ss,uint32_t &level,const Key &k,
                           bool &needRestart, uint64_t v, ThreadInfo &threadEpocheInfo) {
        N *parentNode = nullptr;//需要保留父亲节点
        N *nextNode = nullptr;
        N *res = nullptr;
        while(true){  
            switch (n->getType()) {
                case NTypes::N4: {
                    // cout<<"findFirst Node 4"<<endl;
                    auto node = static_cast<N4 *>(n);
                    if(node->keys[0] == keyByte ){//等于左边界的情况
                        // cout<<"yy"<<endl;
                        if(ss){
                            if(node->parent == NULL){//直接结束 => 直接对node解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{ //到父亲节点中查找
                                 level = level-node->getPrefixLength()-1;
                                 keyByte = k[level];
                                  //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                 parentNode = node;
                                 n = N::getParent(node);
                                 break;
                            }
                            
                        }else{ //当前层右边第一个节点
                            //cout<<"dsd"<<endl;
                            nextNode = node->children[1];
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;
                        }
                    //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
                    }

                    if(node->keys[node->count-1] == keyByte){//右边界
                        //cout<<"n4 right bound"<<endl;
                        if(ss){//当前层左边倒数第二个节点 找的是左边的最大值
                            nextNode = node->children[node->count-2];
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;


                        }else{
                            if(node->parent == NULL){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{
                                //cout<<"right judge 1-1"<<endl;
                                level = level-node->getPrefixLength()-1;
                                //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                keyByte = k[level];
                                parentNode = node;
                                n = N::getParent(node);
                                break;
                            }
                        }

                    }

                    unsigned i,j=-1;
                    for(i=0;(i<node->count)&&(node->keys[i]<keyByte);i++)
                            j = i;
                    if(ss){
                            nextNode = node->children[j];//上层左边最大
                            if(N::isLeaf(nextNode)){
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;
                    }else{
                            nextNode = node->children[i+1];//上层右边最小
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;

                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;

                    }
                    break;
                }
                case  NTypes::N16: {
                    // cout<<"findFirst Node 16"<<endl;
                    unsigned i,j=-1;
                    auto node = static_cast<N16 *>(n);
                    //先判断边界
                    //cout<<"node->flipSign(node->keys[0]) = "<<+(node->flipSign(node->keys[0]))<<endl;
                    //等于左边界
                    if(node->flipSign(node->keys[0]) == keyByte ){
                        // cout<<"yy"<<endl;
                        if(ss){
                            if(node->parent == NULL){//直接结束
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{ //到父亲节点中查找
                                 level = level-node->getPrefixLength()-1;
                                 keyByte = k[level];
                                  //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                 parentNode = node;
                                 n = N::getParent(node);
                                 break;
                            }
                            
                        }else{ //当前层右边第一个节点
                            //cout<<"dsd"<<endl;
                            nextNode = node->children[1];
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;
                        }
                       //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
                    }

                    //等于右边界
                    if(node->flipSign(node->keys[node->count-1]) == keyByte ){
                        //cout<<"right judge"<<endl;
                        if(ss){//当前层左边倒数第二个节点 找的是左边的最大值
                            nextNode = node->children[node->count-2];
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;


                        }else{
                            if(node->parent == NULL){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{
                                //cout<<"right judge 1-1"<<endl;
                                level = level-node->getPrefixLength()-1;
                                //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                keyByte = k[level];
                                parentNode = node;
                                n = N::getParent(node);
                                break;
                            }
                        }

                    }
                    //cout<<"right judge 2-1"<<endl;
                    for(i=0;(i<node->count)&&(node->flipSign(node->keys[i])<keyByte);i++)// 第12次的时候刚好到达了边界
                            j = i;
                    if(ss){
                        nextNode = node->children[j];//上层左边最大
                        if(N::isLeaf(nextNode)){
                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;
                            return nextNode;
                        }
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return NULL;

                        // add node 节点解锁
                        node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return NULL; 

                        res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return NULL;
                        return res;
                    }else{
                            nextNode = node->children[i+1];//上层右边最小
                            if(N::isLeaf(nextNode)){

                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return  NULL;

                            res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;

                    }
                    break;  
                }
                case NTypes::N48: {
                    //cout<<"findFirst Node 48"<<endl;
                    auto node = static_cast<N48 *>(n);
                    unsigned pos1,pos2;
                    if(node->minl == keyByte ){  //同样需要加右边界判断（没有完成）
                        // cout<<"yy"<<endl;
                        if(ss){
                            if(node->parent == NULL){//直接结束
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{ //到父亲节点中查找
                                 level = level-node->getPrefixLength()-1;
                                  //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                 keyByte = k[level];
                                 parentNode = node;
                                 n = N::getParent(node);
                                 break;
                            }
                        }else{ // 右边的最小值
                            //cout<<"n48 bound not complete"<<endl;
                            pos2=keyByte;
                            while(node->childIndex[++pos2] == node->emptyMarker); //先对 pos2++ 比左边界大的第一个节点
                            nextNode = node->children[node->childIndex[pos2]];

                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;
                            
                        }
                    //return ss ? findFirst(n->parent,k[level],ss,level,k):N::minimum(node->children[1]);
                    }

                    if(node->maxr == keyByte){
                        if(ss){//左边最大
                           pos1=keyByte;
                           while(node->childIndex[--pos1] == node->emptyMarker); //先对 pos1减减 第二大的节点
                           nextNode = node->children[node->childIndex[pos1]];
                           if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;

                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;


                            res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;

                        }else{
                            if(node->parent == NULL){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{
                                    level = level-node->getPrefixLength()-1;
                                    //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                    keyByte = k[level];
                                    parentNode = node;
                                    n = N::getParent(node);
                                    break;
                            }

                        }
                         //cout<<"n48 right bound"<<endl;
                    }
                    pos1=pos2=keyByte;
                    //cout<<pos2<<endl;
                    //cout<<+key[depth-1]<<endl;
                    if(ss){
                        //cout<<pos1<<endl;
                        while(node->childIndex[--pos1] == node->emptyMarker);
                        //cout<<pos1<<endl;
                        nextNode = node->children[node->childIndex[pos1]];//上层左边最大
                        if(N::isLeaf(nextNode)){
                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;
                            return nextNode;
                        }
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return NULL;

                        // add node 节点解锁
                        node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return NULL;

                        res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return NULL;
                        return res;

                        //return N::maximum(node->children[node->childIndex[pos1]]);
                    }else{
                        //cout<<pos2<<endl;
                        while(node->childIndex[++pos2] == node->emptyMarker);

                        nextNode = node->children[node->childIndex[pos2]];//上层右边最小
                        if(N::isLeaf(nextNode)){
                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;
                            return nextNode;
                        }
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return NULL;

                        // add node 节点解锁
                        node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return NULL;

                        res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return NULL;
                        return res;
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
                        //cout<<"N256 left boundy"<<endl;
                        if(ss){
                            if(node->parent == NULL){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{
                                level = level-node->getPrefixLength()-1;
                                //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                keyByte = k[level];
                                parentNode = node;
                                n = N::getParent(node);
                                break;
                            }
                        }else{//右边第一个的最小值 这里感觉不需要加判断是否超过255
                            pos2=keyByte;
                            while(!node->children[++pos2] && pos2<256);
                            //cout<<pos2<<endl;
                            if(pos2 == 256){// 在查到更节点的时候，要右边最小节点 右边再无节点 直接返回null
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }

                            nextNode =node->children[pos2];
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;

                            res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;

                            //return  pos2 >255 ? NULL:N::minimum(node->children[pos2]);
                        }
                    }

                    if(node->maxr == keyByte){//最右边界判断  //也需要加一个判断
                        if(ss){
                            pos1=keyByte;
                            while(!node->children[--pos1]);

                            nextNode =node->children[pos1];
                            if(N::isLeaf(nextNode)){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;

                                return nextNode;
                            }
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return NULL;

                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;


                            res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return NULL;
                            return res;
                            //return N::maximum(node->children[pos1]);

                        }else{
                            if(node->parent == NULL){
                                // add node 节点解锁
                                node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return NULL;
                                return NULL;
                            }else{
                                    level = level-node->getPrefixLength()-1;
                                    //return findFirst(node->parent,k[level],ss,level,k);//其父亲左边最大
                                    keyByte = k[level];
                                    parentNode = node;
                                    n = N::getParent(node);
                                    break;
                            }
                        } 
                    }

                    pos1=pos2=keyByte;
                    //cout<<pos1<<endl;
                    if(ss){
                        while(!node->children[--pos1]);
                        //cout<<"pos1="<<pos1<<endl;
                        nextNode =node->children[pos1];
                        if(N::isLeaf(nextNode)){
                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;
                            return nextNode;
                        }
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return NULL;

                        // add node 节点解锁
                        node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return NULL;

                        res = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return NULL;
                        return res;
                    }else{
                        while(!node->children[++pos2] && pos2<256);

                        nextNode =node->children[pos2];
                        if(N::isLeaf(nextNode)){
                            // add node 节点解锁
                            node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return NULL;
                            return nextNode;
                        }
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return NULL;

                        // add node 节点解锁
                        node->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return NULL;

                        res = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return NULL;
                        return res;
                        //return  pos2 >255 ? NULL:N::minimum(node->children[pos2]);
                    } 
                    // return &(node->child[keyByte]);
                    break;
                }
         }
         uint64_t nv = n->readLockOrRestart(needRestart);//读取下一个节点的版本号
         if (needRestart) return NULL;

         parentNode->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
         if (needRestart) return NULL;
         v = nv;

       }//while
        
       throw; // Unreachable
    }

    void  Tree::findBoundary(N *node,const Key &k,uint32_t level,N *&first,N *&second,
                            bool &needRestart, uint64_t v, ThreadInfo &threadEpocheInfo){
        unsigned pos;
        N *nextNode = nullptr; 
        switch (node->getType()) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                //cout<<"n->count:"<<n->count<<endl;
                for(pos=0;(pos<n->count)&&(n->keys[pos]<k[level]);pos++);//相比原来的判断的level发生了改变
                //cout<<pos<<endl;
                if(pos == 0){
                        // get second  second = N::minimum(n->children[pos]);//当前层的最小值
                        nextNode = n->children[pos];
                        if(N::isLeaf(nextNode)){
                            second = nextNode;
                        }else{
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return ;
                            second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return ;
                        }
                        // get first first = findFirst(n->parent,k[level],true,level,k);//上层左边节点最大值
                        if(n->parent != NULL){
                           level = level- n->getPrefixLength()-1;
                           nextNode = n->parent;

                           uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                           if (needRestart) return ;

                           // add 对父亲节点解锁
                           n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                           if (needRestart) return;

                           first = findFirst(nextNode,k[level],true,level,k,needRestart,nv,threadEpocheInfo);
                           if (needRestart) return ;
                        }else{
                             // add 对父亲节点解锁
                            n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return;
                            first = NULL;
                        }
                        //first = findFirst(nodeParent[0],key[nodeParent[0]->prefixLength],true);//上层左边节点最大值
                }else if(pos == n->count){
                            
                            //get first first = N::maximum(n->children[pos-1]);//当前层节点的最大值
                            nextNode = n->children[pos-1];
                            if(N::isLeaf(nextNode)){
                                first = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            //get second second = findFirst(n->parent,k[level],false,level,k);//上层右边节点最小值
                            if(n->parent != NULL){
                                level = level- n->getPrefixLength()-1;
                                nextNode = n->parent;

                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;

                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;

                                second = findFirst(nextNode,k[level],false,level,k,needRestart,nv,threadEpocheInfo);
                                if (needRestart) return ;
                            }else{
                                // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                second = NULL;
                            }
                            //second = findFirst(nodeParent[0],key[nodeParent[0]->prefixLength],false);//上层右边节点最小值
                        } else{
                            //get first  first = N::maximum(n->children[pos-1]);//左边最大
                            nextNode = n->children[pos-1];
                            if(N::isLeaf(nextNode)){
                                first = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            //get second second = N::minimum(n->children[pos]);//右边最小
                            nextNode = n->children[pos];
                            if(N::isLeaf(nextNode)){
                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                second = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;

                                second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            
                        }
                break;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                //cout<<"not N16"<<endl;
                for(pos=0;(pos<n->count)&&n->flipSign(n->keys[pos])<k[level];pos++);
                //cout<<"pos="<<pos<<endl;
                // cout<<"n->count:"<<n->count<<endl;
                if(pos == 0){
                        //get second   second = N::minimum(n->children[pos]);
                        nextNode = n->children[pos];
                        if(N::isLeaf(nextNode)){
                            second = nextNode;
                        }else{
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return ;
                            second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return ;
                        }
                        // get first first = findFirst(n->parent,k[level],true,level,k);//上层左边节点最大值
                        if(n->parent != NULL){
                           level = level- n->getPrefixLength()-1;
                           nextNode = n->parent;

                            // add 对父亲节点解锁
                           n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                           if (needRestart) return;

                           uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                           if (needRestart) return ;

                           first = findFirst(nextNode,k[level],true,level,k,needRestart,nv,threadEpocheInfo);
                           if (needRestart) return ;
                        }else{
                             // add 对父亲节点解锁
                            n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return;
                            first = NULL;
                        }


                }else if(pos == n->count){
                           //get first first = N::maximum(n->children[pos-1]);//当前层节点的最大值
                            nextNode = n->children[pos-1];
                            if(N::isLeaf(nextNode)){
                                first = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            //get second second = findFirst(n->parent,k[level],false,level,k);//上层右边节点最小值
                            if(n->parent != NULL){
                                level = level- n->getPrefixLength()-1;
                                nextNode = n->parent;

                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;

                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;

                                second = findFirst(nextNode,k[level],false,level,k,needRestart,nv,threadEpocheInfo);
                                if (needRestart) return ;
                            }else{
                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                second = NULL;
                            }
                           
                        }else{
                            //get first  first = N::maximum(n->children[pos-1]);//左边最大
                            nextNode = n->children[pos-1];
                            if(N::isLeaf(nextNode)){
                                first = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            //get second second = N::minimum(n->children[pos]);//右边最小
                            nextNode = n->children[pos];
                            if(N::isLeaf(nextNode)){
                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                second = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                 // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                        }
                break;
              
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
               // cout<<"not N48"<<endl;

              // cout<<"min = "<<+(n->minl)<<" max ="<<+(n->maxr)<<endl;
               unsigned pos1,pos2;

               if(n->maxr < k[level]){ //右边界
                    //get first  first = N::maximum(n->children[pos-1]);//左边最大
                    nextNode = n->children[n->childIndex[n->maxr]];
                    if(N::isLeaf(nextNode)){
                        first = nextNode;
                    }else{
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return ;
                        first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return ;
                    }
                    //get second = findFirst(n->parent,k[level],false,level,k);//上层节点的最小值
                    if(n->parent != NULL){
                        level = level- n->getPrefixLength()-1;
                        nextNode = n->parent;

                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return ;

                         // add 对父亲节点解锁
                        n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return;

                        second = findFirst(nextNode,k[level],false,level,k,needRestart,nv,threadEpocheInfo);
                        if (needRestart) return ;
                    }else{
                         // add 对父亲节点解锁
                        n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return;
                        second = NULL;
                    }
                  
               }else if(k[level] < n->minl){ //左边界

                        nextNode = n->children[n->childIndex[n->minl]];
                        if(N::isLeaf(nextNode)){
                            second = nextNode;
                        }else{
                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) return ;
                            second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                            if (needRestart) return ;
                        }
                        // get first first = findFirst(n->parent,k[level],true,level,k);//上层左边节点最大值
                        if(n->parent != NULL){
                           level = level- n->getPrefixLength()-1;
                           nextNode = n->parent;

                           uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                           if (needRestart) return ;

                            // add 对父亲节点解锁
                           n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                           if (needRestart) return;

                           first = findFirst(nextNode,k[level],true,level,k,needRestart,nv,threadEpocheInfo);
                           if (needRestart) return ;
                        }else{
                             // add 对父亲节点解锁
                            n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                            if (needRestart) return;
                            first = NULL;
                        }
                  }
                  else{
                      pos1=pos2=k[level];
                     //cout<<+key[depth-1]<<endl;
                     while(n->childIndex[--pos1] == n->emptyMarker);
                     while(n->childIndex[++pos2] == n->emptyMarker);

                     nextNode = n->children[n->childIndex[pos1]];
                     if(N::isLeaf(nextNode)){
                        first = nextNode;
                     }else{
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return ;

                        first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return ;
                     }
                     //get second second = N::minimum(n->children[pos]);//右边最小
                     nextNode = n->children[n->childIndex[pos2]];
                     if(N::isLeaf(nextNode)){
                        // add 对父亲节点解锁
                        n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return;

                        second = nextNode;
                     }else{
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return ;

                         // add 对父亲节点解锁
                        n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return;

                        second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return ;
                     }
               }
               break;
              
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                //cout<<"not N256"<<endl;
                if(n->count == 0){//第一次的两次查找 索引为空的时候
                    // add 对父亲节点解锁
                    n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                    if (needRestart) return;
                        
                    return;
                }
                unsigned pos1,pos2;
                
                if(n->maxr < k[level]){
                    //cout<<"n256-1"<<endl;
                    //get first  first = N::maximum(n->children[pos-1]);//左边最大
                    nextNode = n->children[n->maxr];
                    if(N::isLeaf(nextNode)){
                        //cout<<"n256-1-1-1"<<endl;
                        first = nextNode;
                    }else{
                        //cout<<"n256-1-1-2"<<endl;
                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return ;
                        first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                        if (needRestart) return ;
                    }
                    //get second = findFirst(n->parent,k[level],false,level,k);//上层节点的最小值
                    if(n->parent != NULL){
                        //cout<<"n256-1-2-1"<<endl;
                        level = level- n->getPrefixLength()-1;
                        nextNode = n->parent;

                        uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                        if (needRestart) return ;

                        // add 对父亲节点解锁
                        n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return;
                        
                        second = findFirst(nextNode,k[level],false,level,k,needRestart,nv,threadEpocheInfo);
                        if (needRestart) return ;
                    }else{
                        //cout<<"n256-1-2-2"<<endl;
                        // add 对父亲节点解锁
                        n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                        if (needRestart) return;
                        
                        second = NULL;
                    }

                }else if(k[level] < n->minl){
                            nextNode = n->children[n->minl];
                            if(N::isLeaf(nextNode)){
                                second = nextNode;
                            }else{
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            // get first first = findFirst(n->parent,k[level],true,level,k);//上层左边节点最大值
                            if(n->parent != NULL){
                                level = level- n->getPrefixLength()-1;
                                nextNode = n->parent;

                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;

                                // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                
                                first = findFirst(nextNode,k[level],true,level,k,needRestart,nv,threadEpocheInfo);
                                if (needRestart) return ;
                            }else{
                                // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                        
                                first = NULL;
                            }
                              
                        }
                        else{
                            //cout<<"n256-3"<<endl;
                            // cout<<depth<<endl;
                            // cout<<+key[depth-1]<<endl;
                            pos1=pos2=k[level];
                            while(!(n->children[--pos1]));
                            //cout<<pos1<<endl;
                            while(!n->children[++pos2]);
                            //cout<<pos2<<endl;
                            nextNode = n->children[pos1];
                            if(N::isLeaf(nextNode)){
                                //cout<<"N256 3-1-1"<<endl;
                                first = nextNode;
                            }else{
                                //cout<<"N256 3-1-2"<<endl;
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;
                                first = maximum(nextNode, needRestart, nv, threadEpocheInfo);
                                if (needRestart) return ;
                            }
                            //get second second = N::minimum(n->children[pos]);//右边最小
                            nextNode = n->children[pos2];
                            //cout<<"get second"<<endl;
                            if(N::isLeaf(nextNode)){

                                // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;
                                //cout<<"N256 3-2-1"<<endl;
                                second = nextNode;
                            }else{
                                //cout<<"N256 3-2-2"<<endl;
                                uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                                if (needRestart) return ;

                                //cout<<"syd1"<<endl;

                                // add 对父亲节点解锁
                                n->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
                                if (needRestart) return;

                                //cout<<"syd2"<<endl;
                                //cout<<needRestart<<endl;
                                second = minimum(nextNode, needRestart, nv, threadEpocheInfo);
                                //cout<<"syd3"<<endl;
                                if (needRestart) return ;
                            }
                            //first = N::maximum(n->children[pos1]);
                            //second = N::minimum(n->children[pos2]);
                        }
                break;
                
            }
        }

    }

    // void  Tree::findBoundary(N *node,const Key &k,uint32_t level,N *&first,N *&second){
    //     unsigned pos;
    //     switch (node->getType()) {
    //         case NTypes::N4: {
    //             auto n = static_cast<N4 *>(node);
    //             //cout<<"n->count:"<<n->count<<endl;
    //             for(pos=0;(pos<n->count)&&(n->keys[pos]<k[level]);pos++);//相比原来的判断的level发生了改变
    //             //cout<<pos<<endl;
    //             if(pos == 0){
    //                     //cout<<"node 2-4-1"<<endl;
                        
    //                     second = N::minimum(n->children[pos]);//当前层的最小值
    //                     if(n->parent != NULL){
    //                        level = level- n->getPrefixLength()-1;
    //                        first = findFirst(n->parent,k[level],true,level,k);//上层左边节点最大值
    //                     }
    //                     //first = findFirst(nodeParent[0],key[nodeParent[0]->prefixLength],true);//上层左边节点最大值
    //             }else if(pos == n->count){
    //                         //cout<<"node 2-4-2"<<endl;
    //                         first = N::maximum(n->children[pos-1]);//当前层节点的最大值
    //                         //cout<<"first:"<< N::getLeaf(first)<<endl;
    //                         if(n->parent != NULL){
    //                             //cout<<level<<endl;
    //                             level = level- n->getPrefixLength()-1;
    //                             second = findFirst(n->parent,k[level],false,level,k);//上层右边节点最小值
    //                         }
    //                         //second = findFirst(nodeParent[0],key[nodeParent[0]->prefixLength],false);//上层右边节点最小值
    //                     } else{
    //                         //cout<<"node 2-4-3"<<endl;
    //                         first = N::maximum(n->children[pos-1]);//左边最大
    //                         second = N::minimum(n->children[pos]);//右边最小
    //                     }
    //             break;
                

               
    //         }
    //         case NTypes::N16: {
    //             auto n = static_cast<N16 *>(node);
    //             //cout<<"not N16"<<endl;
    //             for(pos=0;(pos<n->count)&&n->flipSign(n->keys[pos])<k[level];pos++);
    //             //cout<<"pos="<<pos<<endl;
    //             // cout<<"n->count:"<<n->count<<endl;
    //             if(pos == 0){ //应该有问题 判断不全面
    //                     //cout<<"node 2-16-1"<<endl;
    //                     second = N::minimum(n->children[pos]);
    //                     //if(judge(nodeParent[0],key[depth-2]))
    //                     if(n->parent){
    //                         level = level- n->getPrefixLength()-1;
    //                         first = findFirst(n->parent,k[level],true,level,k);//上层左边最大
    //                     }
    //             }else if(pos == n->count){
    //                         //cout<<"node 2-16-2"<<endl;
    //                         //cout<<"sn sn 2"<<endl;
    //                         first = N::maximum(n->children[pos-1]);
    //                         if(n->parent){
    //                           level = level- n->getPrefixLength()-1;
    //                           second = findFirst(n->parent,k[level],false,level,k);//上层右边节点最小值
    //                         }
    //                         // else{
    //                         //     cout<<"yy is sd"<<endl;
    //                         // }  
    //                     }else{
    //                         //cout<<"dsds"<<endl;
    //                        // cout<<"node 2-16-3"<<endl;
    //                         first = N::maximum(n->children[pos-1]);
    //                         second = N::minimum(n->children[pos]);
    //                     }
    //             break;
              
    //         }
    //         case NTypes::N48: {
    //             auto n = static_cast<N48 *>(node);
    //            // cout<<"not N48"<<endl;

    //           // cout<<"min = "<<+(n->minl)<<" max ="<<+(n->maxr)<<endl;
    //            unsigned pos1,pos2;

    //            if(n->maxr < k[level]){ //右边界
    //               first = N::maximum(n->children[n->childIndex[n->maxr]]);
    //               if(n->parent){
    //                   level = level- n->getPrefixLength()-1;
    //                   second = findFirst(n->parent,k[level],false,level,k);//上层节点的最小值
    //               }
    //            }else if(k[level] < n->minl){ //左边界
    //                     //cout<<"sn sn syd"<<endl;
    //                     second  = N::minimum(n->children[n->childIndex[n->minl]]);
    //                     //需要加判断 有可能要插入的值比当前最小值还小
    //                     level = level- n->getPrefixLength()-1;
    //                     first = findFirst(n->parent,k[level],true,level,k);//上层左边最大
    //               }
    //               else{
    //                   pos1=pos2=k[level];
    //                  //cout<<+key[depth-1]<<endl;
    //                  while(n->childIndex[--pos1] == n->emptyMarker);
    //                  while(n->childIndex[++pos2] == n->emptyMarker);
    //                  first = N::maximum(n->children[n->childIndex[pos1]]);
    //                  second = N::minimum(n->children[n->childIndex[pos2]]);
    //            }
    //            break;
              
    //         }
    //         case NTypes::N256: {
    //             auto n = static_cast<N256 *>(node);
    //             //cout<<"not N256"<<endl;
    //             if(n->count == 0){//第一次的两次查找 索引为空的时候
    //                 return;
    //             }
    //             unsigned pos1,pos2;
                
    //             if(n->maxr < k[level]){
    //                 //cout<<"sn sn sn N256 右"<<endl;
    //                 first = N::maximum(n->children[n->maxr]);
    //                 level = level- n->getPrefixLength()-1;
    //                 second = findFirst(n->parent,k[level],false,level,k);//上层节点的最小值
    //             }else if(k[level] < n->minl){
    //                         //cout<<"sn sn sn N256 左"<<endl;
    //                         second  = N::minimum(n->children[n->minl]);
    //                         //if(judge(nodeParent[0],key[depth-2]))
    //                         if(n->parent){
    //                             level = level- n->getPrefixLength()-1;
    //                             first = findFirst(n->parent,k[level],true,level,k);//上层左边最大
    //                         }    
    //                     }
    //                     else{
    //                         // cout<<depth<<endl;
    //                         // cout<<+key[depth-1]<<endl;
    //                         pos1=pos2=k[level];
    //                         while(!(n->children[--pos1]));
    //                         //cout<<pos1<<endl;
    //                         while(!n->children[++pos2]);
    //                         //cout<<pos2<<endl;
    //                         first = N::maximum(n->children[pos1]);
    //                         second = N::minimum(n->children[pos2]);
    //                     }
    //             break;
                
    //         }
    //     }

    // }


    //查找的代码 多线程下的查找代码（多参考一下并发insert 和 并发 Look）
     N* Tree::LookupLT(const Key &k, N *&first,N *&second,ThreadInfo &threadInfo, uint64_t kk) const{
        //cout<<"sdsd"<<endl;
        EpocheGuardReadonly epocheGuard(threadInfo);
        restart:
        bool needRestart = false;
        N *parentNode = nullptr;//需要保留父亲节点
        N *node;
        N* nextNode = nullptr;
    
        uint64_t v;//版本号
        uint32_t level = 0;//深度
        bool optimisticPrefixMatch = false;
        node = root;

        v = node->readLockOrRestart(needRestart);
        if (needRestart) goto restart;//判断是否要进行重启

        while (true) {
            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch://前缀不匹配的情况 //并发还没有写完
                    {
                    PCCompareResults prefixResult;
                    //cout<<"NoMatch level ="<<level<<endl;
                    auto newLevel = level- node->getPrefixLength();
                    //prefixResult = checkPrefixCompare(node, k, newLevel, loadKey);//其实可以直接使用这个进行下降？？？？
                    prefixResult = checkPrefixCompare(node, k, 0, level, loadKey, needRestart);//和start比较前缀返回比较的结果
                    if (needRestart) goto restart;

                    switch (prefixResult) {
                        //cout<<"1"<<endl;
                        case PCCompareResults::Bigger: {//内部节点比较大 //注意first有可能在上一层中查找
                            //cout<<"bigger"<<endl;

                             level = level- node->getPrefixLength();
                             nextNode = node->parent;
                             uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                             if (needRestart) goto restart;

                             first = findFirst(nextNode,k[level],true,level,k,needRestart,nv,threadInfo);//上层左边最大
                             if (needRestart) goto restart;
                             
                             second = minimum(node, needRestart, v, threadInfo);//当前节点的最小值（直接在minimum里面解锁)
                             if (needRestart) goto restart;

                             break;
                        }
                        case PCCompareResults::Equal: {
                            break;
                        }
                        case PCCompareResults::Smaller:{//内部节点小于key的时候就是small (需要完善 没写)
                            level = level - node->getPrefixLength();
                            nextNode = node->parent;

                            uint64_t nv = nextNode->readLockOrRestart(needRestart);//读取下一个节点的版本号
                            if (needRestart) goto restart;

                            second = findFirst(nextNode,k[level],false,level,k,needRestart,nv,threadInfo);//上层右边最小
                            if (needRestart) goto restart;

                            first = maximum(node, needRestart, v, threadInfo);//当前节点的最小值（直接在maximum里面解锁)
                            if (needRestart) goto restart;
                            //cout<<"Smaller"<<endl;

                            break;
                        }
                    }
                    cout<<"1 "<<kk<<" "<< ART_OLC::N::getLeaf(first)<<" "<<ART_OLC::N::getLeaf(second)<<endl;
                    //cout<<"first:"<< N::getLeaf(first)<<endl;
                    //cout<<"second:"<< N::getLeaf(second)<<endl;
                    return NULL;
                    }
                case CheckPrefixResult::OptimisticMatch://目前没有用上
                    optimisticPrefixMatch = true;
                    // fallthrough
                case CheckPrefixResult::Match:
                    if (k.getKeyLen() <= level) {
                        return NULL;
                    }

                    parentNode = node;//更新父亲节点
                    node = N::getChild(k[level], parentNode);//更新子节点（要查找的节点）
                    //这里是额外的检查，如果没有此检查，node可能指向空，可能使程序崩溃
                    parentNode->checkOrRestart(v,needRestart);//因为下面有可能孩子节点为空，就直接退出了，推出前要对父亲节点解锁（验证）
                    if (needRestart) goto restart;//判断是否需要重启
                    
                    //nextNode = N::getChild(k[level], node);

                    if (node == nullptr) {    //并发没有写完
                        //cout<<"nextNode is NULL"<<endl;
                        //cout<<"2"<<endl;
              
                        findBoundary(parentNode,k,level,first,second,needRestart,v,threadInfo);// 当查找到空节点的时候根据父亲节点的不同去进行查找
                        if (needRestart) goto restart;
                        // cout<<"first:"<< N::getLeaf(first)<<endl;
                        // cout<<"second:"<< N::getLeaf(second)<<endl;
                        cout<<"2 "<<kk<<" "<< ART_OLC::N::getLeaf(first)<<" "<<ART_OLC::N::getLeaf(second)<<endl;

                        return NULL;
                    }
                    if (N::isLeaf(node)) {  //目前还没有增加边界条件的判断  //并发还没有写完
                        // parentNode->readUnlockOrRestart(v, needRestart);//已经是最后一层 可以对父亲节点直接解锁即可
                        // if (needRestart) goto restart;
                        //cout<<"3"<<endl;
                        TID tid = N::getLeaf(node);
                        if (level < k.getKeyLen() - 1 || optimisticPrefixMatch) {//路径压缩的情况 需要比较当前key和叶子节点的key
                            //return checkKey(tid, k);//改成要返回一个节点
                            Key kt;
                            this->loadKey(tid,kt);
                            int res = std::memcmp(&k[0], &kt[0], k.getKeyLen());
                            if(res == 0){
                                return node;
                            }else if(res < 0){//叶子节点的值比较大 
                                  second = node;
                                  //cout<<"yy tx1"<<endl;
                                  first = findFirst(parentNode,k[level],true,level,k,needRestart,v,threadInfo);//上层左边最大
                                   if (needRestart) goto restart;
                                  //cout<<"bound not right"<<endl;
                            }else{
                                 first = node;
                                 if(parentNode){
                                     //cout<<"yy tx2"<<endl;
                                     second = findFirst(parentNode,k[level],false,level,k,needRestart,v,threadInfo);//上层节点的最小值
                                      if (needRestart) goto restart;
                                 }
                                    
                            }
                            // cout<<"first:"<< N::getLeaf(first)<<endl;
                            // cout<<"second:"<< N::getLeaf(second)<<endl;
                             cout<<"3 "<<kk<<" "<< ART_OLC::N::getLeaf(first)<<" "<<ART_OLC::N::getLeaf(second)<<endl;
                            return NULL;
                        }
                        return node;
                    }
                    level++;
            }               
            uint64_t nv = node->readLockOrRestart(needRestart);//读取下一个节点的版本号
            if (needRestart) goto restart;

            parentNode->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
            if (needRestart) goto restart;
            v = nv;
        
       }
    }
    
    

    //ART的修改最多影响2个节点 OLC lookup实现
    TID Tree::lookup(const Key &k, ThreadInfo &threadEpocheInfo) const {
        EpocheGuardReadonly epocheGuard(threadEpocheInfo);
        restart:
        bool needRestart = false;

        N *node;
        N *parentNode = nullptr;//需要保留父亲节点
        uint64_t v;//版本号
        uint32_t level = 0;//深度
        bool optimisticPrefixMatch = false;
        //先读取版本号再访问跟节点
        node = root;
        v = node->readLockOrRestart(needRestart);
        if (needRestart) goto restart;//判断是否要进行重启
        while (true) {
            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch://查不到，前缀就不匹配
                    node->readUnlockOrRestart(v, needRestart);
                    if (needRestart) goto restart;
                    return 0;
                case CheckPrefixResult::OptimisticMatch:
                    optimisticPrefixMatch = true;
                    // fallthrough
                case CheckPrefixResult::Match:
                    if (k.getKeyLen() <= level) { 
                        return 0;
                    }
                    parentNode = node;//更新父亲节点
                    node = N::getChild(k[level], parentNode);//更新子节点（要查找的节点）
                    //这里是额外的检查，如果没有此检查，node可能指向空，可能使程序崩溃
                    parentNode->checkOrRestart(v,needRestart);//因为下面有可能孩子节点为空，就直接退出了，推出前要对父亲节点解锁（验证）
                    if (needRestart) goto restart;//判断是否需要重启

                    if (node == nullptr) {//查不到，节点为空 //这里应该对父亲节点解锁吧？？？
                        return 0;
                    }
                    if (N::isLeaf(node)) {//查到了
                        parentNode->readUnlockOrRestart(v, needRestart);//同样需要验证父亲节点才能访问下一个节点
                        if (needRestart) goto restart;

                        TID tid = N::getLeaf(node);
                        if (level < k.getKeyLen() - 1 || optimisticPrefixMatch) {
                            return checkKey(tid, k);
                        }
                        return tid;
                    }
                    level++;
            }
            uint64_t nv = node->readLockOrRestart(needRestart);//读取下一个节点的版本号
            if (needRestart) goto restart;

            parentNode->readUnlockOrRestart(v, needRestart);//在访问下一个节点的时候，需要验证父亲节点
            if (needRestart) goto restart;
            v = nv;
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

    // test 并发 parent case
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


    void Tree::insert(const Key &k, TID tid, ThreadInfo &epocheInfo) {
        EpocheGuard epocheGuard(epocheInfo);//？？？
        restart:
        bool needRestart = false;

        N *node = nullptr;
        N *nextNode = root;
        N *parentNode = nullptr;
        uint8_t parentKey, nodeKey = 0;
        uint64_t parentVersion = 0;//与单线程相比新增加的
        uint32_t level = 0;

        while (true) {
            parentNode = node;
            parentKey = nodeKey;
            node = nextNode;//当前需要遍历的节点
            //对当前需要遍历的节点进行加读锁
            auto v = node->readLockOrRestart(needRestart);
            if (needRestart) goto restart;//判断是否进行重启

            uint32_t nextLevel = level;

            uint8_t nonMatchingKey;
            Prefix remainingPrefix;
            auto res = checkPrefixPessimistic(node, k, nextLevel, nonMatchingKey, remainingPrefix,
                                   this->loadKey, needRestart); // increases level
            if (needRestart) goto restart;
            switch (res) { // 在不匹配的情况下 需要新增内部节点
                //需要修改父亲节点
                case CheckPrefixPessimisticResult::NoMatch: {
                    //cout<<"NoMatch"<<endl;
                    //升级父节点的锁
                    parentNode->upgradeToWriteLockOrRestart(parentVersion, needRestart);
                    if (needRestart) goto restart;
                    //升级子节点的锁
                    node->upgradeToWriteLockOrRestart(v, needRestart);
                    if (needRestart) {//加写锁失败的话要对父亲节点解锁
                        parentNode->writeUnlock();
                        goto restart;
                    }
                    // 1) Create new node which will be parent of node, Set common prefix, level to this node
                    auto newNode = new N4(node->getPrefix(), nextLevel - level);

                    // 2)  add node and (tid, *k) as children
                    newNode->insert(k[nextLevel], N::setLeaf(tid));
                    newNode->insert(nonMatchingKey, node);

                    // 3) upgradeToWriteLockOrRestart, update parentNode to point to the new node, unlock
                    N::change(parentNode, parentKey, newNode);

                    newNode->parent = node->parent;// add newNode 's parent pointer 需要修改2处
                    parentNode->writeUnlock();//父亲节点已经修改完毕，对父亲节点解锁

                    // 4) update prefix of node, unlock
                    node->setPrefix(remainingPrefix,
                                    node->getPrefixLength() - ((nextLevel - level) + 1));

                    node->parent = newNode; //add               

                    node->writeUnlock();//子节点修改完毕，需要对字节点解锁
                    return;
                }
                case CheckPrefixPessimisticResult::Match:
                    break;
            }
            //在匹配的情况下要做的事情
            level = nextLevel;
            nodeKey = k[level];
            nextNode = N::getChild(nodeKey, node);
            node->checkOrRestart(v,needRestart);//这个是额外的检查，防止nextNode为空（论文里说这一步很重要。有时间验证一下）
            if (needRestart) goto restart;
            
            //nextNode 为空，要插入节点了（1.直接插入当前节点即可  2.面临的节点升级，变为更大的节点）注意这里传入了 epocheInfo？？？
            if (nextNode == nullptr) {
                N::insertAndUnlock(node, v, parentNode, parentVersion, parentKey, nodeKey, N::setLeaf(tid), needRestart, epocheInfo);
                if (needRestart) goto restart;
                return;
            }
            
            //nextNode 不为空，验证父节点（相当于给父节点解锁），不重启的话进入下一层
            if (parentNode != nullptr) {//第一次的时候父亲节点为空，所以需要对父亲判断的时候需要判空
                parentNode->readUnlockOrRestart(parentVersion, needRestart);
                if (needRestart) goto restart;
            }
            //查找到的是一个叶子节点
            if (N::isLeaf(nextNode)) {
                //cout<<"leaf"<<endl;
                //将当前节点的读锁升级为写锁
                node->upgradeToWriteLockOrRestart(v, needRestart);
                if (needRestart) goto restart;

                Key key;
                loadKey(N::getLeaf(nextNode), key);

                level++;
                uint32_t prefixLength = 0;
                while (key[level + prefixLength] == k[level + prefixLength]) {
                    prefixLength++;
                }

                auto n4 = new N4(&k[level], prefixLength);
                n4->insert(k[level + prefixLength], N::setLeaf(tid));
                n4->insert(key[level + prefixLength], nextNode);
                N::change(node, k[level - 1], n4);
                n4->parent = node;// add newNode 's parent pointer
                node->writeUnlock();//节点的写锁解锁
                return;
            }
            level++;
            parentVersion = v;
        }
    }

    
    inline typename Tree::CheckPrefixResult Tree::checkPrefix(N *n, const Key &k, uint32_t &level) {
        if (n->hasPrefix()) {
            if (k.getKeyLen() <= level + n->getPrefixLength()) {
                return CheckPrefixResult::NoMatch;
            }
            for (uint32_t i = 0; i < std::min(n->getPrefixLength(), maxStoredPrefixLength); ++i) {
                if (n->getPrefix()[i] != k[level]) {
                    return CheckPrefixResult::NoMatch;
                }
                ++level;
            }
            if (n->getPrefixLength() > maxStoredPrefixLength) {
                level = level + (n->getPrefixLength() - maxStoredPrefixLength);
                return CheckPrefixResult::OptimisticMatch;
            }
        }
        return CheckPrefixResult::Match;
    }

    typename Tree::CheckPrefixPessimisticResult Tree::checkPrefixPessimistic(N *n, const Key &k, uint32_t &level,
                                                                        uint8_t &nonMatchingKey,
                                                                        Prefix &nonMatchingPrefix,
                                                                        LoadKeyFunction loadKey, bool &needRestart) {
        if (n->hasPrefix()) {
            uint32_t prevLevel = level;
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    auto anyTID = N::getAnyChildTid(n, needRestart);//这个needRestart 基本用不到
                    if (needRestart) return CheckPrefixPessimisticResult::Match;
                    loadKey(anyTID, kt);
                }
                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];
                if (curKey != k[level]) {//当在前缀的某一个位置发生不匹配的情况
                    nonMatchingKey = curKey;
                    if (n->getPrefixLength() > maxStoredPrefixLength) {
                        if (i < maxStoredPrefixLength) {
                            auto anyTID = N::getAnyChildTid(n, needRestart);//这个needRestart 基本用不到
                            if (needRestart) return CheckPrefixPessimisticResult::Match;
                            loadKey(anyTID, kt);
                        }
                        memcpy(nonMatchingPrefix, &kt[0] + level + 1, std::min((n->getPrefixLength() - (level - prevLevel) - 1),
                                                                           maxStoredPrefixLength));
                    } else {
                        memcpy(nonMatchingPrefix, n->getPrefix() + i + 1, n->getPrefixLength() - i - 1);
                    }
                    return CheckPrefixPessimisticResult::NoMatch;
                }
                ++level;//前缀匹配的情况
            }
        }
        return CheckPrefixPessimisticResult::Match;//如果一直匹配的话返回Match
    }
    
    //  prefixResult = checkPrefixCompare(node, start, 0, level, loadKey, needRestart);//比较前缀
    typename Tree::PCCompareResults Tree::checkPrefixCompare(const N *n, const Key &k, uint8_t fillKey, uint32_t &level,
                                                        LoadKeyFunction loadKey, bool &needRestart) {
        if (n->hasPrefix()) {
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    auto anyTID = N::getAnyChildTid(n, needRestart);
                    if (needRestart) return PCCompareResults::Equal;
                    loadKey(anyTID, kt);
                }
                uint8_t kLevel = (k.getKeyLen() > level) ? k[level] : fillKey;//start 第level上的key

                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];// 当前内部节点 第level上的key
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

    typename Tree::PCEqualsResults Tree::checkPrefixEquals(const N *n, uint32_t &level, const Key &start, const Key &end,
                                                      LoadKeyFunction loadKey, bool &needRestart) {
        if (n->hasPrefix()) {
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    auto anyTID = N::getAnyChildTid(n, needRestart);
                    if (needRestart) return PCEqualsResults::BothMatch;
                    loadKey(anyTID, kt);
                }
                uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;

                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];
                if (curKey > startLevel && curKey < endLevel) {
                    return PCEqualsResults::Contained;//前缀是包含关系
                } else if (curKey < startLevel || curKey > endLevel) {
                    return PCEqualsResults::NoMatch;
                }
                ++level;
            }
        }
        return PCEqualsResults::BothMatch;//前缀是一样的
    }

    void Tree::remove(const Key &k, TID tid, ThreadInfo &threadInfo) {
        EpocheGuard epocheGuard(threadInfo);
        restart:
        bool needRestart = false;

        N *node = nullptr;
        N *nextNode = root;
        N *parentNode = nullptr;
        uint8_t parentKey, nodeKey = 0;
        uint64_t parentVersion = 0;
        uint32_t level = 0;

        while (true) {
            parentNode = node;
            parentKey = nodeKey;
            node = nextNode;
            auto v = node->readLockOrRestart(needRestart);
            if (needRestart) goto restart;

            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch:
                    node->readUnlockOrRestart(v, needRestart);
                    if (needRestart) goto restart;
                    return;
                case CheckPrefixResult::OptimisticMatch:
                    // fallthrough
                case CheckPrefixResult::Match: {
                    nodeKey = k[level];
                    nextNode = N::getChild(nodeKey, node);

                    node->checkOrRestart(v, needRestart);
                    if (needRestart) goto restart;

                    if (nextNode == nullptr) {
                        node->readUnlockOrRestart(v, needRestart);
                        if (needRestart) goto restart;
                        return;
                    }
                    if (N::isLeaf(nextNode)) {
                        if (N::getLeaf(nextNode) != tid) {
                            return;
                        }
                        assert(parentNode == nullptr || node->getCount() != 1);
                        if (node->getCount() == 2 && parentNode != nullptr) {
                            parentNode->upgradeToWriteLockOrRestart(parentVersion, needRestart);
                            if (needRestart) goto restart;

                            node->upgradeToWriteLockOrRestart(v, needRestart);
                            if (needRestart) {
                                parentNode->writeUnlock();
                                goto restart;
                            }
                            // 1. check remaining entries
                            N *secondNodeN;
                            uint8_t secondNodeK;
                            std::tie(secondNodeN, secondNodeK) = N::getSecondChild(node, nodeKey);
                            if (N::isLeaf(secondNodeN)) {
                                //N::remove(node, k[level]); not necessary
                                N::change(parentNode, parentKey, secondNodeN);

                                parentNode->writeUnlock();
                                node->writeUnlockObsolete();
                                this->epoche.markNodeForDeletion(node, threadInfo);
                            } else {
                                secondNodeN->writeLockOrRestart(needRestart);
                                if (needRestart) {
                                    node->writeUnlock();
                                    parentNode->writeUnlock();
                                    goto restart;
                                }

                                //N::remove(node, k[level]); not necessary
                                N::change(parentNode, parentKey, secondNodeN);
                                parentNode->writeUnlock();

                                secondNodeN->addPrefixBefore(node, secondNodeK);
                                secondNodeN->writeUnlock();

                                node->writeUnlockObsolete();
                                this->epoche.markNodeForDeletion(node, threadInfo);
                            }
                        } else {
                            N::removeAndUnlock(node, v, k[level], parentNode, parentVersion, parentKey, needRestart, threadInfo);
                            if (needRestart) goto restart;
                        }
                        return;
                    }
                    level++;
                    parentVersion = v;
                }
            }
        }
    }
    // void Tree::insert(const Key &k, TID tid, ThreadInfo &epocheInfo)
    //并发的范围查询
    bool Tree::lookupRange(const Key &start, const Key &end, Key &continueKey, TID result[],
                                std::size_t resultSize, std::size_t &resultsFound, ThreadInfo &threadEpocheInfo) const {
        for (uint32_t i = 0; i < std::min(start.getKeyLen(), end.getKeyLen()); ++i) {
            if (start[i] > end[i]) {//如果从一开始start[i]就大于end[i],肯定不对。相等的话i加1，
                resultsFound = 0;
                return false;
            } else if (start[i] < end[i]) {//从start比end小的某一位进行比较
                break;
            }
        }
        EpocheGuard epocheGuard(threadEpocheInfo);
        TID toContinue = 0;
        //std::cout<<"sdsd"<<std::endl;
        //相当于写了一个函数 功能未知：
        std::function<void(const N *)> copy = [&result, &resultSize, &resultsFound, &toContinue, &copy](const N *node) {
            //std::cout<<"copy"<<std::endl;
            if (N::isLeaf(node)) {
                if (resultsFound == resultSize) {
                     //std::cout<<"same"<<std::endl;
                    toContinue = N::getLeaf(node);
                    return;
                }
                result[resultsFound] = N::getLeaf(node);
                resultsFound++;
               // std::cout<<"resultsFound:"<<resultsFound<<std::endl;
            } else {
                std::cout<<"2lu"<<std::endl;
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
        
        //又写了另外一个函数
        std::function<void(N *, uint8_t, uint32_t, const N *, uint64_t)> findStart = [&copy, &start, &findStart, &toContinue, this](
                N *node, uint8_t nodeK, uint32_t level, const N *parentNode, uint64_t vp) {
            if (N::isLeaf(node)) {
                copy(node);//只要是到叶子节点调用上面函数的功能
                return;
            }
            uint64_t v;
            PCCompareResults prefixResult;//前缀比较的结果

            {
                readAgain:
                bool needRestart = false;
                v = node->readLockOrRestart(needRestart);
                if (needRestart) goto readAgain;

                prefixResult = checkPrefixCompare(node, start, 0, level, loadKey, needRestart);//和start比较前缀返回比较的结果
                if (needRestart) goto readAgain;

                parentNode->readUnlockOrRestart(vp, needRestart);
                if (needRestart) {//解锁失败要作的事情
                    readParentAgain:
                    vp = parentNode->readLockOrRestart(needRestart);
                    if (needRestart) goto readParentAgain;

                    node = N::getChild(nodeK, parentNode);

                    parentNode->readUnlockOrRestart(vp, needRestart);
                    if (needRestart) goto readParentAgain;

                    if (node == nullptr) {
                        return;
                    }
                    if (N::isLeaf(node)) {
                        copy(node);
                        return;
                    }
                    goto readAgain;
                }
                node->readUnlockOrRestart(v, needRestart);//再次解锁
                if (needRestart) goto readAgain;
            }
            //宋一东sb
            switch (prefixResult) {//根据前缀比较的结果执行
                case PCCompareResults::Bigger://内部节点在某一层大于要比较的关键字
                    copy(node);
                    break;
                case PCCompareResults::Equal: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    v = N::getChildren(node, startLevel, 255, children, childrenCount);//下一层所有的节点都满足，返回所有的节点，然后以此进行判断
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        N *n = std::get<1>(children[i]);
                        if (k == startLevel) {
                            //std::cout<<"findStart"<<std::endl;
                            findStart(n, k, level + 1, node, v);//相当于循环调用
                        } else if (k > startLevel) {
                            copy(n);
                        }
                        if (toContinue != 0) {
                            break;
                        }
                    }
                    break;
                }
                case PCCompareResults::Smaller://内部节点在某一层小于要比较的关键字
                    break;
            }
        };

        //第3个函数
        std::function<void(N *, uint8_t, uint32_t, const N *, uint64_t)> findEnd = [&copy, &end, &toContinue, &findEnd, this](
                N *node, uint8_t nodeK, uint32_t level, const N *parentNode, uint64_t vp) {
            if (N::isLeaf(node)) {
                return;
            }
            uint64_t v;
            PCCompareResults prefixResult;
            {
                readAgain:
                bool needRestart = false;
                v = node->readLockOrRestart(needRestart);
                if (needRestart) goto readAgain;

                prefixResult = checkPrefixCompare(node, end, 255, level, loadKey, needRestart);//和end比较前缀返回比较的结果
                if (needRestart) goto readAgain;

                parentNode->readUnlockOrRestart(vp, needRestart);//同上
                if (needRestart) {
                    readParentAgain:
                    vp = parentNode->readLockOrRestart(needRestart);
                    if (needRestart) goto readParentAgain;

                    node = N::getChild(nodeK, parentNode);

                    parentNode->readUnlockOrRestart(vp, needRestart);
                    if (needRestart) goto readParentAgain;

                    if (node == nullptr) {
                        return;
                    }
                    if (N::isLeaf(node)) {
                        return;
                    }
                    goto readAgain;
                }
                node->readUnlockOrRestart(v, needRestart);
                if (needRestart) goto readAgain;
            }
            
            switch (prefixResult) {
                case PCCompareResults::Smaller://和上面的正好相反
                    copy(node);
                    break;
                case PCCompareResults::Equal: {
                    uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    v = N::getChildren(node, 0, endLevel, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        N *n = std::get<1>(children[i]);
                        if (k == endLevel) {
                            findEnd(n, k, level + 1, node, v);
                        } else if (k < endLevel) {
                            copy(n);
                        }
                        if (toContinue != 0) {
                            break;
                        }
                    }
                    break;
                }
                case PCCompareResults::Bigger:
                    break;
            }
        };
        
        //下面的功能是？
        restart:
        bool needRestart = false;

        resultsFound = 0;

        uint32_t level = 0;
        N *node = nullptr;
        N *nextNode = root;
        N *parentNode;
        uint64_t v = 0;
        uint64_t vp;
        //宋一东sb
        //std::cout<<"3lu"<<std::endl;
        while (true) {
            parentNode = node;                                              
            vp = v;
            node = nextNode;
            PCEqualsResults prefixResult;
            v = node->readLockOrRestart(needRestart);
            if (needRestart) goto restart;
            prefixResult = checkPrefixEquals(node, level, start, end, loadKey, needRestart);//又去进行了前缀的匹配
            if (needRestart) goto restart;
            if (parentNode != nullptr) {
                parentNode->readUnlockOrRestart(vp, needRestart);
                if (needRestart) goto restart;
            }
            node->readUnlockOrRestart(v, needRestart);
            if (needRestart) goto restart;

            switch (prefixResult) {
                case PCEqualsResults::NoMatch: {
                    return false;
                }
                case PCEqualsResults::Contained: {
                    copy(node);
                    break;
                }
                case PCEqualsResults::BothMatch: {
                    //std::cout<<"BothMatch"<<std::endl;
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;
                    if (startLevel != endLevel) {
                        std::tuple<uint8_t, N *> children[256];
                        uint32_t childrenCount = 0;
                        v = N::getChildren(node, startLevel, endLevel, children, childrenCount);
                        for (uint32_t i = 0; i < childrenCount; ++i) {
                            const uint8_t k = std::get<0>(children[i]);
                            N *n = std::get<1>(children[i]);
                            if (k == startLevel) {
                                //std::cout<<"k ==startLevel"<<std::endl;
                                findStart(n, k, level + 1, node, v);
                            } else if (k > startLevel && k < endLevel) {
                                copy(n);
                                //std::cout<<"sss"<<std::endl;// 大部分会执行到这一步
                            } else if (k == endLevel) {
                                findEnd(n, k, level + 1, node, v);
                                //std::cout<<"k == endLevel"<<std::endl;
                            }
                            if (toContinue) {
                                break;
                            }
                        }
                    } else {
                        nextNode = N::getChild(startLevel, node);
                        node->readUnlockOrRestart(v, needRestart);
                        if (needRestart) goto restart;
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
    
}
